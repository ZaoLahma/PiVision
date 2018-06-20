#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

#include "pivision_ethtermconnection.h"
#include "jobdispatcher.h"
#include "pivision_events.h"
#include "pivision_macros.h"
#include <stdio.h>

PiVisionEthTermConnection::PiVisionEthTermConnection(const PiVisionConnectionType _connType,
                                                     const uint32_t _serviceNo,
                                                     const int32_t _socketFd) :
active(true),
connType(_connType),
serviceNo(_serviceNo),
socketFd(_socketFd),
ackMsg(0xDEADBEEFu),
heartbeatMsg(0xBEA1BEA1),
HEARTBEAT_TIMEOUT(0xBEA1BEA1u),
HEARTBEAT_PERIODICITY(200u),
receivedAck(true),
ackEnabled(true),
lastHeartbeatReceived(false)
{
  ackMsgBuf = std::make_shared<PiVisionData>(sizeof(ackMsg));
  for(uint32_t i = 0u; i < ackMsgBuf->GetCapacity(); ++i)
  {
    uint8_t byte = (0x000000FF & (ackMsg >> (i * 8u)));
    ackMsgBuf->Append(&byte, 1u);
  }

  heartbeatMsgBuf = std::make_shared<PiVisionData>(sizeof(heartbeatMsg));
  for(uint32_t i = 0u; i < heartbeatMsgBuf->GetCapacity(); ++i)
  {
    uint8_t byte = (0x000000FF & (heartbeatMsg >> (i * 8u)));
    heartbeatMsgBuf->Append(&byte, 1u);
  }

  JobDispatcher::GetApi()->Log("New connection for service: %u", serviceNo);
  JobDispatcher::GetApi()->SubscribeToEvent(serviceNo + 1u, this);
  JobDispatcher::GetApi()->SubscribeToEvent(HEARTBEAT_TIMEOUT, this);
  JobDispatcher::GetApi()->SubscribeToEvent(PIVISION_EVENT_STOP, this);

  auto heartbeat = std::make_shared<PiVisionEthTermConnectionHBTimeout>(socketFd);
  JobDispatcher::GetApi()->RaiseEventIn(HEARTBEAT_TIMEOUT, heartbeat, HEARTBEAT_PERIODICITY);
}

PiVisionEthTermConnection::~PiVisionEthTermConnection()
{
  JobDispatcher::GetApi()->UnsubscribeToEvent(serviceNo + 1u, this);
  JobDispatcher::GetApi()->UnsubscribeToEvent(PIVISION_EVENT_STOP, this);

  JobDispatcher::GetApi()->Log("Service %u waiting for last heartbeat", serviceNo);

  /* Wait for the lingering heartbeat timeout */
  std::unique_lock<std::mutex> lock(exitMutex);
  if(!lastHeartbeatReceived)
  {
    lastExecNotification.wait(lock);
  }
  JobDispatcher::GetApi()->UnsubscribeToEvent(HEARTBEAT_TIMEOUT, this);
  JobDispatcher::GetApi()->Log("Destroying connection instance for service %u", serviceNo);
}

void PiVisionEthTermConnection::Receive(const uint32_t numBytesToGet, std::shared_ptr<PiVisionData> dataBuf)
{
  const uint32_t MAX_CHUNK_SIZE = 4096u;
  unsigned char buffer[MAX_CHUNK_SIZE];

  const uint32_t MAX_RECEIVE_ATTEMPTS = 10u;
  uint32_t numReceiveAttempts = 0u;

  int32_t numBytesReceived = 0;
  while((uint32_t)numBytesReceived < numBytesToGet)
  {
    numReceiveAttempts += 1u;

    uint32_t maxChunkSize = std::min(MAX_CHUNK_SIZE, numBytesToGet - numBytesReceived);
    (void) memset(buffer, 0, sizeof(buffer));

    int32_t chunkSize = -1;
    int32_t error = 0;
    socklen_t len = sizeof(error);
    (void) getsockopt(socketFd, SOL_SOCKET, SO_ERROR, &error, &len);
    if(0 == error)
    {
      struct sigaction new_actn, old_actn;
      new_actn.sa_handler = SIG_IGN;
      sigemptyset(&new_actn.sa_mask);
      new_actn.sa_flags = 0;
      sigaction(SIGPIPE, &new_actn, &old_actn);
      chunkSize = recv(socketFd,
                       buffer,
                       maxChunkSize,
                       0);
      sigaction(SIGPIPE, &old_actn, NULL);
    }
    else
    {
      JobDispatcher::GetApi()->Log("Service %u lost server", serviceNo);
      JobDispatcher::GetApi()->UnsubscribeToEvent(serviceNo + 1, this);
      active = false;
    }

    if(chunkSize > 0)
    {
      numReceiveAttempts = 0u;
      numBytesReceived += chunkSize;
      dataBuf->Append(buffer, chunkSize);
    }
    else if(!active)
    {
      break;
    }
    else if(numReceiveAttempts > MAX_RECEIVE_ATTEMPTS)
    {
      JobDispatcher::GetApi()->Log("Disconnecting %u due to max receive attempts exceeded", serviceNo);
      JobDispatcher::GetApi()->UnsubscribeToEvent(serviceNo + 1, this);
      active = false;
      break;
    }
  }
}

