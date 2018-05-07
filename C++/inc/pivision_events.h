#ifndef HEADER_PIVISION_EVENTS
#define HEADER_PIVISION_EVENTS

#include "eventdatabase.h"

static const uint32_t PIVISION_EVENT_STOP                   = 0xFFFF0000u;
static const uint32_t PIVISION_EVENT_CONNECT_TO_SERVICE_REQ = 0xFFFF0001u;
static const uint32_t PIVISION_EVENT_CONNECT_TO_SERVICE_CFM = 0xFFFF0002u;
static const uint32_t PIVISION_EVENT_CONNECT_TO_SERVICE_REJ = 0xFFFF0003u;

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

#endif
