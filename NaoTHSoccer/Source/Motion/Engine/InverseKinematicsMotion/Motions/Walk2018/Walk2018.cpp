/**
* @file Walk.cpp
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich, Mellmann</a>
* @author <a href="mailto:kaden@informatik.hu-berlin.de">Steffen, Kaden</a>
*
*/

#include "Walk2018.h"
#include "../Walk/FootTrajectoryGenerator.h"
#include "../Walk/ZMPPlanner.h"


#include "Tools/DataStructures/Spline.h"

using namespace naoth;
using namespace InverseKinematic;

Walk2018::Walk2018() : IKMotion(getInverseKinematicsMotionEngineService(), motion::walk, getMotionLock())
{
  DEBUG_REQUEST_REGISTER("Walk:draw_step_plan_geometry", "draw all planed steps, zmp and executed com", false);
  DEBUG_REQUEST_REGISTER("Walk:plot_genTrajectoryWithSplines", "plot spline interpolation to parametrize 3D foot trajectory", false);

  theCoMErrorProvider = registerModule<CoMErrorProvider>("CoMErrorProvider", true);
  theFootStepPlanner  = registerModule<FootStepPlanner2018>("FootStepPlanner2018", true);
  theZMPPlanner       = registerModule<ZMPPlanner2018>("ZMPPlanner2018", true);
}
  
void Walk2018::execute()
{
  // check the integrity 
  getMotionRequest().walkRequest.assertValid();

  theCoMErrorProvider->execute();

  // planing phase
  // add new steps or delete executed ones if necessary
  theFootStepPlanner->execute();

  // NOTE: check the integrity of the step buffer
  ASSERT(!getStepBuffer().empty());
  ASSERT(!getStepBuffer().last().isPlanned());
  ASSERT(!getStepBuffer().first().isExecuted());

  // running phase
  theZMPPlanner->execute();
  executeStep();

  DEBUG_REQUEST("Walk:draw_step_plan_geometry",
    FIELD_DRAWING_CONTEXT;
    getStepBuffer().draw(getDebugDrawings());
  );

  {
    // STABILIZATION
    // HACK: compensate the foot lift movement
    CoMFeetPose tmp(theCoMFeetPose);
    if(getStepBuffer().first().footStep.liftingFoot() == FootStep::LEFT)
    {
      tmp.localInRightFoot();
      theCoMFeetPose.com.translation.z += parameters().hip.comHeightOffset * tmp.feet.left.translation.z;
      theCoMFeetPose.com.translation.y -= parameters().hip.comStepOffsetY * tmp.feet.left.translation.z;
      //theCoMFeetPose.com.rotateX( parameters().hip.comRotationOffsetX *tmp.feet.left.translation.z );

      PLOT("Walk:theCoMFeetPose:total_rotationY",tmp.com.rotation.getYAngle());
    } 
    else if(getStepBuffer().first().footStep.liftingFoot() == FootStep::RIGHT)
    {
      tmp.localInLeftFoot();
      theCoMFeetPose.com.translation.z += parameters().hip.comHeightOffset * tmp.feet.right.translation.z;
      theCoMFeetPose.com.translation.y += parameters().hip.comStepOffsetY * tmp.feet.left.translation.z;
      //theCoMFeetPose.com.rotateX( -parameters().hip.comRotationOffsetX*tmp.feet.right.translation.z );

      PLOT("Walk:theCoMFeetPose:total_rotationY",tmp.com.rotation.getYAngle());
    }

    // buffer the pose
    getCommandBuffer().footIds.add(getStepBuffer().first().footStep.liftingFoot());
    getCommandBuffer().poses.add(theCoMFeetPose);
  }

  // set arms
  // Attention: this will be overwritten by the arm motion engine if the ArmMotionRequest's MotionID is not equal to "none" or "arms_synchronised_with_walk"
  // NOTE: we set the arms before the calculation of the com, so  the motion of the com can be adjusted to the arms
  if(parameters().general.useArm) {
    getEngine().armsSynchronisedWithWalk(getRobotInfo(), theCoMFeetPose, getMotorJointData());
  }

  // apply inverse kinematic
  bool solved = false;
  HipFeetPose c = getEngine().controlCenterOfMass(getMotorJointData(), theCoMFeetPose, solved, false);

  HipFeetPose cassert(c);
  cassert.localInHip();
  ASSERT(std::abs(cassert.feet.left.translation.x) < 150);
  ASSERT(std::abs(cassert.feet.left.translation.y) < 200);
  ASSERT(std::abs(cassert.feet.left.translation.z) < 350);
  ASSERT(std::abs(cassert.feet.right.translation.x) < 150);
  ASSERT(std::abs(cassert.feet.right.translation.y) < 200);
  ASSERT(std::abs(cassert.feet.right.translation.z) < 350);

  // STABILIZATION
  // apply online stabilization
  if(getCalibrationData().calibrated && parameters().stabilization.rotationStabilize)
  {
    if(getStepBuffer().first().footStep.liftingFoot() == FootStep::LEFT) {
      c.localInRightFoot();
    } else if(getStepBuffer().first().footStep.liftingFoot() == FootStep::RIGHT) {
      c.localInLeftFoot();
    } else {
      c.localInHip();
    }

    getEngine().rotationStabilize(
      getInertialModel(),
      getGyrometerData(),
      getRobotInfo().getBasicTimeStepInSecond(),
      parameters().stabilization.rotation.P,
      parameters().stabilization.rotation.VelocityP,
      parameters().stabilization.rotation.D,
      c);
  }

  getEngine().solveHipFeetIK(c);
  getEngine().copyLegJoints(getMotorJointData().position);

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
    getMotorJointData().position[JointData::LHipRoll] *= parameters().general.hipRollSingleSupFactorLeft; // if = 1 no damping or amplifing
  }
  if (getMotorJointData().position[JointData::RHipRoll] > 0) {
    getMotorJointData().position[JointData::RHipRoll] *= parameters().general.hipRollSingleSupFactorRight; // if = 1 no damping or amplifing
  }

  // STABILIZATION
  if(parameters().stabilization.stabilizeFeet) {
    feetStabilize(getStepBuffer().first(), getMotorJointData().position);
  }

  updateMotionStatus(getMotionStatus());

  if(getMotionRequest().id != getId() && getEngine().zmpControl.is_stationary()) {
    setCurrentState(motion::stopped);
    std::cout << "walk stopped" << std::endl;
  } else {
    setCurrentState(motion::running);
  }
}//end execute

