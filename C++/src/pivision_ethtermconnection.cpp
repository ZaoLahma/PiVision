#include <sys/socket.h>

#include "pivision_ethtermconnection.h"
#include "jobdispatcher.h"

PiVisionEthTermConnection::PiVisionEthTermConnection(const int32_t _socketFd) : socketFd(_socketFd)
{

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
      break;
    }
  }
}

void PiVisionEthTermConnection::Execute()
{
  while(1)
  {
    PiVisionDataBuf dataBuf;

    const uint32_t PAYLOAD_SIZE_HEADER_SIZE = 4u;
    Receive(PAYLOAD_SIZE_HEADER_SIZE, dataBuf);

    uint32_t payloadLength = 0u;
    for(uint32_t i = 0u; i < dataBuf.size(); ++i)
    {
      payloadLength = payloadLength | (dataBuf[i] << (i * 8u));
    }

    dataBuf.clear();
    Receive(payloadLength, dataBuf);

    auto newFrameInd = std::make_shared<PiVisionNewFrameInd>(dataBuf);
    JobDispatcher::GetApi()->RaiseEvent(PIVISION_EVENT_NEW_FRAME_IND, newFrameInd);
  }
}

void PiVisionEthTermConnection::HandleEvent(const uint32_t eventNo, std::shared_ptr<EventDataBase> dataPtr)
{

}
