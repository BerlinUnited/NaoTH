/**
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 */

#include "Actuator.h"
#include "Core/Tools/SwapSpace/SwapSpace.h"

Actuator::Actuator()
{
}

Actuator::~Actuator()
{
}


#define REG_OUTPUT(R) \
  platformInterface.registerCognitionOutput(get##R())

void Actuator::init(naoth::PlatformDataInterface& platformInterface)
{
  REG_OUTPUT(CameraSettingsRequest);
  REG_OUTPUT(LEDData);
  REG_OUTPUT(IRSendData);
  REG_OUTPUT(UltraSoundSendData);
  REG_OUTPUT(SoundPlayData);
}//end init

void Actuator::execute()
{  
  //STOPWATCH_STOP("Cognition-Main");

  //STOPWATCH_START("Cognition-PushSwap");
  // data to motion
  SwapSpace::getInstance().theCognitionCache.push(
    getHeadMotionRequest(),
    getMotionRequest()
    );
  //STOPWATCH_STOP("Cognition-PushSwap");
}//end execute
