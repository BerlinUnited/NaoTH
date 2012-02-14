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

void Actuator::init(naoth::PlatformInterfaceBase& platformInterface)
{
  REG_OUTPUT(LEDData);
  REG_OUTPUT(IRSendData);
  REG_OUTPUT(UltraSoundSendData);

  REG_OUTPUT(CameraSettingsRequest);
  REG_OUTPUT(SoundPlayData);
  REG_OUTPUT(TeamMessageDataOut);
  REG_OUTPUT(DebugMessageOut);
  
  platformInterface.registerCognitionOutputChanel<HeadMotionRequest, Serializer<HeadMotionRequest> >(getHeadMotionRequest());
  platformInterface.registerCognitionOutputChanel<MotionRequest, Serializer<MotionRequest> >(getMotionRequest());
}//end init

void Actuator::execute()
{  
  // HACK: copy the time to indicate which motion status this request ist depending on (needed by motion)
  getMotionRequest().time = getMotionStatus().time;

  GT_TRACE("Actuator:execute() end");
}//end execute
