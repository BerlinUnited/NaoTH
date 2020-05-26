/**
* @file FootStepPlanner.cpp
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* plan the foot step according to motion request
*/

#include "FootStepPlanner2018.h"

using namespace InverseKinematic;
using namespace std;

FootStepPlanner2018::FootStepPlanner2018() :
  parameters(getWalk2018Parameters().footStepPlanner2018Params),
  theFootOffsetY(0.0),
  emergencyCounter(0)
{}

void FootStepPlanner2018::updateParameters() {
  theFootOffsetY   = NaoInfo::HipOffsetY + parameters.footOffsetY;
}

void FootStepPlanner2018::init(size_t initial_number_of_cycles, FeetPose initialFeetPose)
{
    Step& initialStep = getStepBuffer().add();
    initialStep.footStep = FootStep(initialFeetPose, FootStep::NONE);
    initialStep.numberOfCycles = static_cast<int>(initial_number_of_cycles) - 1; // TODO: why?
    initialStep.planningCycle  = initialStep.numberOfCycles;
    initialStep.samplesDoubleSupport = std::max(0, (int) (parameters.step.doubleSupportTime / getRobotInfo().basicTimeStep));
    initialStep.samplesSingleSupport = initialStep.numberOfCycles - initialStep.samplesDoubleSupport;
    ASSERT(initialStep.samplesSingleSupport >= 0 && initialStep.samplesDoubleSupport >= 0);
}

void FootStepPlanner2018::execute()
{
    static int delayed_frames = 0;

    bool ready_to_switch_support;
    if(getStepBuffer().first().footStep.liftingFoot() == FootStep::LEFT){ // weight shifting from right to left
        ready_to_switch_support = getCentreOfPressure().in_kinematic_chain_origin.valid
                                  && getCentreOfPressure().in_kinematic_chain_origin.CoP.y > parameters.stabilization.switching_offset;
    } else if(getStepBuffer().first().footStep.liftingFoot() == FootStep::RIGHT){ // weight shifting from left to right
        ready_to_switch_support = getCentreOfPressure().in_kinematic_chain_origin.valid
                                  && getCentreOfPressure().in_kinematic_chain_origin.CoP.y < -1 * parameters.stabilization.switching_offset;
    } else { // both feet on the ground
        ready_to_switch_support = true;
    }

    PLOT("FootStepPlanner2018:ready_to_switch", ready_to_switch_support);
    PLOT("FootStepPlanner2018:centre_of_pressure_y", getCentreOfPressure().in_kinematic_chain_origin.CoP.y);

    // current step has been executed, remove
    if (getStepBuffer().first().isExecuted() && (!parameters.stabilization.use_step_feedback || ready_to_switch_support || delayed_frames > parameters.stabilization.max_frames)) {
      getStepBuffer().remove();
      delayed_frames = 0;
    } else if(parameters.stabilization.use_step_feedback && getStepBuffer().first().isExecuted()){
      delayed_frames++;
    }

    PLOT("FootStepPlanner2018:exceedExecutionCycle", getStepBuffer().first().isExecuted());
    PLOT("FootStepPlanner2018:delayed_frames", delayed_frames);

    // add a new step
    if(getStepBuffer().last().isPlanned()) {
      const Step& lastStep = getStepBuffer().last();
      Step& step = getStepBuffer().add();
      calculateNewStep(lastStep, step, getMotionRequest().walkRequest);
    }
}

