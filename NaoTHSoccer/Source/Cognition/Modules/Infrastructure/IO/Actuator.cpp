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

void Actuator::init(naoth::ProcessInterface& platformInterface, const naoth::PlatformBase& /*platform*/)
{
  REG_OUTPUT(LEDData);
  REG_OUTPUT(UltraSoundSendData);

  REG_OUTPUT(CameraSettingsRequest);
  REG_OUTPUT(CameraSettingsRequestTop);
  REG_OUTPUT(SoundPlayData);
  REG_OUTPUT(TeamMessageDataOut);
  REG_OUTPUT(TeamMessageDebug);
  REG_OUTPUT(DebugMessageOut);
  REG_OUTPUT(GameReturnData);

  REG_OUTPUT(AudioControl);

  platformInterface.registerOutputChanel(getCameraInfo());
  platformInterface.registerOutputChanel(getCameraInfoTop());
  platformInterface.registerOutputChanel(getCameraMatrixOffset());
  platformInterface.registerOutputChanel(getHeadMotionRequest());
  platformInterface.registerOutputChanel(getMotionRequest());
  platformInterface.registerOutputChanel(getBodyState());
}//end init

void Actuator::execute()
{  
  // HACK: copy the time to indicate which motion status this request ist depending on (needed by motion)
  getMotionRequest().time = getMotionStatus().time;
  getMotionRequest().cognitionFrameNumber = getFrameInfo().getFrameNumber();


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

  // play the startsound
  if(getFrameInfo().getFrameNumber() == 1) {
    getSoundPlayData().soundFile = "cognition_start.wav";
  }

  GT_TRACE("Actuator:execute() end");
}//end execute
