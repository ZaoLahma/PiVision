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
receivedAck(false),
ackEnabled(false)
{
  ackMsgBuf = std::make_shared<PiVisionDataBuf>();
  for(uint32_t i = 0u; i < sizeof(ackMsg); ++i)
  {
    ackMsgBuf->push_back(0x000000FF & (ackMsg >> (i * 8u)));
  }

  heartbeatMsgBuf = std::make_shared<PiVisionDataBuf>();
  for(uint32_t i = 0u; i < sizeof(heartbeatMsg); ++i)
  {
    heartbeatMsgBuf->push_back(0x000000FF & (heartbeatMsg >> (i * 8u)));
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
  JobDispatcher::GetApi()->UnsubscribeToEvent(HEARTBEAT_TIMEOUT, this);
  JobDispatcher::GetApi()->UnsubscribeToEvent(PIVISION_EVENT_STOP, this);

  std::unique_lock<std::mutex> lock(sendMutex);
}

void PiVisionEthTermConnection::Receive(const uint32_t numBytesToGet, std::shared_ptr<PiVisionDataBuf> dataBuf)
{
  const uint32_t MAX_CHUNK_SIZE = 256u;
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
      for(uint32_t i = 0; i < (uint32_t)chunkSize; ++i)
      {
        dataBuf->push_back(buffer[i]);
      }
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

void PiVisionEthTermConnection::Send(const std::shared_ptr<PiVisionDataBuf> dataBuf)
{
  std::unique_lock<std::mutex> lock(sendMutex);
  SendHeader(dataBuf);
  SendPayload(dataBuf);
}

void PiVisionEthTermConnection::SendHeader(const std::shared_ptr<PiVisionDataBuf> dataBuf)
{
  uint32_t dataSize = dataBuf->size();

  auto header = std::make_shared<PiVisionDataBuf>();
  for(uint32_t i = 0u; i < sizeof(uint32_t); ++i)
  {
    uint8_t byte = 0x000000FF & (dataSize >> i * 8);
    header->push_back(byte);
  }

  SendPayload(header);
}

void PiVisionEthTermConnection::SendPayload(const std::shared_ptr<PiVisionDataBuf> dataBuf)
{
  const uint32_t MAX_CHUNK_SIZE = 256u;
  unsigned char buffer[MAX_CHUNK_SIZE];

  const uint32_t MAX_SEND_ATTEMPTS = 10u;
  uint32_t numAttempts = 0u;

  uint32_t numBytesSent = 0u;
  uint32_t numBytesToSend = dataBuf->size();
  while(numBytesSent < numBytesToSend)
  {
    uint32_t maxChunkSize = std::min(MAX_CHUNK_SIZE, numBytesToSend - numBytesSent);
    (void) memset(buffer, 0, sizeof(buffer));

    uint32_t bufferIndex = 0u;
    for(uint32_t i = numBytesSent; i < numBytesSent + maxChunkSize; ++i)
    {
      buffer[bufferIndex] = (*dataBuf)[i];
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
    auto dataBuf = std::make_shared<PiVisionDataBuf>();

    const uint32_t PAYLOAD_SIZE_HEADER_SIZE = 4u;
    Receive(PAYLOAD_SIZE_HEADER_SIZE, dataBuf);

    uint32_t payloadLength = 0u;
    for(uint32_t i = 0u; i < dataBuf->size(); ++i)
    {
      payloadLength = payloadLength | ((*dataBuf)[i] << (i * 8u));
    }

    dataBuf->clear();
    Receive(payloadLength, dataBuf);

    if(0 < payloadLength)
    {
      bool isAck = false;
      if(sizeof(ackMsg) <= payloadLength)
      {
        isAck = ((*dataBuf)[0] == (*ackMsgBuf)[0] &&
                 (*dataBuf)[1] == (*ackMsgBuf)[1] &&
                 (*dataBuf)[2] == (*ackMsgBuf)[2] &&
                 (*dataBuf)[3] == (*ackMsgBuf)[3]);
      }

      bool isHeartbeat = false;
      if(sizeof(heartbeatMsg) <= payloadLength)
      {
        isHeartbeat = ((*dataBuf)[0] == (*heartbeatMsgBuf)[0] &&
                       (*dataBuf)[1] == (*heartbeatMsgBuf)[1] &&
                       (*dataBuf)[2] == (*heartbeatMsgBuf)[2] &&
                       (*dataBuf)[3] == (*heartbeatMsgBuf)[3]);
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
        Send(newData->dataBuf);
      }
    }
  }
  else if(HEARTBEAT_TIMEOUT == eventNo)
  {
    auto heartbeat = std::static_pointer_cast<PiVisionEthTermConnectionHBTimeout>(dataPtr);
    if(heartbeat->id == socketFd)
    {
      Send(heartbeatMsgBuf);
      JobDispatcher::GetApi()->RaiseEventIn(HEARTBEAT_TIMEOUT, heartbeat, HEARTBEAT_PERIODICITY);
    }
  }
  else
  {
    switch(eventNo)
    {
      case PIVISION_EVENT_STOP:
        JobDispatcher::GetApi()->UnsubscribeToEvent(serviceNo + 1, this);
        active = false;
      break;
      default:
      break;
    }
  }
}
