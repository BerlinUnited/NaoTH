/**
* @file Walk2018.cpp
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich, Mellmann</a>
* @author <a href="mailto:kaden@informatik.hu-berlin.de">Steffen, Kaden</a>
*
*/

#include "Walk2018.h"

using namespace naoth;
using namespace InverseKinematic;

Walk2018::Walk2018() : IKMotion(getInverseKinematicsMotionEngineService(), motion::walk, getMotionLock()),
    parameters(getWalk2018Parameters().generalParams)
{
  DEBUG_REQUEST_REGISTER("Walk:draw_step_plan_geometry", "draw all planed steps, zmp and executed com", false);
  DEBUG_REQUEST_REGISTER("Walk:plot_genTrajectoryWithSplines", "plot spline interpolation to parametrize 3D foot trajectory", false);

  theCoMErrorProvider          = registerModule<CoMErrorProvider>("CoMErrorProvider", true);
  theFootStepPlanner           = registerModule<FootStepPlanner2018>("FootStepPlanner2018", true);
  theZMPPlanner                = registerModule<ZMPPlanner2018>("ZMPPlanner2018", true);
  theZMPPreviewController      = registerModule<ZMPPreviewController>("ZMPPreviewController", true);
  theFootTrajectoryGenerator   = registerModule<FootTrajectoryGenerator2018>("FootTrajectoryGenerator2018", true);
  theHipRotationOffsetModifier = registerModule<HipRotationOffsetModifier>("HipRotationOffsetModifier", true);
  theLiftingFootCompensator    = registerModule<LiftingFootCompensator>("LiftingFootCompensator", true);
  theTorsoRotationStabilizer   = registerModule<TorsoRotationStabilizer>("TorsoRotationStabilizer", true);
  theFeetStabilizer            = registerModule<FeetStabilizer>("FeetStabilizer", true);

  CoMFeetPose currentCOMFeetPose = getInverseKinematicsMotionEngineService().getEngine().getCurrentCoMFeetPose();
  currentCOMFeetPose.localInLeftFoot();
  Vector3d targetZMP(currentCOMFeetPose.com.translation);
  targetZMP.z = getWalk2018Parameters().zmpPlanner2018Params.comHeight;

  // reset buffers
  getCoMErrors().reset();
  getCommandBuffer().reset();
  getStepBuffer().reset();
  getZMPReferenceBuffer().reset();

  theHipRotationOffsetModifier->getModuleT()->reset();
  theFeetStabilizer->getModuleT()->reset();

  // init state of preview controller
  theZMPPreviewController->getModuleT()->init(currentCOMFeetPose.com.translation);
  size_t inital_number_of_cycles = static_cast<size_t>(theZMPPreviewController->getModuleT()->previewSteps());

  // init step buffer using the foot step planner
  theFootStepPlanner->getModuleT()->init(inital_number_of_cycles,currentCOMFeetPose.feet);

  // init zmp reference buffer using the zmp planner
  theZMPPlanner->getModuleT()->init(inital_number_of_cycles, currentCOMFeetPose.com.translation, targetZMP);

  //std::cout << "walk start" << std::endl;
}