void Walk2018::executeStep()
{
  ASSERT(!getStepBuffer().empty());

  Step& executingStep = getStepBuffer().first();
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
      PLOT("Walk:liftingFootRightTrajec", theCoMFeetPose.feet.right.translation.y);
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

  // apply rotation offset depending on step change
  static Vector2d lastStepLength;
  static bool updateOffset = false;

  static Vector2d hipRotationOffsetBasedOnStepChange;
  //apply stepChange on hip
  if(updateOffset){
      updateOffset = false;

      Pose3D startFoot, targetFoot;
      if(executingStep.footStep.liftingFoot() == FootStep::LEFT){
              InverseKinematic::FeetPose begin = executingStep.footStep.begin();
              begin.localInRightFoot();
              InverseKinematic::FeetPose end   = executingStep.footStep.end();
              end.localInRightFoot();

              startFoot  = begin.left;
              targetFoot = end.left;

          } else {
              InverseKinematic::FeetPose begin = executingStep.footStep.begin();
              begin.localInLeftFoot();
              InverseKinematic::FeetPose end   = executingStep.footStep.end();
              end.localInLeftFoot();

              startFoot  = begin.right;
              targetFoot = end.right;
          }

      Vector2d currentStepLength = targetFoot.projectXY().translation - startFoot.projectXY().translation;

      Vector2d currentStepChange = currentStepLength - lastStepLength;

      hipRotationOffsetBasedOnStepChange = Vector2d(parameters().stabilization.hipOffsetBasedOnStepChange.x * currentStepChange.x,
                                                    parameters().stabilization.hipOffsetBasedOnStepChange.y * currentStepChange.y);

      PLOT("Walk:currentStepChange.x", currentStepChange.x);
      PLOT("Walk:currentStepChange.y", currentStepChange.y);
  }

  // TODO: check this
  // TODO: apply hipRotationOffset smoothly
  PLOT("Walk::hipRotationOffsetBasedOnStepChange.x",hipRotationOffsetBasedOnStepChange.x);

  std::vector<double> xA = {0.0, 0.5, 1.0};
  std::vector<double> yA = {0.0, 1.0, 0.0};

  tk::spline scaleOffset;
  scaleOffset.set_boundary(tk::spline::first_deriv,0.0, tk::spline::first_deriv,0.0, false);
  scaleOffset.set_points(xA,yA);

  double t = executingStep.executingCycle/executingStep.numberOfCycles;
  theCoMFeetPose.com.rotation = calculateBodyRotation(theCoMFeetPose.feet, scaleOffset(t)*Math::fromDegrees(hipRotationOffsetBasedOnStepChange.x) + getEngine().getParameters().walk.general.bodyPitchOffset);
  executingStep.executingCycle++;

  if(getStepBuffer().first().isExecuted()){ // new step in next cycle
      // TODO: only need to be done once per step
      Pose3D startFoot, targetFoot;
      if(executingStep.footStep.liftingFoot() == FootStep::LEFT){
              InverseKinematic::FeetPose begin = executingStep.footStep.begin();
              begin.localInRightFoot();
              InverseKinematic::FeetPose end   = executingStep.footStep.end();
              end.localInRightFoot();

              startFoot  = begin.left;
              targetFoot = end.left;

          } else {
              InverseKinematic::FeetPose begin = executingStep.footStep.begin();
              begin.localInLeftFoot();
              InverseKinematic::FeetPose end   = executingStep.footStep.end();
              end.localInLeftFoot();

              startFoot  = begin.right;
              targetFoot = end.right;
          }

      lastStepLength = targetFoot.projectXY().translation - startFoot.projectXY().translation;
      // TODO end
      updateOffset = true;
  }
}

