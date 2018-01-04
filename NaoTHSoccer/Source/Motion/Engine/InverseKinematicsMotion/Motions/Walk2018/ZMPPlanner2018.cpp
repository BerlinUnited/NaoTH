/**
* @file FootStepPlanner.cpp
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* plan the foot step according to motion request
*/

#include "ZMPPlanner2018.h"

using namespace InverseKinematic;

void ZMPPlanner2018::execute(){
    ASSERT(!getStepBuffer().empty());

    Step& planningStep = getStepBuffer().last();
    ASSERT(!planningStep.isPlanned());

    Vector3d zmp;
    Vector3d other_zmp;
    if(planningStep.footStep.liftingFoot() == FootStep::NONE)
    {
      Pose3D finalBody = calculateStableCoMByFeet(planningStep.footStep.end(), parameters().general.bodyPitchOffset);
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
        if (planningStep.type == Step::STEP_CONTROL && planningStep.walkRequest.stepControl.type == WalkRequest::StepControlRequest::KICKSTEP)
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
        zmp_new = bezierBased(
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
        Vector2d zmp_simple = simplest(planningStep.footStep, zmpOffsetX, zmpOffsetY);

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
    //getEngine().zmpControl.push(zmp);
    getZMPReferenceBuffer().push(zmp);

    DEBUG_REQUEST("Walk:draw_step_plan_geometry",
            FIELD_DRAWING_CONTEXT;
            getDebugDrawings().pen(Color::BLUE, 5.0);
            getDebugDrawings().drawCircle(zmp.x, zmp.y, 10);
    );

    PLOT("Walk:DRAW_ZMP_x", zmp.x);
    PLOT("Walk:DRAW_ZMP_y", zmp.y);

    planningStep.planningCycle++;
}

Vector2d ZMPPlanner2018::simplest(const FootStep& step, double offsetX, double offsetY)
{
  Pose3D supFoot = step.supFoot();
  supFoot.translate(offsetX, offsetY * step.liftingFoot(), 0);
  return Vector2d(supFoot.translation.x, supFoot.translation.y);
}//end simplest

Vector2d ZMPPlanner2018::bezierBased(
  const FootStep step,
  double cycle,
  double samplesDoubleSupport, double samplesSingleSupport,
  double offsetX, double offsetY,
  double inFootScalingY,
  double inFootSpacingY,
  double transitionScaling)
{
    Pose3D supFoot, startFoot, targetFoot;

    // poses in support foot coordinates
    if(step.liftingFoot() == FootStep::LEFT){
        InverseKinematic::FeetPose begin = step.begin();
        begin.localInRightFoot();
        InverseKinematic::FeetPose end   = step.end();
        end.localInRightFoot();

        supFoot    = begin.right;
        startFoot  = begin.left;
        targetFoot = end.left;

        inFootSpacingY *= -1;
    } else {
        InverseKinematic::FeetPose begin = step.begin();
        begin.localInLeftFoot();
        InverseKinematic::FeetPose end   = step.end();
        end.localInLeftFoot();

        supFoot    = begin.left;
        startFoot  = begin.right;
        targetFoot = end.right;
    }

  // Don't apply offset befor determining start and target for y.
  // Applying offsets first will add discontinuities in y (start of step i won't be target of step i-1)
  // because the y offset will be applied in different directions depending on the lifting foot
  double start_y  = (supFoot.translation.y + startFoot.translation.y)/2;
  double target_y = (supFoot.translation.y + targetFoot.translation.y)/2;

  double start_x  = (supFoot.translation.x + startFoot.translation.x)/2  + offsetX;
  double target_x = (supFoot.translation.x + targetFoot.translation.x)/2 + offsetX;

  double supFootY = supFoot.translation.y + offsetY * step.liftingFoot();

  static std::vector<Vector2d> trajectory;
  static unsigned int idx;

  if(cycle < 0.000001 || trajectory.empty()){
      idx = 0;
      trajectory.clear();
      static std::vector<Vector2d> temp;

      // trajectory from start to foot as (time, value) pairs
      std::vector<Vector2d> start_to_foot = {Vector2d(0.0,start_y),
                                             Vector2d(transitionScaling/4,(start_y+supFootY-inFootSpacingY)/2),
                                             Vector2d(transitionScaling/4,(start_y+supFootY-inFootSpacingY)/2),
                                             Vector2d(transitionScaling/2, supFootY-inFootSpacingY)};
      temp = FourPointBezier2D(start_to_foot, 200);
      trajectory.insert(trajectory.end(),temp.begin(),temp.end());

      // trajectory in foot as (time, value) pairs
      Vector2d inFootStart = Vector2d(transitionScaling/2,supFootY-inFootSpacingY);
      Vector2d inDirection = Vector2d(transitionScaling/2+transitionScaling/4,supFootY-inFootSpacingY+(supFootY-inFootSpacingY-start_y)/2) - inFootStart;

      Vector2d inFootEnd    = Vector2d(1-transitionScaling/2, supFootY-inFootSpacingY);
      Vector2d outDirection = inFootEnd - Vector2d(1-transitionScaling/2-transitionScaling/4,supFootY-inFootSpacingY+(supFootY-inFootSpacingY-target_y)/2);

      //scaling directions to let the control points be on supFootY+inFootSpacingY
      inDirection  *= (supFootY+inFootSpacingY)/inDirection.y;
      outDirection *= -(supFootY+inFootSpacingY)/outDirection.y;

      std::vector<Vector2d> in_foot =  {inFootStart,
                                        inFootStart + (inDirection  * inFootScalingY),
                                        inFootEnd   - (outDirection * inFootScalingY),
                                        inFootEnd};

      temp = FourPointBezier2D(in_foot, 200);
      trajectory.insert(trajectory.end(),temp.begin(),temp.end());

      // trajectory from foot to target as (time, value) pairs
      std::vector<Vector2d> foot_to_target = {Vector2d(1-transitionScaling/2,supFootY-inFootSpacingY),
                                              Vector2d(1-transitionScaling/4,(target_y+supFootY-inFootSpacingY)/2),
                                              Vector2d(1-transitionScaling/4,(target_y+supFootY-inFootSpacingY)/2),
                                              Vector2d(1.0, target_y)};
      temp = FourPointBezier2D(foot_to_target, 200);
      trajectory.insert(trajectory.end(),temp.begin(),temp.end());
  }

  double t = cycle / (samplesSingleSupport+samplesDoubleSupport);
  for(; trajectory[idx].x < t && idx+1 < trajectory.size(); ++idx);

  double y = trajectory[idx].y;
  double x = start_x*(1.0-t) + t*target_x;

  return step.supFoot().projectXY()*Vector2d(x, y);
}//end bezierBased

Pose3D ZMPPlanner2018::calculateStableCoMByFeet(FeetPose feet, double pitch) const
{
  feet.left.translate(parameters().general.hipOffsetX, 0, 0);
  feet.right.translate(parameters().general.hipOffsetX, 0, 0);

  Pose3D com;
  com.rotation = calculateBodyRotation(feet, pitch);
  com.translation = (feet.left.translation + feet.right.translation) * 0.5;
  com.translation.z = parameters().hip.comHeight;

  return com;
}

RotationMatrix ZMPPlanner2018::calculateBodyRotation(const FeetPose& feet, double pitch) const
{
  double bodyAngleZ = Math::meanAngle(feet.left.rotation.getZAngle(), feet.right.rotation.getZAngle());
  return RotationMatrix::getRotationZ(bodyAngleZ) * RotationMatrix::getRotationY(pitch);
}
