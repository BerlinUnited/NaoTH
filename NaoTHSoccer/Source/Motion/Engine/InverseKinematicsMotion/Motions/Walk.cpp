/**
* @file Walk.cpp
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich, Mellmann</a>
* @author <a href="mailto:kaden@informatik.hu-berlin.de">Steffen, Kaden</a>
*
*/

#include "Walk.h"
#include "Walk/FootTrajectoryGenerator.h"
#include "Walk/ZMPPlanner.h"

using namespace naoth;
using namespace InverseKinematic;

Walk::Walk() : IKMotion(getInverseKinematicsMotionEngineService(), motion::walk, getMotionLock())
{
  DEBUG_REQUEST_REGISTER("Walk:draw_step_plan_geometry", "draw all planed steps, zmp and executed com", false);
  DEBUG_REQUEST_REGISTER("Walk:plot_genTrajectoryWithSplines", "plot spline interpolation to parametrize 3D foot trajectory", false);
}
  
void Walk::execute()
{
  // check the integrity 
  getMotionRequest().walkRequest.assertValid();

  // STABILIZATION
  calculateError();

  // planing phase
  // add new steps or delete executed ones if necessary
  manageSteps(getMotionRequest());

  // NOTE: check the integrity of the step buffer
  ASSERT(!stepBuffer.empty());
  ASSERT(!stepBuffer.last().isPlanned());
  ASSERT(!stepBuffer.first().isExecuted());

  // running phase
  planZMP();
  executeStep();


  DEBUG_REQUEST("Walk:draw_step_plan_geometry",
    FIELD_DRAWING_CONTEXT;
    stepBuffer.draw(getDebugDrawings());
  );

  
  {
    // STABILIZATION
    // HACK: compensate the foot lift movement
    CoMFeetPose tmp(theCoMFeetPose);
    if(stepBuffer.first().footStep.liftingFoot() == FootStep::LEFT) 
    {
      tmp.localInRightFoot();
      theCoMFeetPose.com.translation.z += parameters().hip.comHeightOffset * tmp.feet.left.translation.z;
      theCoMFeetPose.com.translation.y -= parameters().hip.comStepOffsetY * tmp.feet.left.translation.z;
      //theCoMFeetPose.com.rotateX( parameters().hip.comRotationOffsetX *tmp.feet.left.translation.z );

      PLOT("Walk:theCoMFeetPose:total_rotationY",tmp.com.rotation.getYAngle());
    } 
    else if(stepBuffer.first().footStep.liftingFoot() == FootStep::RIGHT) 
    {
      tmp.localInLeftFoot();
      theCoMFeetPose.com.translation.z += parameters().hip.comHeightOffset * tmp.feet.right.translation.z;
      theCoMFeetPose.com.translation.y += parameters().hip.comStepOffsetY * tmp.feet.left.translation.z;
      //theCoMFeetPose.com.rotateX( -parameters().hip.comRotationOffsetX*tmp.feet.right.translation.z );

      PLOT("Walk:theCoMFeetPose:total_rotationY",tmp.com.rotation.getYAngle());
    }

    // buffer the pose
    commandFootIdBuffer.add(stepBuffer.first().footStep.liftingFoot());
    commandPoseBuffer.add(theCoMFeetPose);
  }


  // apply inverse kinematic
  bool solved = false;
  HipFeetPose c = getEngine().controlCenterOfMass(getMotorJointData(), theCoMFeetPose, solved, false);


  // STABILIZATION
  // apply online stabilization
  if(getCalibrationData().calibrated && parameters().stabilization.rotationStabilize)
  {
    if(stepBuffer.first().footStep.liftingFoot() == FootStep::LEFT) {
      c.localInRightFoot();
    } else if(stepBuffer.first().footStep.liftingFoot() == FootStep::RIGHT) {
      c.localInLeftFoot();
    } else {
      c.localInHip();
    }
      
    getEngine().rotationStabilize(
      getInertialModel(),
      getGyrometerData(),
      getRobotInfo().getBasicTimeStepInSecond(),
      c);
  } 
  else if(getCalibrationData().calibrated && parameters().stabilization.rotationStabilizeRC16)
  {
    if(stepBuffer.first().footStep.liftingFoot() == FootStep::LEFT) {
      c.localInRightFoot();
    } else if(stepBuffer.first().footStep.liftingFoot() == FootStep::RIGHT) {
      c.localInLeftFoot();
    } else {
      c.localInHip();
    }
    
    getEngine().rotationStabilizeRC16(
      getInertialSensorData(),
      getGyrometerData(),
      getRobotInfo().getBasicTimeStepInSecond(),
      c);
  }
  else if(getCalibrationData().calibrated && parameters().stabilization.rotationStabilizeNewIMU)
  {
    if(stepBuffer.first().footStep.liftingFoot() == FootStep::LEFT) {
      c.localInRightFoot();
    } else if(stepBuffer.first().footStep.liftingFoot() == FootStep::RIGHT) {
      c.localInLeftFoot();
    } else {
      c.localInHip();
    }

    getEngine().rotationStabilizenNewIMU(
      getIMUData(),
      getGyrometerData(),
      getRobotInfo().getBasicTimeStepInSecond(),
      c);
  }

  getEngine().solveHipFeetIK(c);
  getEngine().copyLegJoints(getMotorJointData().position);

  // set arms
  // Attention: this will be overwritten by the arm motion engine if the ArmMotionRequest's MotionID is not equal to "none" or "arms_synchronised_with_walk"
  if(parameters().general.useArm) {
    getEngine().armsSynchronisedWithWalk(getRobotInfo(), c, getMotorJointData());
  }

  // set stiffness for the arms
  for (size_t i = JointData::RShoulderRoll; i <= JointData::LElbowYaw; ++i) {
    getMotorJointData().stiffness[i] = parameters().general.stiffnessArms;
  }
  getMotorJointData().stiffness[JointData::LWristYaw] = parameters().general.stiffnessArms;
  getMotorJointData().stiffness[JointData::RWristYaw] = parameters().general.stiffnessArms;

 
	// set the legs stiffness for walking
  for (size_t i = JointData::RHipYawPitch; i <= JointData::LAnkleRoll; ++i) {
    getMotorJointData().stiffness[i] = parameters().general.stiffness;
  }

  // WIEDERLICHER HACK: force the hip joint
  if (getMotorJointData().position[JointData::LHipRoll] < 0) {
    getMotorJointData().position[JointData::LHipRoll] *= parameters().general.hipRollSingleSupFactorLeft;
  }
  if (getMotorJointData().position[JointData::RHipRoll] > 0) {
    getMotorJointData().position[JointData::RHipRoll] *= parameters().general.hipRollSingleSupFactorRight;
  }

  // STABILIZATION
  if(parameters().stabilization.stabilizeFeet) {
    feetStabilize(stepBuffer.first(), getMotorJointData().position);
  }

  updateMotionStatus(getMotionStatus());

  if(getMotionRequest().id != getId() && getEngine().zmpControl.is_stationary()) {
    setCurrentState(motion::stopped);
    std::cout << "walk stopped" << std::endl;
  } else {
    setCurrentState(motion::running);
  }
}//end execute


