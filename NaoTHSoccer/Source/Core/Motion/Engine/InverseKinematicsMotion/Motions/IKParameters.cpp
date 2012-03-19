/**
* @file IKParameters.cpp
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* Implement of parameters for IK motion
*/

#include "IKParameters.h"
#include "Tools/Debug/DebugParameterList.h"


IKParameters::IKParameters()
:ParameterList("IKParameters")
{
  PARAMETER_REGISTER(bodyPitchOffset) = 0;
  PARAMETER_REGISTER(hipOffsetX) = 10;
  PARAMETER_REGISTER(footOffsetY) = 0;
  
  // stand parameter
  PARAMETER_REGISTER(stand.speed) = 0.04;
  PARAMETER_REGISTER(stand.enableStabilization) = false;
  
  // walk parameter
  PARAMETER_REGISTER(walk.stiffness) = 0.7;

  PARAMETER_REGISTER(walk.comHeight) = 260;
  PARAMETER_REGISTER(walk.ZMPOffsetY) = 5;
  PARAMETER_REGISTER(walk.ZMPOffsetYByCharacter) = 0;

  PARAMETER_REGISTER(walk.singleSupportTime) = 300;
  PARAMETER_REGISTER(walk.doubleSupportTime) = 40;
  PARAMETER_REGISTER(walk.maxExtendDoubleSupportTime) = 20;
  PARAMETER_REGISTER(walk.extendDoubleSupportTimeByCharacter) = 0;
  PARAMETER_REGISTER(walk.stepHeight) = 10;
//  PARAMETER_REGISTER(walk.curveFactor) = 7;

  PARAMETER_REGISTER(walk.maxTurnInner) = 10;
  PARAMETER_REGISTER(walk.maxStepTurn) = 30;
  PARAMETER_REGISTER(walk.maxStepLength) = 50;
  PARAMETER_REGISTER(walk.maxStepLengthBack) = 50;
  PARAMETER_REGISTER(walk.maxStepWidth) = 50;
  PARAMETER_REGISTER(walk.maxStepChange) = 0.5;

  PARAMETER_REGISTER(walk.enableFSRProtection) = true;
  PARAMETER_REGISTER(walk.enableWaitLanding) = false;
  PARAMETER_REGISTER(walk.minFSRProtectionCount) = 0;
  PARAMETER_REGISTER(walk.maxUnsupportedCount) = 0;
  PARAMETER_REGISTER(walk.maxWaitLandingCount) = 20;

  PARAMETER_REGISTER(walk.leftHipRollSingleSupFactor) = 0.4;
  PARAMETER_REGISTER(walk.rightHipRollSingleSupFactor) = 0.4;
  PARAMETER_REGISTER(walk.rotationStabilize) = true;

  PARAMETER_REGISTER(walk.stabilizeFeet) = true;
  PARAMETER_REGISTER(walk.stabilizeFeetP.x) = 0.0;
  PARAMETER_REGISTER(walk.stabilizeFeetP.y) = 0.0;
  PARAMETER_REGISTER(walk.stabilizeFeetD.x) = 0.0;
  PARAMETER_REGISTER(walk.stabilizeFeetD.y) = 0.0;

  PARAMETER_REGISTER(walk.dynamicStepsize) = true;

  PARAMETER_REGISTER(walk.useArm) = false;

  // rotation stabilize parameter
  PARAMETER_REGISTER(rotationStabilize.k.x) = -0.5;
  PARAMETER_REGISTER(rotationStabilize.k.y) = -0.2;
  PARAMETER_REGISTER(rotationStabilize.threshold.x) = 2;
  PARAMETER_REGISTER(rotationStabilize.threshold.y) = 3;

  // arm parameter
  PARAMETER_REGISTER(arm.shoulderPitchInterialSensorRate) = -10;
  PARAMETER_REGISTER(arm.shoulderRollInterialSensorRate) = -10;
  PARAMETER_REGISTER(arm.maxSpeed) = 200;
  PARAMETER_REGISTER(arm.alwaysEnabled)  = false;
  PARAMETER_REGISTER(arm.kickEnabled) = true;
  PARAMETER_REGISTER(arm.walkEnabled) = true;

  // kick parameter
  PARAMETER_REGISTER(kick.shiftSpeed) = 0.05;
  PARAMETER_REGISTER(kick.time_for_first_preparation) = 500;
  PARAMETER_REGISTER(kick.retractionSpeed) = 0.05;
  PARAMETER_REGISTER(kick.takeBackSpeed) = 0.05;
  PARAMETER_REGISTER(kick.timeToLand) = 500;
  PARAMETER_REGISTER(kick.stabilization_time) = 100;
  PARAMETER_REGISTER(kick.strengthOffset) = 32.5;

  PARAMETER_REGISTER(kick.minNumberOfPreKickSteps) = 15;
  PARAMETER_REGISTER(kick.shiftTime) = 1000;
  PARAMETER_REGISTER(kick.stopTime) = 1500;
  PARAMETER_REGISTER(kick.kickSpeed) = 1;
  PARAMETER_REGISTER(kick.afterKickTime) = 500;
  PARAMETER_REGISTER(kick.hipHeightOffset) = -10;
  PARAMETER_REGISTER(kick.footRadius) = 40;
  PARAMETER_REGISTER(kick.maxDistanceToRetract) = 200;
  PARAMETER_REGISTER(kick.enableStaticStabilizer) = true;

  PARAMETER_REGISTER(kick.basicXRotationOffset) = 3;
  PARAMETER_REGISTER(kick.extendedXRotationOffset) = 7;
  PARAMETER_REGISTER(kick.rotationTime) = 300;

  PARAMETER_REGISTER(kick.knee_pitch_offset);
  PARAMETER_REGISTER(kick.ankle_roll_offset);

  PARAMETER_REGISTER(kick.shiftOffsetYLeft);
  PARAMETER_REGISTER(kick.shiftOffsetYRight);

  syncWithConfig();

  DebugParameterList::getInstance().add(this);
}

IKParameters::~IKParameters()
{
  DebugParameterList::getInstance().remove(this);
}