void PiVisionEthTermConnection::Send(const std::shared_ptr<PiVisionData> dataBuf)
{
  std::unique_lock<std::mutex> lock(sendMutex);
  SendHeader(dataBuf);
  SendPayload(dataBuf);
}

void PiVisionEthTermConnection::SendHeader(const std::shared_ptr<PiVisionData> dataBuf)
{
  uint32_t dataSize = dataBuf->GetSize();

  auto header = std::make_shared<PiVisionData>(sizeof(uint32_t));
  for(uint32_t i = 0u; i < header->GetCapacity(); ++i)
  {
    uint8_t byte = 0x000000FF & (dataSize >> i * 8);
    header->Append(&byte, 1u);
  }

  SendPayload(header);
}

void PiVisionEthTermConnection::SendPayload(const std::shared_ptr<PiVisionData> dataBuf)
{
  const uint32_t MAX_CHUNK_SIZE = 4096u;
  unsigned char buffer[MAX_CHUNK_SIZE];

  const uint32_t MAX_SEND_ATTEMPTS = 10u;
  uint32_t numAttempts = 0u;

  uint32_t numBytesSent = 0u;
  uint32_t numBytesToSend = dataBuf->GetSize();
  while(numBytesSent < numBytesToSend)
  {
    uint32_t maxChunkSize = std::min(MAX_CHUNK_SIZE, numBytesToSend - numBytesSent);
    (void) memset(buffer, 0, sizeof(buffer));

    uint32_t bufferIndex = 0u;
    for(uint32_t i = numBytesSent; i < numBytesSent + maxChunkSize; ++i)
    {
      buffer[bufferIndex] = dataBuf->GetElementAt(i);
      bufferIndex += 1u;
    }

    int32_t chunkSize = -1;
    int32_t error = 0;
    socklen_t len = sizeof(error);
    (void) getsockopt(socketFd, SOL_SOCKET, SO_ERROR, &error, &len);
    if(0 == error)
    {
      numAttempts += 1u;
      struct sigaction new_actn, old_actn;
      new_actn.sa_handler = SIG_IGN;
      sigemptyset(&new_actn.sa_mask);
      new_actn.sa_flags = 0;
      sigaction(SIGPIPE, &new_actn, &old_actn);
      chunkSize = send(socketFd,
                      buffer,
                      maxChunkSize,
                      0);
      sigaction(SIGPIPE, &old_actn, NULL);
    }
    else
    {
      JobDispatcher::GetApi()->Log("Client at service %u disconnected", serviceNo);
      JobDispatcher::GetApi()->UnsubscribeToEvent(serviceNo + 1u, this);
      active = false;
    }

    if(chunkSize >= 0)
    {
      numAttempts = 0u;
      numBytesSent += chunkSize;
    }
    else if(numAttempts > MAX_SEND_ATTEMPTS)
    {
      JobDispatcher::GetApi()->Log("Disconnecting %u due to max send attempts exceeded", serviceNo);
      JobDispatcher::GetApi()->UnsubscribeToEvent(serviceNo + 1u, this);
      active = false;
      break;
    }
  }
}

