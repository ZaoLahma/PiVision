#ifndef HEADER_PIVISION_SERVICEHANDLER
#define HEADER_PIVISION_SERVICEHANDLER

#include "eventlistenerbase.h"
#include <map>

typedef std::map<uint32_t, int32_t> PiVisionServiceToSocketMap;

class PiVisionServiceHandler : public EventListenerBase
{
private:
  int32_t LOCAL_SERVICE;
  int32_t REMOTE_SERVICE_DISCONNECTED;
  PiVisionServiceToSocketMap services;

protected:

public:
  PiVisionServiceHandler();
  void HandleEvent(const uint32_t eventNo, std::shared_ptr<EventDataBase> dataPtr);
};

#endif
