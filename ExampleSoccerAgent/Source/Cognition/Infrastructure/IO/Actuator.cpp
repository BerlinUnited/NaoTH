/**
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 */

#include "Actuator.h"

Actuator::Actuator() :
  theMotionRequestWriter(NULL)
{
}

Actuator::~Actuator()
{
  if (theMotionRequestWriter != NULL)
      delete theMotionRequestWriter;
}


#define REG_OUTPUT(R) \
  platformInterface.registerCognitionOutput(get##R())

void Actuator::init(naoth::PlatformInterfaceBase& platformInterface)
{
  REG_OUTPUT(CameraSettingsRequest);
  REG_OUTPUT(LEDData);
  REG_OUTPUT(IRSendData);
  REG_OUTPUT(UltraSoundSendData);
  REG_OUTPUT(SoundPlayData);
  REG_OUTPUT(TeamMessageDataOut);
  REG_OUTPUT(DebugMessageOut);

  theMotionRequestWriter = new MessageWriter(platformInterface.getMessageQueue("MotionRequest"));

}//end init

void Actuator::execute()
{
  // send the motion request to motion
  stringstream mrmsg;
  Serializer<MotionRequest>::serialize(getMotionRequest(), mrmsg);
  theMotionRequestWriter->write(mrmsg.str());
}//end execute
