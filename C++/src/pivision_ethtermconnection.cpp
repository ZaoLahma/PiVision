#include <sys/socket.h>
#include <unistd.h>

#include "pivision_ethtermconnection.h"
#include "jobdispatcher.h"
#include "pivision_events.h"
#include "pivision_macros.h"

PiVisionEthTermConnection::PiVisionEthTermConnection(const uint32_t _serviceNo,
                                                     const int32_t _socketFd) :
active(true),
serviceNo(_serviceNo),
socketFd(_socketFd)
{
  JobDispatcher::GetApi()->Log("New connection for service: %u", serviceNo);
  JobDispatcher::GetApi()->SubscribeToEvent(serviceNo + 1u, this);
  JobDispatcher::GetApi()->SubscribeToEvent(PIVISION_EVENT_STOP, this);
}

PiVisionEthTermConnection::~PiVisionEthTermConnection()
{
  JobDispatcher::GetApi()->UnsubscribeToEvent(serviceNo + 1u, this);
  JobDispatcher::GetApi()->UnsubscribeToEvent(PIVISION_EVENT_STOP, this);
}

void PiVisionEthTermConnection::Receive(const uint32_t numBytesToGet, PiVisionDataBuf& dataBuf)
{
  const uint32_t MAX_CHUNK_SIZE = 256u;
  unsigned char buffer[MAX_CHUNK_SIZE];

  int32_t numBytesReceived = 0;
  while((uint32_t)numBytesReceived < numBytesToGet)
  {
    uint32_t maxChunkSize = std::min(MAX_CHUNK_SIZE, numBytesToGet - numBytesReceived);
    (void) memset(buffer, 0, sizeof(buffer));
    int32_t chunkSize = recv(socketFd,
                             buffer,
                             maxChunkSize,
                             0);

    if(chunkSize > 0)
    {
      numBytesReceived += chunkSize;
      for(uint32_t i = 0; i < (uint32_t)chunkSize; ++i)
      {
        dataBuf.push_back(buffer[i]);
      }
    }
    else
    {
      if(!active)
      {
        break;
      }
    }
  }
}

void PiVisionEthTermConnection::Send(const PiVisionDataBuf& dataBuf)
{
  std::unique_lock<std::mutex> lock(sendMutex);

  const uint32_t MAX_CHUNK_SIZE = 256u;
  unsigned char buffer[MAX_CHUNK_SIZE];

  uint32_t numBytesSent = 0u;
  uint32_t numBytesToSend = dataBuf.size();
  while(numBytesSent < numBytesToSend)
  {
    uint32_t maxChunkSize = std::min(MAX_CHUNK_SIZE, numBytesToSend - numBytesSent);
    (void) memset(buffer, 0, sizeof(buffer));

    uint32_t bufferIndex = 0u;
    for(uint32_t i = numBytesSent; i < numBytesSent + maxChunkSize; ++i)
    {
      buffer[bufferIndex] = dataBuf[i];
      bufferIndex += 1u;
    }

  //  JobDispatcher::GetApi()->Log("Trying to send %u bytes, numBytesToSend: %u (total %u bytes sent) for service %u", maxChunkSize, numBytesToSend, numBytesSent, serviceNo);

    int32_t chunkSize = send(socketFd,
                             buffer,
                             maxChunkSize,
                             0);

  //  JobDispatcher::GetApi()->Log("Sent %d bytes", chunkSize);

    if(chunkSize >= 0)
    {
      numBytesSent += chunkSize;
    }
    else
    {
      if(!active)
      {
        break;
      }
    }
  }
  JobDispatcher::GetApi()->Log("Send return");
}

void PiVisionEthTermConnection::Execute()
{
  while(active)
  {
    PiVisionDataBuf dataBuf;

    const uint32_t PAYLOAD_SIZE_HEADER_SIZE = 4u;
    Receive(PAYLOAD_SIZE_HEADER_SIZE, dataBuf);

    uint32_t payloadLength = 0u;
    for(uint32_t i = 0u; i < dataBuf.size(); ++i)
    {
      payloadLength = payloadLength | (dataBuf[i] << (i * 8u));
    }

    JobDispatcher::GetApi()->Log("Service: %u, payloadLength: %u", serviceNo, payloadLength);

    dataBuf.clear();
    Receive(payloadLength, dataBuf);

    auto newData = std::make_shared<PiVisionNewDataInd>(dataBuf);
    JobDispatcher::GetApi()->RaiseEvent(serviceNo, newData);
  }

  auto serviceUnavailable = std::make_shared<PiVisionServiceUnavailableInd>(this->serviceNo);
  JobDispatcher::GetApi()->RaiseEvent(PIVISION_EVENT_SERVICE_UNAVAILABLE_IND, serviceUnavailable);

  close(socketFd);
}

void PiVisionEthTermConnection::HandleEvent(const uint32_t eventNo, std::shared_ptr<EventDataBase> dataPtr)
{
  if(serviceNo + 1u == eventNo)
  {
    auto newData = std::static_pointer_cast<PiVisionNewDataInd>(dataPtr);
    uint32_t dataSize = newData->dataBuf.size();
    JobDispatcher::GetApi()->Log("Service: %u dataSize: 0x%X", serviceNo, dataSize);

    PiVisionDataBuf header;
    for(uint32_t i = 0u; i < sizeof(uint32_t); ++i)
    {
      uint8_t byte = 0x000000FF & (dataSize >> i * 8);
      header.push_back(byte);
    }
    
    Send(header);
    Send(newData->dataBuf);
  }
  else
  {
    switch(eventNo)
    {
      case PIVISION_EVENT_STOP:
        active = false;
      break;
      default:
      break;
    }
  }
}