void Walk::manageSteps(const MotionRequest& motionRequest)
{
  // add the initial step
  // NOTE: zmp for this step it already completely planed,
  //       so we add the actual first step right after
  if(stepBuffer.empty())
  {
    std::cout << "walk start" << std::endl;

    // use the current com pose as a basis for start
    CoMFeetPose currentCOMFeetPose = getEngine().getCurrentCoMFeetPose();
    currentCOMFeetPose.localInLeftFoot();

    Vector3d startZMPTarget(currentCOMFeetPose.com.translation);
    startZMPTarget.z = parameters().hip.comHeight;
    
    // new step (don't move the feet)
    Step& initialStep = stepBuffer.add();
    initialStep.footStep = FootStep(currentCOMFeetPose.feet, FootStep::NONE);

    // initialize the zmp buffer with the current com pose
    initialStep.numberOfCycles = getEngine().zmpControl.init(currentCOMFeetPose.com.translation, startZMPTarget);
    initialStep.planningCycle = initialStep.numberOfCycles;
  }

  // current step has been executed, remove
  if ( stepBuffer.first().isExecuted() ) {
    stepBuffer.pop();
  }

  // add a new step
  if(stepBuffer.last().isPlanned()) {
    const Step& lastStep = stepBuffer.last();
    Step& step = stepBuffer.add();
    calculateNewStep(lastStep, step, motionRequest.walkRequest);
  }

}//end manageSteps


