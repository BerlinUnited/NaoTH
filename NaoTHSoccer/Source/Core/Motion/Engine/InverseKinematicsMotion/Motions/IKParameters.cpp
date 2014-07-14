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
  


  // walk parameter:
  // General
  PARAMETER_REGISTER(walk.general.stiffness) = 0.7;
  PARAMETER_REGISTER(walk.general.useArm) = false;
  PARAMETER_REGISTER(walk.general.hipRollSingleSupFactorLeft) = 0.4;
  PARAMETER_REGISTER(walk.general.hipRollSingleSupFactorRight) = 0.4;

  // hip trajectory geometry
  PARAMETER_REGISTER(walk.hip.comHeight) = 260;
  PARAMETER_REGISTER(walk.hip.ZMPOffsetY) = 5;
  PARAMETER_REGISTER(walk.hip.ZMPOffsetYByCharacter) = 0;

  // step geometry
  PARAMETER_REGISTER(walk.step.singleSupportTime) = 300;
  PARAMETER_REGISTER(walk.step.doubleSupportTime) = 40;
  PARAMETER_REGISTER(walk.step.maxExtendDoubleSupportTime) = 20;
  PARAMETER_REGISTER(walk.step.extendDoubleSupportTimeByCharacter) = 0;
  PARAMETER_REGISTER(walk.step.stepHeight) = 10;
//  PARAMETER_REGISTER(walk.step.curveFactor) = 7;

  // step limits
  PARAMETER_REGISTER(walk.limits.maxTurnInner) = 10;
  PARAMETER_REGISTER(walk.limits.maxStepTurn) = 30;
  PARAMETER_REGISTER(walk.limits.maxStepLength) = 50;
  PARAMETER_REGISTER(walk.limits.maxStepLengthBack) = 50;
  PARAMETER_REGISTER(walk.limits.maxStepWidth) = 50;
  PARAMETER_REGISTER(walk.limits.maxStepChange) = 0.5;

  // step control
  PARAMETER_REGISTER(walk.limits.maxCtrlTurn) = 30;
  PARAMETER_REGISTER(walk.limits.maxCtrlLength) = 80;
  PARAMETER_REGISTER(walk.limits.maxCtrlWidth) = 50;

  // Stabilization
  PARAMETER_REGISTER(walk.stabilization.enableFSRProtection) = true;
  PARAMETER_REGISTER(walk.stabilization.enableWaitLanding) = false;
  PARAMETER_REGISTER(walk.stabilization.minFSRProtectionCount) = 0;
  PARAMETER_REGISTER(walk.stabilization.maxUnsupportedCount) = 0;
  PARAMETER_REGISTER(walk.stabilization.maxWaitLandingCount) = 20;

  PARAMETER_REGISTER(walk.stabilization.rotationStabilize) = true;
  PARAMETER_REGISTER(walk.stabilization.rotationP.x) = 0;
  PARAMETER_REGISTER(walk.stabilization.rotationP.y) = 0;
  PARAMETER_REGISTER(walk.stabilization.rotationD.x) = 0;
  PARAMETER_REGISTER(walk.stabilization.rotationD.y) = 0;

  PARAMETER_REGISTER(walk.stabilization.stabilizeFeet) = true;
  PARAMETER_REGISTER(walk.stabilization.stabilizeFeetP.x) = 0.04;
  PARAMETER_REGISTER(walk.stabilization.stabilizeFeetP.y) = 0.035;
  PARAMETER_REGISTER(walk.stabilization.stabilizeFeetD.x) = -0.4;
  PARAMETER_REGISTER(walk.stabilization.stabilizeFeetD.y) = -0.3;

  PARAMETER_REGISTER(walk.stabilization.dynamicStepsize) = true;
  PARAMETER_REGISTER(walk.stabilization.dynamicStepsizeP) = -1;
  PARAMETER_REGISTER(walk.stabilization.dynamicStepsizeD) = 0.5;

  // rotation stabilize parameter
  PARAMETER_REGISTER(rotationStabilize.k.x) = -0.5;
  PARAMETER_REGISTER(rotationStabilize.k.y) = -0.2;
  PARAMETER_REGISTER(rotationStabilize.threshold.x) = 2;
  PARAMETER_REGISTER(rotationStabilize.threshold.y) = 3;

  // arm parameter
  PARAMETER_REGISTER(arm.shoulderPitchInterialSensorRate) = -10;
  PARAMETER_REGISTER(arm.shoulderRollInterialSensorRate) = -10;
  PARAMETER_REGISTER(arm.maxSpeed) = 60;
  PARAMETER_REGISTER(arm.alwaysEnabled)  = false;
  PARAMETER_REGISTER(arm.kickEnabled) = true;
  PARAMETER_REGISTER(arm.walkEnabled) = true;
  PARAMETER_REGISTER(arm.takeBack) = false;

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

  PARAMETER_REGISTER(kick.knee_pitch_offset) = 0;
  PARAMETER_REGISTER(kick.ankle_roll_offset) = 0;

  PARAMETER_REGISTER(kick.shiftOffsetYLeft) = 0;
  PARAMETER_REGISTER(kick.shiftOffsetYRight) = 0;

  PARAMETER_REGISTER(balanceCoM.kP) = 0;
  PARAMETER_REGISTER(balanceCoM.kI) = 0;
  PARAMETER_REGISTER(balanceCoM.kD) = 0;
  PARAMETER_REGISTER(balanceCoM.threshold) = 10;

  syncWithConfig();

  DebugParameterList::getInstance().add(this);
}

IKParameters::~IKParameters()
{
  DebugParameterList::getInstance().remove(this);
}