void Walk2018::execute()
{
  // check the integrity
  getMotionRequest().walkRequest.assertValid();

  theCoMErrorProvider->execute();

  // planing phase
  // add new steps or delete executed ones if necessary
  theFootStepPlanner->execute();

  DEBUG_REQUEST("Walk:draw_step_plan_geometry",
    FIELD_DRAWING_CONTEXT;
    getStepBuffer().draw(getDebugDrawings());
  );

  // NOTE: check the integrity of the step buffer
  ASSERT(!getStepBuffer().empty());
  ASSERT(!getStepBuffer().last().isPlanned());
  if(!getStepBuffer().first().isExecuted()){
    theZMPPlanner->execute();
  }

  // running phase
  //ASSERT(!getStepBuffer().first().isExecuted());
  calculateTargetCoMFeetPose();

  // set arms
  // Attention: this will be overwritten by the arm motion engine if the ArmMotionRequest's MotionID is not equal to "none" or "arms_synchronised_with_walk"
  // NOTE: we set the arms before the calculation of the com, so  the motion of the com can be adjusted to the arms
  if(parameters.useArm) {
    getEngine().armsSynchronisedWithWalk(getRobotInfo(), getTargetCoMFeetPose().pose, getMotorJointData());
  }

  // calculate HipFeetPose which is getting close to the target CoMFeetPose
  bool solved = false;
  getTargetHipFeetPose().pose = getEngine().controlCenterOfMass(getMotorJointData(), getTargetCoMFeetPose().pose, solved, false);

  HipFeetPose cassert(getTargetHipFeetPose().pose);
  cassert.localInHip();
  ASSERT(std::abs(cassert.feet.left.translation.x)  < 150);
  ASSERT(std::abs(cassert.feet.left.translation.y)  < 200);
  ASSERT(std::abs(cassert.feet.left.translation.z)  < 350);
  ASSERT(std::abs(cassert.feet.right.translation.x) < 150);
  ASSERT(std::abs(cassert.feet.right.translation.y) < 200);
  ASSERT(std::abs(cassert.feet.right.translation.z) < 350);

  theTorsoRotationStabilizer->execute();

  calculateJoints();

  updateMotionStatus(getMotionStatus());

  // TODO: remove, only for debugging
  static FrameInfo start_stopping(getFrameInfo());
  if(getMotionRequest().id == getId()){
      start_stopping = getFrameInfo();
  }

  PLOT("Walk:stopping_for", getFrameInfo().getTimeSince(start_stopping));

  if(getMotionRequest().id != getId()
     && theZMPPreviewController->getModuleT()->is_stationary()
     && getStepBuffer().only_zero_steps()) {
    setCurrentState(motion::stopped);
    //std::cout << "walk stopped" << std::endl;
  } else {
    setCurrentState(motion::running);
  }
}//end execute

void Walk2018::calculateTargetCoMFeetPose()
{
  if(!getStepBuffer().first().isExecuted()){
    theZMPPreviewController->execute();
  }

  Vector3d com = getTargetCoMFeetPose().pose.com.translation;

  DEBUG_REQUEST("Walk:draw_step_plan_geometry",
    FIELD_DRAWING_CONTEXT;
    getDebugDrawings().pen(Color::BLUE, 1.0);
    getDebugDrawings().fillOval(com.x, com.y, 10, 10);
  );

  if(!getStepBuffer().first().isExecuted()){
    theFootTrajectoryGenerator->execute();
  }

  theHipRotationOffsetModifier->execute();

  if(!getStepBuffer().first().isExecuted()){
    theLiftingFootCompensator->execute();
  }

  // buffer target CoM feet pose
  getCommandBuffer().footIds.add(getStepBuffer().first().footStep.liftingFoot());
  getCommandBuffer().poses.add(getTargetCoMFeetPose().pose);

  getStepBuffer().first().executingCycle++;
}

void Walk2018::calculateJoints() {
    getEngine().solveHipFeetIK(getTargetHipFeetPose().pose);
    getEngine().copyLegJoints(getMotorJointData().position);

    // set stiffness for the arms
    for (size_t i = JointData::RShoulderRoll; i <= JointData::LElbowYaw; ++i) {
      getMotorJointData().stiffness[i] = parameters.stiffnessArms;
    }
    getMotorJointData().stiffness[JointData::LWristYaw] = parameters.stiffnessArms;
    getMotorJointData().stiffness[JointData::RWristYaw] = parameters.stiffnessArms;

    // set the legs stiffness for walking
    for (size_t i = JointData::RHipYawPitch; i <= JointData::LAnkleRoll; ++i) {
      getMotorJointData().stiffness[i] = parameters.stiffness;
    }

    // WIEDERLICHER HACK: force the hip joint
    if (getMotorJointData().position[JointData::LHipRoll] < 0) {
      getMotorJointData().position[JointData::LHipRoll] *= parameters.hipRollSingleSupFactorLeft; // if = 1 no damping or amplifing
    }
    if (getMotorJointData().position[JointData::RHipRoll] > 0) {
      getMotorJointData().position[JointData::RHipRoll] *= parameters.hipRollSingleSupFactorRight; // if = 1 no damping or amplifing
    }

    // STABILIZATION
    theFeetStabilizer->execute();
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
    // TODO: check
    FeetPose lastFeet = getStepBuffer().last().footStep.end();
    Pose3D lastCom = calculateStableCoMByFeet(lastFeet, getWalk2018Parameters().hipRotationOffsetModifierParams.bodyPitchOffset);

    Pose3D plannedHip = getTargetCoMFeetPose().pose.com.invert() * lastCom;
    Pose3D plannedlFoot = getTargetCoMFeetPose().pose.feet.left.invert() * lastFeet.left;
    Pose3D plannedrFoot = getTargetCoMFeetPose().pose.feet.right.invert() * lastFeet.right;

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