void Walk::calculateNewStep(const Step& lastStep, Step& newStep, const WalkRequest& walkRequest) //const
{
  // update the parameters in case they have changed
  theFootStepPlanner.updateParameters(getEngine().getParameters());
  
  newStep.walkRequest = walkRequest;

  // STABILIZATION
  bool do_emergency_stop = com_errors.size() == com_errors.getMaxEntries() && com_errors.getAverage() > parameters().stabilization.emergencyStopError;

  if ( getMotionRequest().id != getId() || (do_emergency_stop && !walkRequest.stepControl.isProtected))
  {
    // try to make a last step to align the feet if it is required
    if ( getMotionRequest().standardStand ) {
      newStep.footStep = theFootStepPlanner.finalStep(lastStep.footStep, walkRequest);
    } else {
      newStep.footStep = theFootStepPlanner.zeroStep(lastStep.footStep);
    }

    newStep.numberOfCycles = (newStep.footStep.liftingFoot() == FootStep::NONE)?1:parameters().step.duration/getRobotInfo().basicTimeStep;
    newStep.type = STEP_WALK;

    // print it only once
    if(newStep.footStep.liftingFoot() == FootStep::NONE && lastStep.footStep.liftingFoot() != FootStep::NONE) {
      std::cout << "walk stopping ..." << std::endl;
    }
    return;
  }

  if (walkRequest.stepControl.stepRequestID == getMotionStatus().stepControl.stepRequestID + 1)
  {
    // return the accepted stepRequestID
    getMotionStatus().stepControl.stepRequestID += 1;

    switch (walkRequest.stepControl.type)
    {
    case WalkRequest::StepControlRequest::ZEROSTEP:
      newStep.footStep = theFootStepPlanner.zeroStep(lastStep.footStep);
      newStep.numberOfCycles = walkRequest.stepControl.time / getRobotInfo().basicTimeStep;
      newStep.type = STEP_CONTROL;
      break;
    case WalkRequest::StepControlRequest::KICKSTEP:
      newStep.footStep = theFootStepPlanner.controlStep(lastStep.footStep, walkRequest);
      newStep.numberOfCycles = walkRequest.stepControl.time / getRobotInfo().basicTimeStep;
      newStep.type = STEP_CONTROL;
      break;
    case WalkRequest::StepControlRequest::WALKSTEP:
      newStep.footStep = theFootStepPlanner.controlStep(lastStep.footStep, walkRequest);
      newStep.numberOfCycles = parameters().step.duration / getRobotInfo().basicTimeStep;
      newStep.type = STEP_CONTROL;

      PLOT("Walk:after_adaptStepSize_x", newStep.footStep.footEnd().translation.x);
      PLOT("Walk:after_adaptStepSize_y", newStep.footStep.footEnd().translation.y);
      break;
    default:
      ASSERT(false);
    }

    // STABILIZATION
    if (parameters().stabilization.dynamicStepsize && !walkRequest.stepControl.isProtected) {
      adaptStepSize(newStep.footStep);
      currentComErrorBuffer.clear();
    }

  }
  else // regular walk
  {
    newStep.footStep = theFootStepPlanner.nextStep(lastStep.footStep, walkRequest);
    newStep.numberOfCycles = parameters().step.duration / getRobotInfo().basicTimeStep;
    newStep.type = STEP_WALK;

    // STABILIZATION
    if (parameters().stabilization.dynamicStepsize) {
      adaptStepSize(newStep.footStep);
      currentComErrorBuffer.clear();
    }

    PLOT("Walk:XABSL_after_adaptStepSize_x", newStep.footStep.footEnd().translation.x);
    PLOT("Walk:XABSL_after_adaptStepSize_y", newStep.footStep.footEnd().translation.y);
  }
}


