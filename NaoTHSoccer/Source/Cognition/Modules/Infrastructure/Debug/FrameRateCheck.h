#ifndef FRAMERATECHECK_H
#define FRAMERATECHECK_H

#include <ModuleFramework/Module.h>
#include <Representations/Infrastructure/LEDRequest.h>
#include <Representations/Infrastructure/FrameInfo.h>

BEGIN_DECLARE_MODULE(FrameRateCheck)
  REQUIRE(FrameInfo)
  PROVIDE(FrameRateCheckLEDRequest)
END_DECLARE_MODULE(FrameRateCheck)

class FrameRateCheck : public FrameRateCheckBase
{
public:
  FrameRateCheck();
  virtual ~FrameRateCheck();

  virtual void execute();
private:
  FrameInfo lastFrameInfo;

};

#endif // FRAMERATECHECK_H
