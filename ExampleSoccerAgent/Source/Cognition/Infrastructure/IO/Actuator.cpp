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
  platformInterface.registerOutput(get##R())

void Actuator::init(naoth::ProcessInterface& platformInterface, const naoth::PlatformBase& platform)
{
  REG_OUTPUT(CameraSettingsRequest);
  REG_OUTPUT(LEDData);
  REG_OUTPUT(IRSendData);
  REG_OUTPUT(UltraSoundSendData);
  REG_OUTPUT(SoundPlayData);
  REG_OUTPUT(TeamMessageDataOut);
  REG_OUTPUT(DebugMessageOut);

  platformInterface.registerOutputChanel(getMotionRequest());
}//end init

void Actuator::execute()
{

}//end execute