void FootStepPlanner2018::calculateNewStep(const Step& lastStep, Step& newStep, const WalkRequest& walkRequest) //const
{
  // update the parameters in case they have changed
  updateParameters();

  newStep.walkRequest = walkRequest;

  // STABILIZATION
  bool do_emergency_stop = getCoMErrors().absolute2.isFull() && getCoMErrors().absolute2.getAverage() > parameters.stabilization.emergencyStopError;
  // NOTE: set the walk_emergency_stop below only in the case if it's actually executed
  getMotionStatus().walk_emergency_stop = false;

  // TODO: maybe move the check back to walk2018 and call a special function for finishing the motion
  if ( getMotionRequest().id != motion::walk /*getId()*/ || (do_emergency_stop && !walkRequest.stepControl.isProtected))
  {
    // TODO: find reason for deadlock
    // current fix: force leaving emergency_stop after some cycles
    if(do_emergency_stop) {
      emergencyCounter++;

      getMotionStatus().walk_emergency_stop = do_emergency_stop;
    }

    PLOT("Walk:emergencyCounter",emergencyCounter);

    if(emergencyCounter > parameters.stabilization.maxEmergencyCounter) {
        emergencyCounter = 0;
        getCoMErrors().absolute2.clear();
    }

    // try to make a last step to align the feet if it is required
    if ( getMotionRequest().standardStand ) {
      newStep.footStep = finalStep(lastStep.footStep, walkRequest);
    } else {
      newStep.footStep = zeroStep(lastStep.footStep);
    }

    if(newStep.footStep.liftingFoot() == FootStep::NONE) {
      newStep.numberOfCycles = 1;
      newStep.samplesDoubleSupport = 1;
      newStep.samplesSingleSupport = 0;
    } else {
      newStep.numberOfCycles = parameters.step.duration/getRobotInfo().basicTimeStep;
      newStep.samplesDoubleSupport = std::max(0, (int) (parameters.step.doubleSupportTime / getRobotInfo().basicTimeStep));
      newStep.samplesSingleSupport = newStep.numberOfCycles - newStep.samplesDoubleSupport;
    }
    ASSERT(newStep.samplesSingleSupport >= 0 && newStep.samplesDoubleSupport >= 0);
    newStep.type = Step::STEP_WALK;

    // print it only once
    if(newStep.footStep.liftingFoot() == FootStep::NONE && lastStep.footStep.liftingFoot() != FootStep::NONE) {
      //std::cout << "walk stopping ..." << std::endl;
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
      newStep.footStep = zeroStep(lastStep.footStep);
      newStep.numberOfCycles = walkRequest.stepControl.time / getRobotInfo().basicTimeStep;
      newStep.type = Step::STEP_CONTROL;
      break;
    case WalkRequest::StepControlRequest::KICKSTEP:
      newStep.footStep = controlStep(lastStep.footStep, walkRequest);
      newStep.numberOfCycles = walkRequest.stepControl.time / getRobotInfo().basicTimeStep;
      newStep.type = Step::STEP_CONTROL;
      break;
    case WalkRequest::StepControlRequest::WALKSTEP:
    {
      newStep.footStep = controlStep(lastStep.footStep, walkRequest);

      int duration = parameters.step.duration;

      if(parameters.step.dynamicDuration.on) {
        if(walkRequest.character <= 0.3) {
          duration = parameters.step.dynamicDuration.stable; //300;
        } else if(walkRequest.character <= 0.7) {
          duration = parameters.step.dynamicDuration.normal; //280;
        } else {// if(walkRequest.character == 1) {
          duration = parameters.step.dynamicDuration.fast; //260;
        }
      }

      //newStep.numberOfCycles = parameters().step.duration / getRobotInfo().basicTimeStep;
      newStep.numberOfCycles = duration / getRobotInfo().basicTimeStep;
      newStep.type = Step::STEP_CONTROL;

      PLOT("Walk:after_adaptStepSize_x", newStep.footStep.footEnd().translation.x);
      PLOT("Walk:after_adaptStepSize_y", newStep.footStep.footEnd().translation.y);
      break;
    }
    default:
      ASSERT(false);
    }
  }
  else // regular walk
  {
    newStep.footStep = nextStep(lastStep.footStep, walkRequest);
    int duration = parameters.step.duration;

    if(parameters.step.dynamicDuration.on) {
      if(walkRequest.character <= 0.3) {
        duration = parameters.step.dynamicDuration.stable; //300;
      } else if(walkRequest.character <= 0.7) {
        duration = parameters.step.dynamicDuration.normal; //280;
      } else {// if(walkRequest.character == 1) {
        duration = parameters.step.dynamicDuration.fast; //260;
      }
    }

    newStep.numberOfCycles = duration / getRobotInfo().basicTimeStep;
    newStep.type = Step::STEP_WALK;

    PLOT("Walk:XABSL_after_adaptStepSize_x", newStep.footStep.footEnd().translation.x);
    PLOT("Walk:XABSL_after_adaptStepSize_y", newStep.footStep.footEnd().translation.y);
  }

  // TODO: is the following assert always hold by special steps if doubleSupportTime != 0 ?
  newStep.samplesDoubleSupport = std::max(0, (int) (parameters.step.doubleSupportTime / getRobotInfo().basicTimeStep));
  newStep.samplesSingleSupport = newStep.numberOfCycles - newStep.samplesDoubleSupport;
  ASSERT(newStep.samplesSingleSupport >= 0 && newStep.samplesDoubleSupport >= 0);

  // STABILIZATION
  if (parameters.stabilization.dynamicStepSize && !walkRequest.stepControl.isProtected) {
    adaptStepSize(newStep.footStep);
    getCoMErrors().e.clear();
  }
}

