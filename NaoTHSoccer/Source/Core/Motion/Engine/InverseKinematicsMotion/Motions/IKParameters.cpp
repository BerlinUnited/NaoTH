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
  PARAMETER_REGISTER(bodyPitchOffset) = 3;
  PARAMETER_REGISTER(hipOffsetX) = 10;
  PARAMETER_REGISTER(footOffsetY) = 0;
  
  PARAMETER_REGISTER(stand.speed) = 0.04;
  
  PARAMETER_REGISTER(walk.comHeight) = 260;
  PARAMETER_REGISTER(walk.bodyPitchOffset) = 0;
  PARAMETER_REGISTER(walk.singleSupportTime) = 300;
  PARAMETER_REGISTER(walk.doubleSupportTime) = 40;
  PARAMETER_REGISTER(walk.stepHeight) = 10;
  PARAMETER_REGISTER(walk.curveFactor) = 7;
  PARAMETER_REGISTER(walk.maxTurnInner) = 10;
  PARAMETER_REGISTER(walk.maxStepTurn) = 30;
  PARAMETER_REGISTER(walk.maxStepLength) = 50;
  PARAMETER_REGISTER(walk.maxStepLengthBack) = 50;
  PARAMETER_REGISTER(walk.maxStepWidth) = 50;
  PARAMETER_REGISTER(walk.maxStepChange) = 0.5;
  PARAMETER_REGISTER(walk.enableFSRProtection) = true;
  PARAMETER_REGISTER(walk.leftHipRollSingleSupFactor) = 0.4;
  PARAMETER_REGISTER(walk.rightHipRollSingleSupFactor) = 0.4;

  PARAMETER_REGISTER(rotationStabilize.k.x) = -0.5;
  PARAMETER_REGISTER(rotationStabilize.k.y) = -0.2;
  PARAMETER_REGISTER(rotationStabilize.threshold.x) = 2;
  PARAMETER_REGISTER(rotationStabilize.threshold.y) = 2;
  
  loadFromConfig();
}
