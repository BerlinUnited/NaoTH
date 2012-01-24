/**
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 */

#include "Actuator.h"

Actuator::Actuator():
theHeadMotionRequestWriter(NULL),
theMotionRequestWriter(NULL)
{
}

Actuator::~Actuator()
{
  if (theHeadMotionRequestWriter != NULL)
    delete theHeadMotionRequestWriter;
  if (theMotionRequestWriter != NULL)
    delete theMotionRequestWriter;
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

  //theHeadMotionRequestWriter = new MessageWriter(platformInterface.getMessageQueue("HeadMotionRequest"));
  //theMotionRequestWriter = new MessageWriter(platformInterface.getMessageQueue("MotionRequest"));
}//end init

void Actuator::execute()
{  
  // HACK: copy the time to indicate which motion status this request ist depending on (needed by motion)
  getMotionRequest().time = getMotionStatus().time;

  /*
  // data to motion
  stringstream hmmsg;
  Serializer<HeadMotionRequest>::serialize(getHeadMotionRequest(), hmmsg);
  GT_TRACE("Actuator:execute():writing theHeadMotionRequest");
  theHeadMotionRequestWriter->write(hmmsg.str());
  

  stringstream mrmsg;
  Serializer<MotionRequest>::serialize(getMotionRequest(), mrmsg);
  GT_TRACE("Actuator:execute():writing theMotionRequest");
  theMotionRequestWriter->write(mrmsg.str());
  */
  GT_TRACE("Actuator:execute() end");
}//end execute
