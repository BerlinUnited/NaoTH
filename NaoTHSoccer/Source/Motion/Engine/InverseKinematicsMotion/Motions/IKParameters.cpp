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
  PARAMETER_REGISTER(footOffsetY) = 0;

  // stand parameter
  PARAMETER_REGISTER(stand.speed) = 0.04;
  PARAMETER_REGISTER(stand.enableStabilization) = true;
  PARAMETER_REGISTER(stand.enableStabilizationRC16) = false;
  PARAMETER_REGISTER(stand.stiffnessGotoPose) = 1.0;
  PARAMETER_REGISTER(stand.stiffnessRelax) = 0.7;

  PARAMETER_ANGLE_REGISTER(stand.bodyPitchOffset) = 0.2;
  PARAMETER_REGISTER(stand.hipOffsetX) = 15;

  PARAMETER_REGISTER(stand.stabilization.rotation.P.x) = -0.03;
  PARAMETER_REGISTER(stand.stabilization.rotation.P.y) = -0.03;
  PARAMETER_REGISTER(stand.stabilization.rotation.D.x) = 0;
  PARAMETER_REGISTER(stand.stabilization.rotation.D.y) = 0;
  PARAMETER_REGISTER(stand.stabilization.rotation.VelocityP.x) = 0.03;
  PARAMETER_REGISTER(stand.stabilization.rotation.VelocityP.y) = 0.03;

  PARAMETER_REGISTER(stand.stabilization.rotationRC16.P.x) = -0.05;
  PARAMETER_REGISTER(stand.stabilization.rotationRC16.P.y) = -0.05;
  PARAMETER_REGISTER(stand.stabilization.rotationRC16.D.x) = 0;
  PARAMETER_REGISTER(stand.stabilization.rotationRC16.D.y) = 0;
  PARAMETER_REGISTER(stand.stabilization.rotationRC16.VelocityP.x) = 0.02;
  PARAMETER_REGISTER(stand.stabilization.rotationRC16.VelocityP.y) = 0.01;

  // relax
  PARAMETER_REGISTER(stand.relax.enable) = true;
  PARAMETER_REGISTER(stand.relax.allowedDeviation) = 5;   // [mm]
  PARAMETER_ANGLE_REGISTER(stand.relax.allowedRotationDeviation) = 5; // [rad]
  //PARAMETER_REGISTER(stand.relax.timeBonusForCorrection)   = 1000; // [ms]

  PARAMETER_REGISTER(stand.relax.jointOffsetTuning.enable) = true;
  PARAMETER_REGISTER(stand.relax.jointOffsetTuning.deadTime)         = 1000;         // [ms]
  PARAMETER_REGISTER(stand.relax.jointOffsetTuning.currentThreshold) = 0.3;          // [A]
  PARAMETER_REGISTER(stand.relax.jointOffsetTuning.minimalJointStep) = 0.0013962634; // [rad]

  PARAMETER_REGISTER(stand.relax.stiffnessControl.enable) = true;
  //PARAMETER_REGISTER(stand.relax.stiffnessControl.deadTime)     = 100;  // [ms]
  PARAMETER_REGISTER(stand.relax.stiffnessControl.minAngle)     = 0.08; // [degrees]
  PARAMETER_REGISTER(stand.relax.stiffnessControl.maxAngle)     = 2;    // [degrees]
  PARAMETER_REGISTER(stand.relax.stiffnessControl.minStiffness) = 0.3;
  PARAMETER_REGISTER(stand.relax.stiffnessControl.maxStiffness) = 1.0;

  // arm parameter
  PARAMETER_REGISTER(arm.inertialModelBasedMovement.shoulderPitchInterialSensorRate) = -10;
  PARAMETER_REGISTER(arm.inertialModelBasedMovement.shoulderRollInterialSensorRate)  = -10;
  PARAMETER_REGISTER(arm.synchronisedWithWalk.shoulderPitchRate) = 0.5;
  PARAMETER_REGISTER(arm.synchronisedWithWalk.shoulderRollRate)  = 0.5;
  PARAMETER_REGISTER(arm.synchronisedWithWalk.elbowRollRate)     = 0.5;
  PARAMETER_REGISTER(arm.maxSpeed) = 60;

  PARAMETER_REGISTER(balanceCoM.kP) = 0;
  PARAMETER_REGISTER(balanceCoM.kI) = 0;
  PARAMETER_REGISTER(balanceCoM.kD) = 0;
  PARAMETER_REGISTER(balanceCoM.threshold) = 10;

  syncWithConfig();
}

IKParameters::~IKParameters()
{
}