void FootStepPlanner2018::adaptStepSize(FootStep& step) const
{
  // only do something when the buffer is not empty
  if(getCoMErrors().e.size() > 0)
  {
    Vector3d errorCoM = getCoMErrors().e.getAverage();
    static Vector3d lastCoMError = errorCoM;

    Vector3d comCorrection = errorCoM * parameters.stabilization.dynamicStepSizeP +
                          (errorCoM - lastCoMError) * parameters.stabilization.dynamicStepSizeD;

    Vector3d stepCorrection = step.supFoot().rotation * comCorrection;
    step.footEnd().translation.x += stepCorrection.x;
    step.footEnd().translation.y += stepCorrection.y;

    lastCoMError = errorCoM;
  }
}//end adaptStepSize

FootStep FootStepPlanner2018::finalStep(const FootStep& lastStep, const WalkRequest& /*req*/) const
{
  // don't move the feet if they stoped moving once
  if(lastStep.liftingFoot() == FootStep::NONE) {
    return zeroStep(lastStep);
  }

  // TODO: check if an actual step is necessary based on the last step
  //       => calculate an actual step only if necessary

  // try to plan a real last step with an empty walk request
  FootStep footStep = nextStep(lastStep, WalkRequest());
  // how much did the foot move in this step
  Pose3D diff = footStep.footBegin().invert() * footStep.footEnd();

  // planed step almost didn't move the foot, i.e., is was almost a zero step
  if(diff.translation.abs2() < 1 && diff.rotation.getZAngle() < Math::fromDegrees(1)) {
    return zeroStep(lastStep);
  } else {
    return footStep;
  }
}

FootStep FootStepPlanner2018::controlStep(const FootStep& lastStep, const WalkRequest& req) const
{
  WalkRequest myReq = req;
  myReq.target = req.stepControl.target;//HACK

  FootStep::Foot liftingFoot = req.stepControl.moveLeftFoot?FootStep::LEFT:FootStep::RIGHT;
  return calculateNextWalkStep(lastStep.end(), lastStep.offset(), lastStep.stepRequest(), liftingFoot, myReq, true);
}

FootStep FootStepPlanner2018::zeroStep(const FootStep& lastStep) const {
  return FootStep(lastStep.end(), FootStep::NONE);
}

FootStep FootStepPlanner2018::nextStep(const FootStep& lastStep, const WalkRequest& req) const
{
  if ( lastStep.liftingFoot() == FootStep::NONE ) {
    return firstStep(lastStep.end(), lastStep.offset(), lastStep.stepRequest(), req);
  } else {
    FootStep::Foot liftingFoot = (lastStep.liftingFoot()==FootStep::LEFT)?FootStep::RIGHT:FootStep::LEFT;
    return calculateNextWalkStep(lastStep.end(), lastStep.offset(), lastStep.stepRequest(), liftingFoot, req, false);
  }
}

FootStep FootStepPlanner2018::firstStep(const InverseKinematic::FeetPose& pose, const Pose2D& offset, const Pose2D& lastStepRequest, const WalkRequest& req) const
{
  FootStep firstStepLeft  = calculateNextWalkStep(pose, offset, lastStepRequest, FootStep::LEFT, req);
  FootStep firstStepRight = calculateNextWalkStep(pose, offset, lastStepRequest, FootStep::RIGHT, req);

  Pose3D leftMove  = firstStepLeft.footBegin().invert() * firstStepLeft.footEnd();
  Pose3D rightMove = firstStepRight.footBegin().invert() * firstStepRight.footEnd();

  // TODO: think about criteria, it should be better to always take the step which aligns the rotation at most
  //       proposal: remove else part
  if ( fabs(req.target.rotation) > parameters.limits.maxTurnInner )
  {
    // choose foot by rotation
    double leftTurn = leftMove.rotation.getZAngle();
    double rightTurn = rightMove.rotation.getZAngle();
    if ( fabs(leftTurn) > fabs(rightTurn) ) {
      return firstStepLeft;
    } else {
      return firstStepRight;
    }
  }
  else
  {
    // choose foot by distance
    if ( leftMove.translation.abs2() > rightMove.translation.abs2() ) {
      return firstStepLeft;
    } else {
      return firstStepRight;
    }
  }
}//end firstStep

