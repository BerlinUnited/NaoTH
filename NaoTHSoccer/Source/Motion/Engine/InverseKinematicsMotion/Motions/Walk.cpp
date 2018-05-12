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


#include "Tools/DataStructures/Spline.h"

using namespace naoth;
using namespace InverseKinematic;

Walk::Walk() : IKMotion(getInverseKinematicsMotionEngineService(), motion::walk, getMotionLock())
{
  DEBUG_REQUEST_REGISTER("Walk:draw_step_plan_geometry", "draw all planed steps, zmp and executed com", false);
  DEBUG_REQUEST_REGISTER("Walk:plot_genTrajectoryWithSplines", "plot spline interpolation to parametrize 3D foot trajectory", false);

  DEBUG_REQUEST_REGISTER("Walk:useBezierBased2", "use method 2 for bezier interpolation", false);

  emergencyCounter = 0;
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
      parameters().stabilization.rotation.P,
      parameters().stabilization.rotation.VelocityP,
      parameters().stabilization.rotation.D,
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
      getInertialSensorData().data,
      getGyrometerData(),
      getRobotInfo().getBasicTimeStepInSecond(),
      parameters().stabilization.rotationRC16.P,
      parameters().stabilization.rotationRC16.VelocityP,
      parameters().stabilization.rotationRC16.D,
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

    getEngine().rotationStabilizeRC16(
      getIMUData().orientation,
      getGyrometerData(),
      getRobotInfo().getBasicTimeStepInSecond(),
      parameters().stabilization.rotationNewIMU.P,
      parameters().stabilization.rotationNewIMU.VelocityP,
      parameters().stabilization.rotationNewIMU.D,
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
  bool do_emergency_stop = com_errors.isFull() && com_errors.getAverage() > parameters().stabilization.emergencyStopError;

  if ( getMotionRequest().id != getId() || (do_emergency_stop && !walkRequest.stepControl.isProtected))
  {
    // TODO: find reason for deadlock
    // current fix: force leaving emergency_stop after some cycles
    if(do_emergency_stop) {
      emergencyCounter++;
    }

    PLOT("Walk:emergencyCounter",emergencyCounter);

    if(emergencyCounter > parameters().stabilization.maxEmergencyCounter){
        emergencyCounter = 0;
        com_errors.clear();
    }

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
  } else {
      // reset emergencyCounter if the stop was succesful (no deadlock case)
      emergencyCounter = 0;
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
    {
      newStep.footStep = theFootStepPlanner.controlStep(lastStep.footStep, walkRequest);

      int duration = parameters().step.duration;

      if(parameters().step.dynamicDuration)
      {
        if(walkRequest.character <= 0.3) {
          duration = 300;
        } else if(walkRequest.character <= 0.7) {
          duration = 280;
        } else {// if(walkRequest.character == 1) {
          duration = 260;
        }
      }

      //newStep.numberOfCycles = parameters().step.duration / getRobotInfo().basicTimeStep;
      newStep.numberOfCycles = duration / getRobotInfo().basicTimeStep;
      newStep.type = STEP_CONTROL;

      PLOT("Walk:after_adaptStepSize_x", newStep.footStep.footEnd().translation.x);
      PLOT("Walk:after_adaptStepSize_y", newStep.footStep.footEnd().translation.y);
      break;
    }
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
  Vector3d other_zmp;
  if(planningStep.footStep.liftingFoot() == FootStep::NONE)
  {
    Pose3D finalBody = calculateStableCoMByFeet(planningStep.footStep.end(), getEngine().getParameters().walk.general.bodyPitchOffset);
    zmp = finalBody.translation;
  } else {
      // TODO: need to be done only once per step
      Pose3D startFoot, targetFoot;
      if(planningStep.footStep.liftingFoot() == FootStep::LEFT){
          InverseKinematic::FeetPose begin = planningStep.footStep.begin();
          begin.localInRightFoot();
          InverseKinematic::FeetPose end   = planningStep.footStep.end();
          end.localInRightFoot();

          startFoot  = begin.left;
          targetFoot = end.left;
      } else {
          InverseKinematic::FeetPose begin = planningStep.footStep.begin();
          begin.localInLeftFoot();
          InverseKinematic::FeetPose end   = planningStep.footStep.end();
          end.localInLeftFoot();

          startFoot  = begin.right;
          targetFoot = end.right;
      }

      Vector2d currentStepLength = targetFoot.projectXY().translation - startFoot.projectXY().translation;

      PLOT("Walk:hipOffsetBasedOnStepLength.x", parameters().stabilization.maxHipOffsetBasedOnStepLength.x * std::abs(currentStepLength.x)/parameters().limits.maxStepLength);
      // TODO end

      double zmpOffsetY, newZMPOffsetY, zmpOffsetX, newZMPOffsetX;

      // TODO: should it be a part of the Step?
      // TODO: hipOffsetBasedOnStepLength.y?
      if (planningStep.type == STEP_CONTROL && planningStep.walkRequest.stepControl.type == WalkRequest::StepControlRequest::KICKSTEP)
      {
          zmpOffsetX = parameters().general.hipOffsetX
                     + parameters().stabilization.maxHipOffsetBasedOnStepLengthForKicks.x * ((currentStepLength.x > 0) ? currentStepLength.x / parameters().limits.maxCtrlLength : 0);
          zmpOffsetY = parameters().kick.ZMPOffsetY    + parameters().hip.ZMPOffsetYByCharacter * (1-planningStep.walkRequest.character);

          newZMPOffsetX = parameters().zmp.bezier.offsetXForKicks
                        + parameters().stabilization.maxHipOffsetBasedOnStepLengthForKicks.x * ((currentStepLength.x > 0) ? currentStepLength.x / parameters().limits.maxCtrlLength : 0);
          newZMPOffsetY = parameters().zmp.bezier.offsetYForKicks + parameters().hip.ZMPOffsetYByCharacter * (1-planningStep.walkRequest.character);
      }
      else
      {
          zmpOffsetX = parameters().general.hipOffsetX + parameters().stabilization.maxHipOffsetBasedOnStepLength.x * ((currentStepLength.x > 0) ? currentStepLength.x / parameters().limits.maxStepLength : 0);
          zmpOffsetY = parameters().hip.ZMPOffsetY     + parameters().hip.ZMPOffsetYByCharacter * (1-planningStep.walkRequest.character);

          newZMPOffsetX = parameters().zmp.bezier.offsetX + parameters().stabilization.maxHipOffsetBasedOnStepLength.x * ((currentStepLength.x > 0) ? currentStepLength.x / parameters().limits.maxStepLength : 0);
          newZMPOffsetY = parameters().zmp.bezier.offsetY + parameters().hip.ZMPOffsetYByCharacter * (1-planningStep.walkRequest.character);
      }

      int samplesDoubleSupport = std::max(0, (int) (parameters().step.doubleSupportTime / getRobotInfo().basicTimeStep));
      int samplesSingleSupport = planningStep.numberOfCycles - samplesDoubleSupport;
      ASSERT(samplesSingleSupport >= 0 && samplesDoubleSupport >= 0);

      Vector2d zmp_new;
      zmp_new = ZMPPlanner::bezierBased(
                  planningStep.footStep,
                  planningStep.planningCycle,
                  samplesDoubleSupport,
                  samplesSingleSupport,
                  newZMPOffsetX,
                  newZMPOffsetY,
                  parameters().zmp.bezier.inFootScalingY,
                  parameters().zmp.bezier.inFootSpacing,
                  parameters().zmp.bezier.transitionScaling);

      // old zmp
      Vector2d zmp_simple = ZMPPlanner::simplest(planningStep.footStep, zmpOffsetX, zmpOffsetY);

      if(parameters().hip.newZMP_ON)
      {
          zmp = Vector3d(zmp_new.x, zmp_new.y, parameters().hip.comHeight);
          other_zmp = Vector3d(zmp_simple.x, zmp_simple.y, parameters().hip.comHeight);
      } else {
          zmp = Vector3d(zmp_simple.x, zmp_simple.y, parameters().hip.comHeight);
          other_zmp = Vector3d(zmp_new.x, zmp_new.y, parameters().hip.comHeight);
      }
  }

  Vector2d zmp_in_local = planningStep.footStep.supFoot().projectXY()/Vector2d(zmp.x,zmp.y);
  PLOT("Walk:zmp:x", zmp_in_local.x);
  PLOT("Walk:zmp:y", zmp_in_local.y);

  Vector2d other_zmp_in_local = planningStep.footStep.supFoot().projectXY()/Vector2d(other_zmp.x,other_zmp.y);
  PLOT("Walk:other_zmp:x", other_zmp_in_local.x);
  PLOT("Walk:other_zmp:y", other_zmp_in_local.y);
  //PLOT_GENERIC("Walk:zmp:xy", zmp.x, zmp.y);

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

  if(stepBuffer.first().isExecuted()){ // new step in next cycle
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

  // HACK: small filter:
  filteredGyro = filteredGyro*0.8 + gyro*0.2;

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

}//end feetStabilize
