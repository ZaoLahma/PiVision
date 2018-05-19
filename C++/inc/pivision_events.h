#ifndef HEADER_PIVISION_EVENTS
#define HEADER_PIVISION_EVENTS

#include <string.h>

#include "eventdatabase.h"
#include "pivision_image_constants.h"

static const uint32_t PIVISION_EVENT_STOP                      = 0xFFFF0000u;
static const uint32_t PIVISION_EVENT_CONNECT_TO_SERVICE_REQ    = 0xFFFF0001u;
static const uint32_t PIVISION_EVENT_CONNECT_TO_SERVICE_CFM    = 0xFFFF0002u;
static const uint32_t PIVISION_EVENT_CONNECT_TO_SERVICE_REJ    = 0xFFFF0003u;
static const uint32_t PIVISION_EVENT_SERVICE_STATUS_IND        = 0xFFFF0004u;
static const uint32_t PIVISION_EVENT_SERVICE_DISCOVERY_TIMEOUT = 0xFFFF0005u;
static const uint32_t PIVISION_EVENT_NEW_DATA_IND              = 0xFFFF0006u;
static const uint32_t PIVISION_EVENT_SUBSCRIBE_SERVICE_IND     = 0xFFFF0007u;
static const uint32_t PIVISION_EVENT_SERVICE_AVAILABLE_IND     = 0xFFFF0008u;
static const uint32_t PIVISION_EVENT_SERVICE_UNAVAILABLE_IND   = 0xFFFF0009u;
static const uint32_t PIVISION_EVENT_SERVICE_PROVIDED_IND      = 0xFFFF000Au;

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

typedef std::vector<unsigned char> PiVisionDataBuf;

class PiVisionNewDataInd : public EventDataBase
{
private:
  PiVisionNewDataInd();

protected:

public:
  PiVisionNewDataInd(const PiVisionDataBuf _buf) :
  imageData(_buf)
  {

  }
  const PiVisionDataBuf imageData;
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

#endif