// TODO: parameter for the foot to move
FootStep FootStepPlanner2018::calculateNextWalkStep(const InverseKinematic::FeetPose& pose, const Pose2D& offset, const Pose2D& lastStepRequest, FootStep::Foot movingFoot, const WalkRequest& req, bool stepControl) const
{
  // TODO: how to deal with zero steps properly
  ASSERT(movingFoot != FootStep::NONE);

  // transform between the foot coordinates and the corresponding origin
  const Pose2D supportOriginOffset = offset * Pose2D(0, theFootOffsetY);
  const Pose2D targetOriginOffset = req.offset * Pose2D(0, theFootOffsetY);

  // transform between the global odometry coordinates and the origin of the support foot
  const Pose2D supportOrigin = (movingFoot == FootStep::RIGHT)?
    pose.left.projectXY() * supportOriginOffset.invert() :
    pose.right.projectXY() * supportOriginOffset;

  // transform the request in the coordinates of the support origin
  Pose2D stepRequest = req.target;
  if (req.coordinate == WalkRequest::LFoot) {
    stepRequest = supportOrigin.invert() * pose.left.projectXY() * stepRequest * targetOriginOffset.invert();
  } else if(req.coordinate == WalkRequest::RFoot) {
    stepRequest = supportOrigin.invert() * pose.right.projectXY() * stepRequest * targetOriginOffset;
  }

  // apply restriction mode
  if (stepControl && req.stepControl.restriction == WalkRequest::StepControlRequest::RestrictionMode::SOFT)
  {
    restrictStepSize(stepRequest, req.character, true);
  }
  // the stepControl with restriction mode HARD behaves the same way as regular walk request
  else if (stepControl && req.stepControl.restriction == WalkRequest::StepControlRequest::RestrictionMode::HARD)
  {
    restrictStepSize(stepRequest, req.character, false);
    restrictStepChange(stepRequest, lastStepRequest, true);
  }
  else // regular walk request
  {
    restrictStepSize(stepRequest, req.character, false);
    restrictStepChange(stepRequest, lastStepRequest, false);
  }

  // create a new step
  FootStep newStep(pose, movingFoot);
  newStep.offset() = req.offset;

  // HACK: save the step request before geometrical restrictions
  // TODO: why?
  newStep.stepRequest() = stepRequest;

  // apply geometric restrictions to the step request
  if(movingFoot == FootStep::RIGHT) {
    stepRequest = Pose2D(min(parameters.limits.maxTurnInner, stepRequest.rotation), stepRequest.translation.x, min(0.0, stepRequest.translation.y));
  } else {
    stepRequest = Pose2D(max(-parameters.limits.maxTurnInner, stepRequest.rotation), stepRequest.translation.x, max(0.0, stepRequest.translation.y));
  }

  // apply the planed motion and calculate the coordinates of the moving foot
  Pose2D footStepTarget = supportOrigin * stepRequest * ((movingFoot == FootStep::RIGHT) ? targetOriginOffset.invert() : targetOriginOffset);

  newStep.footEnd() = Pose3D::embedXY(footStepTarget);

  return newStep;
}//end calculateNextWalkStep

