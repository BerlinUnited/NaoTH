/**
* @file IKParameters.cpp
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* Implement of parameters for IK motion
*/

#include "IKParameters.h"


IKParameters::IKParameters()
:ParameterList("IKParameters")
{
  PARAMETER_REGISTER(bodyPitchOffset) = 0;
  PARAMETER_REGISTER(hipOffsetX) = 10;
  PARAMETER_REGISTER(footOffsetY) = 0;
  
  // stand parameter
  PARAMETER_REGISTER(stand.speed) = 0.04;
  
  // walk parameter
  PARAMETER_REGISTER(walk.stiffness) = 0.7;
  PARAMETER_REGISTER(walk.comHeight) = 260;
  PARAMETER_REGISTER(walk.ZMPOffsetY) = 5;
  PARAMETER_REGISTER(walk.singleSupportTime) = 300;
  PARAMETER_REGISTER(walk.doubleSupportTime) = 40;
  PARAMETER_REGISTER(walk.maxExtendDoubleSupportTime) = 20;
  PARAMETER_REGISTER(walk.stepHeight) = 10;
  PARAMETER_REGISTER(walk.curveFactor) = 7;
  PARAMETER_REGISTER(walk.maxTurnInner) = 10;
  PARAMETER_REGISTER(walk.maxStepTurn) = 30;
  PARAMETER_REGISTER(walk.maxStepLength) = 50;
  PARAMETER_REGISTER(walk.maxStepLengthBack) = 50;
  PARAMETER_REGISTER(walk.maxStepWidth) = 50;
  PARAMETER_REGISTER(walk.maxStepChange) = 0.5;
  PARAMETER_REGISTER(walk.enableFSRProtection) = true;
  PARAMETER_REGISTER(walk.maxUnsupportedCount) = 0;
  PARAMETER_REGISTER(walk.maxWaitLandingCount) = 20;
  PARAMETER_REGISTER(walk.leftHipRollSingleSupFactor) = 0.4;
  PARAMETER_REGISTER(walk.rightHipRollSingleSupFactor) = 0.4;

  // rotation stabilize parameter
  PARAMETER_REGISTER(rotationStabilize.k.x) = -0.5;
  PARAMETER_REGISTER(rotationStabilize.k.y) = -0.2;
  PARAMETER_REGISTER(rotationStabilize.threshold.x) = 2;
  PARAMETER_REGISTER(rotationStabilize.threshold.y) = 3;

  // arm parameter
  PARAMETER_REGISTER(arm.maxSpeed) = 200;

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
  PARAMETER_REGISTER(kick.hipHeight) = 210;
  PARAMETER_REGISTER(kick.footRadius) = 40;
  PARAMETER_REGISTER(kick.maxDistanceToRetract) = 200;
  PARAMETER_REGISTER(kick.enableStaticStabilizer) = true;


  loadFromConfig();
}
