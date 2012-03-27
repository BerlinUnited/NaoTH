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


  // HACK: transform the head motion request to the support foot coordinates
  const Pose3D& lFoot = getKinematicChain().theLinks[KinematicChain::LFoot].M;
  const Pose3D& rFoot = getKinematicChain().theLinks[KinematicChain::RFoot].M;

  // left foot is the support foot
  if(lFoot.translation.z < rFoot.translation.z)
  {
    getHeadMotionRequest().targetPointInTheWorld = lFoot.invert()*getHeadMotionRequest().targetPointInTheWorld;
    getHeadMotionRequest().coordinate = HeadMotionRequest::LFoot;
  }
  else
  {
    getHeadMotionRequest().targetPointInTheWorld = rFoot.invert()*getHeadMotionRequest().targetPointInTheWorld;
    getHeadMotionRequest().coordinate = HeadMotionRequest::RFoot;
  }


  GT_TRACE("Actuator:execute() end");
}//end execute
