#ifndef HEADER_PIVISION_UNIQUEID
#define HEADER_PIVISION_UNIQUEID

#include <mutex>

class PiVisionUniqueId
{
private:
  static PiVisionUniqueId* instance;
  static std::mutex instanceMutex;
  uint32_t currId = 0u;

protected:

public:
  static PiVisionUniqueId* GetApi();
  uint32_t GetId();
};

#endif