void Walk::planZMP()
{
  ASSERT(!stepBuffer.empty());

  Step& planningStep = stepBuffer.last();
  ASSERT(!planningStep.isPlanned());

  Vector3d zmp;
  if(planningStep.footStep.liftingFoot() == FootStep::NONE)
  {
    Pose3D finalBody = calculateStableCoMByFeet(planningStep.footStep.end(), getEngine().getParameters().walk.general.bodyPitchOffset);
    zmp = finalBody.translation;
  } else {
    double zmpOffsetYParameter;
    if (planningStep.type == STEP_CONTROL && planningStep.walkRequest.stepControl.type == WalkRequest::StepControlRequest::KICKSTEP)
    {
      zmpOffsetYParameter = parameters().kick.ZMPOffsetY;
    }
    else
    {
      zmpOffsetYParameter = parameters().hip.ZMPOffsetY;
    }
    // TODO: should it be a part of the Step?
    double zmpOffsetY = zmpOffsetYParameter + parameters().hip.ZMPOffsetYByCharacter * (1-planningStep.walkRequest.character);
    double zmpOffsetX = getEngine().getParameters().walk.general.hipOffsetX;

    Vector2d zmp_simple = ZMPPlanner::simplest(planningStep.footStep, zmpOffsetX, zmpOffsetY);
    zmp = Vector3d(zmp_simple.x, zmp_simple.y, parameters().hip.comHeight);
  }

  //zmp.z = parameters().hip.comHeight;
  getEngine().zmpControl.push(zmp);

  DEBUG_REQUEST("Walk:draw_step_plan_geometry",
    FIELD_DRAWING_CONTEXT;
    getDebugDrawings().pen(Color::BLUE, 5.0);
    getDebugDrawings().drawCircle(zmp.x, zmp.y, 10);
  );

  PLOT("Walk:DRAW_ZMP_x", zmp.x);
  PLOT("Walk:DRAW_ZMP_y", zmp.y);

  planningStep.planningCycle++;
}


