/**
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 */

#include "Actuator.h"

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
  REG_OUTPUT(SoundData);
}//end init

void Actuator::execute()
{  
  /*
  STOPWATCH_STOP("Cognition-Main");

  STOPWATCH_START("Cognition-SetData");
  // data to robot
  thePlatformInterface->setCognitionOutput();
  STOPWATCH_STOP("Cognition-SetData");

  STOPWATCH_START("Cognition-PushSwap");
  // data to motion
  theMotionRequest.time = theMotionStatus.time;
  SwapSpace::getInstance().theCognitionCache.push(
    theHeadMotionRequest,
    theMotionRequest
    );
  STOPWATCH_STOP("Cognition-PushSwap");
  */
}//end execute
