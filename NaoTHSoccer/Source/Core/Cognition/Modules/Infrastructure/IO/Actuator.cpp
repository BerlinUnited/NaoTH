/**
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 */

#include "Actuator.h"

Actuator::Actuator():
theHeadMotionRequestMsgQueue(NULL),
theMotionRequestMsgQueue(NULL)
{
}

Actuator::~Actuator()
{
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
  
  theHeadMotionRequestMsgQueue = platformInterface.getMessageQueue("HeadMotionRequest");
  theMotionRequestMsgQueue = platformInterface.getMessageQueue("MotionRequest");
}//end init

void Actuator::execute()
{  
  // data to motion
  stringstream hmmsg;
  Serializer<HeadMotionRequest>::serialize(getHeadMotionRequest(), hmmsg);
  theHeadMotionRequestMsgQueue->write(hmmsg.str());
  
  stringstream mrmsg;
  Serializer<MotionRequest>::serialize(getMotionRequest(), mrmsg);
  theMotionRequestMsgQueue->write(mrmsg.str());
}//end execute