void Walk::executeStep()
{
  ASSERT(!stepBuffer.empty());

  Step& executingStep = stepBuffer.first();
  ASSERT(!executingStep.isExecuted());

  Vector3d com;
  if( !getEngine().zmpControl.pop(com) ) {
    return;
  }


  DEBUG_REQUEST("Walk:draw_step_plan_geometry",
    FIELD_DRAWING_CONTEXT;
    getDebugDrawings().pen(Color::BLUE, 1.0);
    getDebugDrawings().fillOval(com.x, com.y, 10, 10);
  );

  switch(executingStep.footStep.liftingFoot()) 
  {
    case FootStep::LEFT:
    {
      theCoMFeetPose.feet.left = calculateLiftingFootPos(executingStep);
      theCoMFeetPose.feet.right = executingStep.footStep.supFoot();
      break;
    }
    case FootStep::RIGHT:
    {
      theCoMFeetPose.feet.left = executingStep.footStep.supFoot();
      theCoMFeetPose.feet.right = calculateLiftingFootPos(executingStep);
      break;
    }
    case FootStep::NONE:
    {
      theCoMFeetPose.feet = executingStep.footStep.begin();
      break;
    }
    default: ASSERT(false);
  }

  PLOT("Walk:trajectory:cos:x",theCoMFeetPose.feet.left.translation.x);
  PLOT("Walk:trajectory:cos:y",theCoMFeetPose.feet.left.translation.y);
  PLOT("Walk:trajectory:cos:z",theCoMFeetPose.feet.left.translation.z);

  DEBUG_REQUEST("Walk:plot_genTrajectoryWithSplines",
    if(executingStep.footStep.liftingFoot() == FootStep::LEFT) {
      Pose3D returnPose2 = FootTrajectorGenerator::genTrajectoryWithSplines(
                             executingStep.footStep.footBegin(),
                             executingStep.footStep.footEnd(),
                             executingStep.executingCycle,
                             executingStep.numberOfCycles,
                             parameters().step.stepHeight,
                             0, // footPitchOffset
                             0  // footRollOffset
                           );

      PLOT("Walk:trajectory:spline:x",returnPose2.translation.x);
      PLOT("Walk:trajectory:spline:y",returnPose2.translation.y);
      PLOT("Walk:trajectory:spline:z",returnPose2.translation.z);
    } else {
      PLOT("Walk:trajectory:spline:x",theCoMFeetPose.feet.left.translation.x);
      PLOT("Walk:trajectory:spline:y",theCoMFeetPose.feet.left.translation.y);
      PLOT("Walk:trajectory:spline:z",theCoMFeetPose.feet.left.translation.z);
    }
  );

  theCoMFeetPose.com.translation = com;
  // TODO: check this
  theCoMFeetPose.com.rotation = calculateBodyRotation(theCoMFeetPose.feet, getEngine().getParameters().walk.general.bodyPitchOffset);
  executingStep.executingCycle++;
}

Pose3D Walk::calculateLiftingFootPos(const Step& step) const
{
  int samplesDoubleSupport = std::max(0, (int) (parameters().step.doubleSupportTime / getRobotInfo().basicTimeStep));
  int samplesSingleSupport = step.numberOfCycles - samplesDoubleSupport;
  ASSERT(samplesSingleSupport >= 0 && samplesDoubleSupport >= 0);

  if ( step.type == STEP_CONTROL && step.walkRequest.stepControl.type == WalkRequest::StepControlRequest::KICKSTEP)
  {
    return FootTrajectorGenerator::stepControl(
      step.footStep.footBegin(),
      step.footStep.footEnd(),
      step.executingCycle,
      samplesDoubleSupport,
      samplesSingleSupport,
      parameters().kick.stepHeight,
      0, //footPitchOffset
      0, //footRollOffset
      step.walkRequest.stepControl.speedDirection,
      step.walkRequest.stepControl.scale);
  }
  else
  {
    if(parameters().step.splineFootTrajectory)
    {
      return FootTrajectorGenerator::genTrajectoryWithSplines(
              step.footStep.footBegin(),
              step.footStep.footEnd(),
              step.executingCycle,
              samplesSingleSupport,
              parameters().step.stepHeight,
              0, // footPitchOffset
              0  // footRollOffset
            );
    }
    else
    {
      return FootTrajectorGenerator::genTrajectory(
              step.footStep.footBegin(),
              step.footStep.footEnd(),
              step.executingCycle,
              samplesDoubleSupport,
              samplesSingleSupport,
              parameters().step.stepHeight,
              0, // footPitchOffset
              0  // footRollOffset
      );
    }
  }

  ASSERT(false);
  return Pose3D(); // is never reached
}

RotationMatrix Walk::calculateBodyRotation(const FeetPose& feet, double pitch) const
{
  double bodyAngleZ = Math::meanAngle(feet.left.rotation.getZAngle(), feet.right.rotation.getZAngle());
  return RotationMatrix::getRotationZ(bodyAngleZ) * RotationMatrix::getRotationY(pitch);
}

