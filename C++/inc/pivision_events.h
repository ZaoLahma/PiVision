#ifndef HEADER_PIVISION_EVENTS
#define HEADER_PIVISION_EVENTS

#include "eventdatabase.h"

static const uint32_t PIVISION_EVENT_STOP                      = 0xFFFF0000u;
static const uint32_t PIVISION_EVENT_CONNECT_TO_SERVICE_REQ    = 0xFFFF0001u;
static const uint32_t PIVISION_EVENT_CONNECT_TO_SERVICE_CFM    = 0xFFFF0002u;
static const uint32_t PIVISION_EVENT_CONNECT_TO_SERVICE_REJ    = 0xFFFF0003u;
static const uint32_t PIVISION_EVENT_SERVICE_STATUS_IND        = 0xFFFF0004u;
static const uint32_t PIVISION_EVENT_SERVICE_DISCOVERY_TIMEOUT = 0xFFFF0005u;

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
  PiVisionConnectToServiceCfm(const uint32_t _serviceNo) : serviceNo(_serviceNo)
  {

  }
  const uint32_t serviceNo;
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

class PiVisionServiceStatusInd : public EventDataBase
{
private:
  PiVisionServiceStatusInd();

protected:

public:
  PiVisionServiceStatusInd(const PiVisionServiceStatus _status,
                           const uint32_t _serviceNo) :
  status(_status),
  serviceNo(_serviceNo)
  {

  }
  const PiVisionServiceStatus status;
  const uint32_t serviceNo;
};

#endif
