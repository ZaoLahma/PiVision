#ifndef HEADER_PIVISION_EVENTS
#define HEADER_PIVISION_EVENTS

#include <memory>
#include <string>
#include <string.h>
#include <stdint.h>
#include <vector>

#include "eventdatabase.h"
#include "pivision_image_constants.h"

static const uint32_t PIVISION_EVENT_STOP                       = 0xFFFF0000u;
static const uint32_t PIVISION_EVENT_CONNECT_TO_SERVICE_REQ     = 0xFFFF0001u;
static const uint32_t PIVISION_EVENT_CONNECT_TO_SERVICE_CFM     = 0xFFFF0002u;
static const uint32_t PIVISION_EVENT_CONNECT_TO_SERVICE_REJ     = 0xFFFF0003u;
static const uint32_t PIVISION_EVENT_SERVICE_STATUS_IND         = 0xFFFF0004u;
static const uint32_t PIVISION_EVENT_SERVICE_DISCOVERY_TIMEOUT  = 0xFFFF0005u;
static const uint32_t PIVISION_EVENT_NEW_DATA_IND               = 0xFFFF0006u;
static const uint32_t PIVISION_EVENT_SUBSCRIBE_SERVICE_IND      = 0xFFFF0007u;
static const uint32_t PIVISION_EVENT_SERVICE_AVAILABLE_IND      = 0xFFFF0008u;
static const uint32_t PIVISION_EVENT_SERVICE_UNAVAILABLE_IND    = 0xFFFF0009u;
static const uint32_t PIVISION_EVENT_SERVICE_PROVIDED_IND       = 0xFFFF000Au;
static const uint32_t PIVISION_EVENT_PROVIDE_SERVICE_IND        = 0xFFFF000Bu;
static const uint32_t PIVISION_EVENT_GRAYSCALE_JOB_COMPLETE_IND = 0xFFFF000Cu;

class PiVisionConnectToServiceReq : public EventDataBase
{
private:
  PiVisionConnectToServiceReq();

protected:

public:
  PiVisionConnectToServiceReq(const uint32_t _serviceNo) : serviceNo(_serviceNo)
  {

  }
  const uint32_t serviceNo;
};

class PiVisionConnectToServiceCfm : public EventDataBase
{
private:
  PiVisionConnectToServiceCfm();

protected:

public:
  PiVisionConnectToServiceCfm(const uint32_t _serviceNo, const uint32_t _serviceId) :
  serviceNo(_serviceNo),
  serviceId(_serviceId)
  {

  }
  const uint32_t serviceNo;
  const uint32_t serviceId;
};

class PiVisionConnectToServiceRej : public EventDataBase
{
private:
  PiVisionConnectToServiceRej();

protected:

public:
  PiVisionConnectToServiceRej(const uint32_t _serviceNo) : serviceNo(_serviceNo)
  {

  }
  const uint32_t serviceNo;
};

class PiVisionServiceDiscoveryTimeout : public EventDataBase
{
private:
  PiVisionServiceDiscoveryTimeout();

protected:

public:
  PiVisionServiceDiscoveryTimeout(const uint32_t _serviceNo) : serviceNo(_serviceNo)
  {

  }
  const uint32_t serviceNo;
};

enum class PiVisionServiceStatus
{
  SERVICE_CONNECTED = 0u,
  SERVICE_DISCONNECTED,
  SERVICE_NOT_FOUND
};

class PiVisionConnectionStatusInd : public EventDataBase
{
private:
  PiVisionConnectionStatusInd();

protected:

public:
  PiVisionConnectionStatusInd(const PiVisionServiceStatus _status,
                           const uint32_t _serviceNo,
                           const int32_t _socketFd) :
  status(_status),
  serviceNo(_serviceNo),
  socketFd(_socketFd)
  {

  }
  const PiVisionServiceStatus status;
  const uint32_t serviceNo;
  const int32_t socketFd;
};

enum class PiVisionConnectionType
{
  PIVISION_SERVER = 0,
  PIVISION_CLIENT
};

class PiVisionData
{
private:
  PiVisionData();
  uint8_t* buf;
  const uint32_t capacity;
  uint32_t size;

protected:

public:
  PiVisionData(const uint32_t _capacity) : capacity(_capacity), size(0u)
  {
    buf = new uint8_t[capacity];
  }

  ~PiVisionData()
  {
    delete[] buf;
    buf = nullptr;
  }

  uint32_t Append(uint8_t* data, uint32_t dataSize)
  {
    uint32_t retVal = 0u;
    if((size + dataSize) <= capacity)
    {
      (void) memcpy(&buf[size], data, dataSize);
      size += dataSize;
      retVal = size;
    }
    return retVal;
  }

  uint32_t GetSize()
  {
    return size;
  }

  uint32_t GetCapacity()
  {
    return capacity;
  }

  uint8_t GetElementAt(const uint32_t index)
  {
    uint8_t retVal = 0x0u;

    if(index < size)
    {
      retVal = buf[index];
    }

    return retVal;
  }
};

class PiVisionNewDataInd : public EventDataBase
{
private:
  PiVisionNewDataInd();

protected:

public:
  PiVisionNewDataInd(const PiVisionConnectionType _connType, const std::shared_ptr<PiVisionData> _buf) :
  connType(_connType),
  data(_buf)
  {

  }
  const PiVisionConnectionType connType;
  const std::shared_ptr<PiVisionData> data;
};

class PiVisionSubscribeServiceInd : public EventDataBase
{
private:
  PiVisionSubscribeServiceInd();

protected:

public:
  const uint32_t serviceNo;
  PiVisionSubscribeServiceInd(const uint32_t _serviceNo) : serviceNo(_serviceNo)
  {

  }
};

class PiVisionServiceAvailableInd : public EventDataBase
{
private:
  PiVisionServiceAvailableInd();

protected:

public:
  const uint32_t serviceNo;
  PiVisionServiceAvailableInd(const uint32_t _serviceNo) : serviceNo(_serviceNo)
  {

  }
};

class PiVisionServiceUnavailableInd : public EventDataBase
{
private:
  PiVisionServiceUnavailableInd();

protected:

public:
  const uint32_t serviceNo;
  PiVisionServiceUnavailableInd(const uint32_t _serviceNo) : serviceNo(_serviceNo)
  {

  }
};

class PiVisionProvideServiceInd : public EventDataBase
{
private:
  PiVisionProvideServiceInd();

protected:

public:
  PiVisionProvideServiceInd(const uint32_t _serviceNo) : serviceNo(_serviceNo)
  {

  }
  const uint32_t serviceNo;
};

#endif