Pose3D Walk::calculateStableCoMByFeet(FeetPose feet, double pitch) const
{
  feet.left.translate(getEngine().getParameters().walk.general.hipOffsetX, 0, 0);
  feet.right.translate(getEngine().getParameters().walk.general.hipOffsetX, 0, 0);
  
  Pose3D com;
  com.rotation = calculateBodyRotation(feet, pitch);
  com.translation = (feet.left.translation + feet.right.translation) * 0.5;
  com.translation.z = parameters().hip.comHeight;

  return com;
}


void Walk::updateMotionStatus(MotionStatus& motionStatus) const
{
  if ( stepBuffer.empty() )
  {
    motionStatus.plannedMotion.lFoot = Pose2D();
    motionStatus.plannedMotion.rFoot = Pose2D();
    motionStatus.plannedMotion.hip = Pose2D();
  }
  else
  {
    FeetPose lastFeet = stepBuffer.last().footStep.end();
    Pose3D lastCom = calculateStableCoMByFeet(lastFeet, getEngine().getParameters().walk.general.bodyPitchOffset);

    Pose3D plannedHip = theCoMFeetPose.com.invert() * lastCom;
    Pose3D plannedlFoot = theCoMFeetPose.feet.left.invert() * lastFeet.left;
    Pose3D plannedrFoot = theCoMFeetPose.feet.right.invert() * lastFeet.right;

    motionStatus.plannedMotion.hip = plannedHip.projectXY();
    motionStatus.plannedMotion.lFoot =  plannedlFoot.projectXY();
    motionStatus.plannedMotion.rFoot = plannedrFoot.projectXY();
  }

  // step control
  if ( stepBuffer.empty() )
  {
    motionStatus.stepControl.stepID = 0;
    motionStatus.stepControl.moveableFoot = MotionStatus::StepControlStatus::BOTH;
  }
  else
  {
    motionStatus.stepControl.stepID = stepBuffer.last().id() + 1; 
    FootStep::Foot lastMovingFoot = stepBuffer.last().footStep.liftingFoot();
    switch(lastMovingFoot)
    {
    case FootStep::NONE:
      motionStatus.stepControl.moveableFoot = MotionStatus::StepControlStatus::BOTH;
      break;
    case FootStep::LEFT:
      motionStatus.stepControl.moveableFoot = MotionStatus::StepControlStatus::RIGHT;
      break;
    case FootStep::RIGHT:
      motionStatus.stepControl.moveableFoot = MotionStatus::StepControlStatus::LEFT;
      break;
    default: 
      ASSERT(false);
    }
  }
}//end updateMotionStatus


void Walk::adaptStepSize(FootStep& step) const
{
  // only do something when the buffer is not empty
  if(currentComErrorBuffer.size() > 0)
  {
    Vector3d errorCoM = currentComErrorBuffer.getAverage();
    static Vector3d lastCoMError = errorCoM;
    
    Vector3d comCorrection = errorCoM*parameters().stabilization.dynamicStepsizeP + 
                          (errorCoM - lastCoMError)*parameters().stabilization.dynamicStepsizeD;

    Vector3d stepCorrection = step.supFoot().rotation * comCorrection;
    step.footEnd().translation.x += stepCorrection.x;
    step.footEnd().translation.y += stepCorrection.y;

    lastCoMError = errorCoM;
  }
}//end adaptStepSize