Pose3D Walk2018::calculateLiftingFootPos(const Step& step) const
{
  int samplesDoubleSupport = std::max(0, (int) (parameters().step.doubleSupportTime / getRobotInfo().basicTimeStep));
  int samplesSingleSupport = step.numberOfCycles - samplesDoubleSupport;
  ASSERT(samplesSingleSupport >= 0 && samplesDoubleSupport >= 0);

  if ( step.type == Step::STEP_CONTROL && step.walkRequest.stepControl.type == WalkRequest::StepControlRequest::KICKSTEP)
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

void Walk2018::updateMotionStatus(MotionStatus& motionStatus) const
{
  if ( getStepBuffer().empty() )
  {
    motionStatus.plannedMotion.lFoot = Pose2D();
    motionStatus.plannedMotion.rFoot = Pose2D();
    motionStatus.plannedMotion.hip = Pose2D();
  }
  else
  {
    FeetPose lastFeet = getStepBuffer().last().footStep.end();
    Pose3D lastCom = calculateStableCoMByFeet(lastFeet, getEngine().getParameters().walk.general.bodyPitchOffset);

    Pose3D plannedHip = theCoMFeetPose.com.invert() * lastCom;
    Pose3D plannedlFoot = theCoMFeetPose.feet.left.invert() * lastFeet.left;
    Pose3D plannedrFoot = theCoMFeetPose.feet.right.invert() * lastFeet.right;

    motionStatus.plannedMotion.hip = plannedHip.projectXY();
    motionStatus.plannedMotion.lFoot =  plannedlFoot.projectXY();
    motionStatus.plannedMotion.rFoot = plannedrFoot.projectXY();
  }

  // step control
  if ( getStepBuffer().empty() )
  {
    motionStatus.stepControl.stepID = 0;
    motionStatus.stepControl.moveableFoot = MotionStatus::StepControlStatus::BOTH;
  }
  else
  {
    motionStatus.stepControl.stepID = getStepBuffer().last().id() + 1;
    FootStep::Foot lastMovingFoot = getStepBuffer().last().footStep.liftingFoot();
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

void Walk2018::feetStabilize(const Step& executingStep, double (&position)[naoth::JointData::numOfJoint]) const
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
  Vector3d filteredGyro = filteredGyro*0.8 + gyro*0.2;

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
