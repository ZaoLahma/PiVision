#include "pivision_uniqueid.h"

PiVisionUniqueId* PiVisionUniqueId::instance = nullptr;
std::mutex PiVisionUniqueId::instanceMutex;

PiVisionUniqueId* PiVisionUniqueId::GetApi()
{
  std::unique_lock<std::mutex> instanceLock(instanceMutex);

  if(nullptr == instance)
  {
    instance = new PiVisionUniqueId();
  }

  return instance;
}

uint32_t PiVisionUniqueId::GetId()
{
  std::unique_lock<std::mutex> instanceLock(instanceMutex);

  const uint32_t retVal = currId;
  currId += 1u;
  return retVal;
}