void Walk::calculateError()
{
  int observerMeasurementDelay = 40;
  int index = int(observerMeasurementDelay / 10 - 0.5);
  ASSERT(index >= 0);

  // we need enough history
  if(commandPoseBuffer.size() == 0 || commandPoseBuffer.size() <= index ) {
    return;
  }

  InverseKinematic::CoMFeetPose expectedCoMFeetPose = commandPoseBuffer[index];

  Vector3d requested_com;
  Vector3d observed_com;

  // if right support
  if(commandFootIdBuffer[index] == FootStep::LEFT)
  {
    //const Pose3D& footRef_right = expectedCoMFeetPose.feet.right;
    //requested_com = footRef_right.local(expectedCoMFeetPose.com).translation;
    expectedCoMFeetPose.localInRightFoot();
    requested_com = expectedCoMFeetPose.com.translation;

    Pose3D footObs = getKinematicChainSensor().theLinks[KinematicChain::RFoot].M;
    footObs.translate(0, 0, -NaoInfo::FootHeight);
    footObs.rotation = RotationMatrix::getRotationY(footObs.rotation.getYAngle()); // assume the foot is flat on the ground
    observed_com = footObs.invert() * getKinematicChainSensor().CoM;
  }
  else
  {
    //const Pose3D& footRef_left = expectedCoMFeetPose.feet.left;
    //requested_com = footRef_left.local(expectedCoMFeetPose.com).translation;
    expectedCoMFeetPose.localInLeftFoot();
    requested_com = expectedCoMFeetPose.com.translation;

    Pose3D footObs = getKinematicChainSensor().theLinks[KinematicChain::LFoot].M;
    footObs.translate(0, 0, -NaoInfo::FootHeight);
    footObs.rotation = RotationMatrix::getRotationY(footObs.rotation.getYAngle()); // assume the foot is flat on the ground
    observed_com = footObs.invert() * getKinematicChainSensor().CoM;
  }
  
  currentComError = requested_com - observed_com;
  com_errors.add(currentComError.abs2());
  currentComErrorBuffer.add(currentComError);
}//end calculateError


void Walk::feetStabilize(const Step& executingStep, double (&position)[naoth::JointData::numOfJoint]) const
{
  // calculate the cycle
  // the same as in "FootTrajectorGenerator::genTrajectory"
  
  // TODO: remove dupication 
  double samplesDoubleSupport = std::max(0, (int) (parameters().step.doubleSupportTime / getRobotInfo().basicTimeStep));
  double samplesSingleSupport = executingStep.numberOfCycles - samplesDoubleSupport;
  ASSERT(samplesSingleSupport >= 0 && samplesDoubleSupport >= 0);

  double doubleSupportEnd = samplesDoubleSupport / 2;
  double doubleSupportBegin = samplesDoubleSupport / 2 + samplesSingleSupport;

  double cycle = executingStep.executingCycle;
  double z = 0;

  if (cycle > doubleSupportEnd && cycle <= doubleSupportBegin)
  {
    double t = 1 - (doubleSupportBegin - cycle) / samplesSingleSupport;
    t = t * Math::pi - Math::pi_2; // scale t
    z = (1 + cos(t * 2))*0.5;
  }

  const Vector2d& inertial = getInertialModel().orientation;
  const Vector3d& gyro = getGyrometerData().data;

  // HACK: small filter...
  static Vector3d lastGyro = gyro;
  Vector3d filteredGyro = (lastGyro+gyro)*0.5;

  Vector2d weight;
  weight.x = 
      parameters().stabilization.stabilizeFeetP.x * inertial.x
    + parameters().stabilization.stabilizeFeetD.x * filteredGyro.x;

  weight.y = 
      parameters().stabilization.stabilizeFeetP.y * inertial.y
    + parameters().stabilization.stabilizeFeetD.y * filteredGyro.y;


  switch(executingStep.footStep.liftingFoot())
  {
  case FootStep::LEFT: 
    // adjust left
    position[JointData::LAnkleRoll] -= inertial.x*z;
    position[JointData::LAnklePitch] -= inertial.y*z;

    // stabilize right
    position[JointData::RAnkleRoll] += weight.x*z;
    position[JointData::RAnklePitch] += weight.y*z;
    break;

  case FootStep::RIGHT:
    // adjust right
    position[JointData::RAnkleRoll] -= inertial.x*z;
    position[JointData::RAnklePitch] -= inertial.y*z;

    // stabilize left
    position[JointData::LAnkleRoll] += weight.x*z;
    position[JointData::LAnklePitch] += weight.y*z;
    break;
  default: break; // don't stabilize in double support mode
  };

  lastGyro = gyro;
}//end feetStabilize