void PiVisionEthTermConnection::Execute()
{
  while(active)
  {
    const uint32_t PAYLOAD_SIZE_HEADER_SIZE = 4u;

    auto dataBuf = std::make_shared<PiVisionData>(PAYLOAD_SIZE_HEADER_SIZE);

    Receive(PAYLOAD_SIZE_HEADER_SIZE, dataBuf);

    uint32_t payloadLength = 0u;
    for(uint32_t i = 0u; i < dataBuf->GetSize(); ++i)
    {
      payloadLength = payloadLength | (dataBuf->GetElementAt(i) << (i * 8u));
    }

    dataBuf = std::make_shared<PiVisionData>(payloadLength);
    Receive(payloadLength, dataBuf);

    if(0 < payloadLength)
    {
      bool isAck = false;
      if(sizeof(ackMsg) <= payloadLength)
      {
        isAck = (dataBuf->GetElementAt(0) == ackMsgBuf->GetElementAt(0 )&&
                 dataBuf->GetElementAt(1) == ackMsgBuf->GetElementAt(1) &&
                 dataBuf->GetElementAt(2) == ackMsgBuf->GetElementAt(2) &&
                 dataBuf->GetElementAt(3) == ackMsgBuf->GetElementAt(3));
      }

      bool isHeartbeat = false;
      if(sizeof(heartbeatMsg) <= payloadLength)
      {
        isHeartbeat = (dataBuf->GetElementAt(0) == heartbeatMsgBuf->GetElementAt(0) &&
                       dataBuf->GetElementAt(1) == heartbeatMsgBuf->GetElementAt(1) &&
                       dataBuf->GetElementAt(2) == heartbeatMsgBuf->GetElementAt(2) &&
                       dataBuf->GetElementAt(3) == heartbeatMsgBuf->GetElementAt(3));
      }

      if((!isAck) && (!isHeartbeat))
      {
        auto newData = std::make_shared<PiVisionNewDataInd>(connType, dataBuf);
        JobDispatcher::GetApi()->RaiseEvent(serviceNo, newData);
        if(ackEnabled)
        {
          Send(ackMsgBuf);
        }
      }
      else if(isAck)
      {
        receivedAck = true;
        if(!ackEnabled)
        {
          JobDispatcher::GetApi()->Log("PiVisionEthTermConnection - Ack enabled for service %u", serviceNo);
        }
        ackEnabled = true;
      }
      else
      {
        /* heartbeat - do nothing */
      }
    }
  }

  JobDispatcher::GetApi()->UnsubscribeToEvent(serviceNo + 1u, this);

  JobDispatcher::GetApi()->Log("PiVisionEthTermConnection - Disconnected from service %u", serviceNo);

  PiVisionServiceStatus status = PiVisionServiceStatus::SERVICE_DISCONNECTED;
  auto connectionStatusIndInd = std::make_shared<PiVisionConnectionStatusInd>(status, serviceNo, -1);
  JobDispatcher::GetApi()->RaiseEvent(PIVISION_EVENT_SERVICE_STATUS_IND, connectionStatusIndInd);

  close(socketFd);
}

void PiVisionEthTermConnection::HandleEvent(const uint32_t eventNo, std::shared_ptr<EventDataBase> dataPtr)
{
  if(serviceNo + 1u == eventNo)
  {
    if((receivedAck) || (!ackEnabled))
    {
      auto newData = std::static_pointer_cast<PiVisionNewDataInd>(dataPtr);

      if(connType == newData->connType)
      {
        receivedAck = false;
        Send(newData->data);
      }
    }
  }
  else if(HEARTBEAT_TIMEOUT == eventNo)
  {
    auto heartbeat = std::static_pointer_cast<PiVisionEthTermConnectionHBTimeout>(dataPtr);
    if(heartbeat->id == socketFd)
    {
      if(active)
      {
        Send(heartbeatMsgBuf);
        JobDispatcher::GetApi()->RaiseEventIn(HEARTBEAT_TIMEOUT, heartbeat, HEARTBEAT_PERIODICITY);
      }
      else
      {
        std::unique_lock<std::mutex> lock(exitMutex);
        lastExecNotification.notify_one();
        lastHeartbeatReceived = true;
      }
    }
  }
  else if(PIVISION_EVENT_STOP == eventNo)
  {
    JobDispatcher::GetApi()->UnsubscribeToEvent(serviceNo + 1, this);
    active = false;
  }
}
