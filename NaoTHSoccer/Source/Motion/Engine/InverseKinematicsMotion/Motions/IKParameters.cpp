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
  PARAMETER_REGISTER(useWalk2018) = true;

  // stand parameter
  PARAMETER_REGISTER(stand.speed) = 0.04;
  PARAMETER_REGISTER(stand.enableStabilization) = true;
  PARAMETER_REGISTER(stand.enableStabilizationRC16) = false;
  PARAMETER_REGISTER(stand.stiffness) = 0.7;

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
  PARAMETER_REGISTER(stand.relax.stiffnessControl.minAngle)     = 0.08; // [°]
  PARAMETER_REGISTER(stand.relax.stiffnessControl.maxAngle)     = 2;    // [°]
  PARAMETER_REGISTER(stand.relax.stiffnessControl.minStiffness) = 0.3;
  PARAMETER_REGISTER(stand.relax.stiffnessControl.maxStiffness) = 1.0;

  // walk parameter:
  // General
  PARAMETER_ANGLE_REGISTER(walk.general.bodyPitchOffset) = 0.2;
  PARAMETER_REGISTER(walk.general.hipOffsetX) = 15;
  PARAMETER_REGISTER(walk.general.stiffness) = 0.7;
  PARAMETER_REGISTER(walk.general.stiffnessArms) = 0.7;
  PARAMETER_REGISTER(walk.general.useArm) = false;
  PARAMETER_REGISTER(walk.general.hipRollSingleSupFactorLeft) = 0.4;
  PARAMETER_REGISTER(walk.general.hipRollSingleSupFactorRight) = 0.4;

  // hip trajectory geometry
  PARAMETER_REGISTER(walk.hip.comHeight) = 260;
  PARAMETER_REGISTER(walk.hip.comHeightOffset) = 0.18;
  PARAMETER_REGISTER(walk.hip.comStepOffsetY) = 0;
  PARAMETER_REGISTER(walk.hip.ZMPOffsetY) = 5;
  PARAMETER_REGISTER(walk.hip.ZMPOffsetYByCharacter) = 0;

  // experimental: new ZMP
  PARAMETER_REGISTER(walk.hip.newZMP_ON) = false;

  PARAMETER_REGISTER(walk.zmp.bezier.transitionScaling) = 0.6;
  PARAMETER_REGISTER(walk.zmp.bezier.inFootScalingY) = 1;
  PARAMETER_REGISTER(walk.zmp.bezier.inFootSpacing)  = 10;
  PARAMETER_REGISTER(walk.zmp.bezier.offsetX) = 20;
  PARAMETER_REGISTER(walk.zmp.bezier.offsetY) = -5;
  PARAMETER_REGISTER(walk.zmp.bezier.offsetXForKicks) = 0;
  PARAMETER_REGISTER(walk.zmp.bezier.offsetYForKicks) = -20;

  PARAMETER_REGISTER(walk.zmp.bezier2.offsetY) = 0;
  PARAMETER_REGISTER(walk.zmp.bezier2.offsetT) = 0;

  // step geometry
  PARAMETER_REGISTER(walk.step.duration) = 300;
  PARAMETER_REGISTER(walk.step.dynamicDuration) = true;
  PARAMETER_REGISTER(walk.step.doubleSupportTime) = 40;
  PARAMETER_REGISTER(walk.step.stepHeight) = 15;
  PARAMETER_REGISTER(walk.step.splineFootTrajectory) = true;

  // kick
  PARAMETER_REGISTER(walk.kick.stepHeight) = 20;
  PARAMETER_REGISTER(walk.kick.ZMPOffsetY) = 0;

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
  //PARAMETER_REGISTER(walk.stabilization.enableFSRProtection) = true;
  //PARAMETER_REGISTER(walk.stabilization.maxWaitLandingCount) = 20;
  //PARAMETER_REGISTER(walk.stabilization.enableWaitLanding) = false;
  //PARAMETER_REGISTER(walk.stabilization.minFSRProtectionCount) = 0;
  //PARAMETER_REGISTER(walk.stabilization.maxUnsupportedCount) = 0;

  PARAMETER_REGISTER(walk.stabilization.emergencyStopError)  = 500;
  PARAMETER_REGISTER(walk.stabilization.maxEmergencyCounter) = 1000;

  PARAMETER_REGISTER(walk.stabilization.rotationStabilize) = false;
  PARAMETER_REGISTER(walk.stabilization.rotationStabilizeRC16) = false;
  PARAMETER_REGISTER(walk.stabilization.rotationStabilizeNewIMU) = true;

  PARAMETER_REGISTER(walk.stabilization.rotation.P.x) = -0.03;
  PARAMETER_REGISTER(walk.stabilization.rotation.P.y) = -0.03;
  PARAMETER_REGISTER(walk.stabilization.rotation.D.x) = 0;
  PARAMETER_REGISTER(walk.stabilization.rotation.D.y) = 0;
  PARAMETER_REGISTER(walk.stabilization.rotation.VelocityP.x) = 0.03;
  PARAMETER_REGISTER(walk.stabilization.rotation.VelocityP.y) = 0.03;

  PARAMETER_REGISTER(walk.stabilization.rotationRC16.P.x) = -0.05;
  PARAMETER_REGISTER(walk.stabilization.rotationRC16.P.y) = -0.05;
  PARAMETER_REGISTER(walk.stabilization.rotationRC16.D.x) = 0;
  PARAMETER_REGISTER(walk.stabilization.rotationRC16.D.y) = 0;
  PARAMETER_REGISTER(walk.stabilization.rotationRC16.VelocityP.x) = 0.02;
  PARAMETER_REGISTER(walk.stabilization.rotationRC16.VelocityP.y) = 0.01;

  PARAMETER_REGISTER(walk.stabilization.rotationNewIMU.P.x) = -0.2;
  PARAMETER_REGISTER(walk.stabilization.rotationNewIMU.P.y) = -0.2;
  PARAMETER_REGISTER(walk.stabilization.rotationNewIMU.D.x) = 0;
  PARAMETER_REGISTER(walk.stabilization.rotationNewIMU.D.y) = 0;
  PARAMETER_REGISTER(walk.stabilization.rotationNewIMU.VelocityP.x) = 0.03;
  PARAMETER_REGISTER(walk.stabilization.rotationNewIMU.VelocityP.y) = 0.03;

  PARAMETER_REGISTER(walk.stabilization.stabilizeFeet) = true;
  PARAMETER_REGISTER(walk.stabilization.stabilizeFeetP.x) = -0.1;
  PARAMETER_REGISTER(walk.stabilization.stabilizeFeetP.y) = -0.1;
  PARAMETER_REGISTER(walk.stabilization.stabilizeFeetD.x) = 0.01;
  PARAMETER_REGISTER(walk.stabilization.stabilizeFeetD.y) = 0.01;

  PARAMETER_REGISTER(walk.stabilization.dynamicStepsize) = true;
  PARAMETER_REGISTER(walk.stabilization.dynamicStepsizeP) = -1;
  PARAMETER_REGISTER(walk.stabilization.dynamicStepsizeD) = 0.5;

  PARAMETER_REGISTER(walk.stabilization.hipOffsetBasedOnStepChange.x) = 0.0;
  PARAMETER_REGISTER(walk.stabilization.hipOffsetBasedOnStepChange.y) = 0.0;

  PARAMETER_REGISTER(walk.stabilization.maxHipOffsetBasedOnStepLength.x) = 5;
  PARAMETER_REGISTER(walk.stabilization.maxHipOffsetBasedOnStepLength.y) = 0;
  PARAMETER_REGISTER(walk.stabilization.maxHipOffsetBasedOnStepLengthForKicks.x) = 0;
  PARAMETER_REGISTER(walk.stabilization.maxHipOffsetBasedOnStepLengthForKicks.y) = 0;

//  // rotation stabilize parameter
//  PARAMETER_REGISTER(rotationStabilize.k.x) = -0.5;
//  PARAMETER_REGISTER(rotationStabilize.k.y) = -0.2;
//  PARAMETER_REGISTER(rotationStabilize.threshold.x) = 2;
//  PARAMETER_REGISTER(rotationStabilize.threshold.y) = 3;

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