//TODO: do we really need different parameters for normal and step control steps?
void FootStepPlanner2018::restrictStepSize(Pose2D& step, double character, bool stepControl) const
{
  // scale the character: [0, 1] --> [0.5, 1]
  character = 0.5*character + 0.5;

  double maxTurn, maxStepLength, maxStepLengthBack, maxStepWidth;

  if(stepControl) {
      maxTurn       = parameters.limits.maxCtrlTurn   * character;
      maxStepLength = parameters.limits.maxCtrlLength * character;
      maxStepWidth  = parameters.limits.maxCtrlWidth  * character;
  } else {
      maxTurn       = parameters.limits.maxStepTurn   * character;
      maxStepLength = parameters.limits.maxStepLength * character;
      maxStepWidth  = parameters.limits.maxStepWidth  * character;
  }

  maxStepLengthBack = parameters.limits.maxStepLengthBack * character;

  if ( step.translation.x < 0 ) {
      maxStepLength = maxStepLengthBack;
  }

  // limit translation
  if ( maxStepLength > 0.5 && maxStepWidth > 0.5 ) {
    // restrict the step size in an ellipse
    double alpha = step.translation.angle();
    double cosa  = cos(alpha);
    double sina  = sin(alpha);

    const double maxStepLength2 = Math::sqr(maxStepLength);
    const double maxStepWidth2  = Math::sqr(maxStepWidth);
    double length = sqrt((maxStepLength2 * maxStepWidth2) / (maxStepLength2 * Math::sqr(sina) + maxStepWidth2 * Math::sqr(cosa)));

    if (step.translation.abs2() > Math::sqr(length))
    {
      step.translation.x = length * cosa;
      step.translation.y = length * sina;
    }
  } else {
    step.translation.x = Math::clamp(step.translation.x, -maxStepLengthBack, maxStepLength);
    step.translation.y = Math::clamp(step.translation.y, -maxStepWidth, maxStepWidth);
  }

  ASSERT( step.translation.x >= -maxStepLengthBack);
  ASSERT( step.translation.x <=  maxStepLength);
  ASSERT( fabs(step.translation.y) <= maxStepWidth);

  // limit rotation
  step.rotation = Math::clamp(step.rotation, -maxTurn, maxTurn);  // limit the rotation

  // limit translation depending on rotation if not doing a stepControl
  if (!stepControl && maxTurn > Math::fromDegrees(1.0) ) {
    step.translation *= cos( step.rotation/maxTurn * Math::pi / 2);
  }
}//end restrictStepSize

void FootStepPlanner2018::restrictStepChange(Pose2D& step, const Pose2D& lastStep, bool stepControl) const
{
    double maxChangeDownX,  maxChangeDownY, maxChangeDownTurn, maxChangeX, maxChangeY,  maxChangeTurn;

    if(stepControl){
        maxChangeDownX    = parameters.limits.maxCtrlLength * parameters.limits.maxCtrlChangeDown;
        maxChangeDownY    = parameters.limits.maxCtrlWidth  * parameters.limits.maxCtrlChangeDown;
        maxChangeDownTurn = parameters.limits.maxCtrlTurn   * parameters.limits.maxCtrlChangeDown;
        maxChangeX        = parameters.limits.maxCtrlLength * parameters.limits.maxCtrlChange;
        maxChangeY        = parameters.limits.maxCtrlWidth  * parameters.limits.maxCtrlChange;
        maxChangeTurn     = parameters.limits.maxCtrlTurn   * parameters.limits.maxCtrlChange;
    } else {
        maxChangeDownX    = parameters.limits.maxStepLength * parameters.limits.maxStepChangeDown;
        maxChangeDownY    = parameters.limits.maxStepWidth  * parameters.limits.maxStepChangeDown;
        maxChangeDownTurn = parameters.limits.maxStepTurn   * parameters.limits.maxStepChangeDown;
        maxChangeX        = parameters.limits.maxStepLength * parameters.limits.maxStepChange;
        maxChangeY        = parameters.limits.maxStepWidth  * parameters.limits.maxStepChange;
        maxChangeTurn     = parameters.limits.maxStepTurn   * parameters.limits.maxStepChange;
    }

    Pose2D change;
    change.translation = step.translation - lastStep.translation;
    change.rotation = Math::normalize(step.rotation - lastStep.rotation);

    change.translation.x = Math::clamp(change.translation.x, -maxChangeDownX, maxChangeX);
    change.translation.y = Math::clamp(change.translation.y, -maxChangeDownY, maxChangeY);
    change.rotation = Math::clamp(change.rotation, -maxChangeDownTurn, maxChangeTurn);

    step.translation = lastStep.translation + change.translation;
    step.rotation = Math::normalize(lastStep.rotation + change.rotation);
}
