/**
* @file FootStepPlanner.cpp
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* plan the foot step according to motion request
*/

#include "ZMPPlanner2018.h"

using namespace InverseKinematic;

void ZMPPlanner2018::init(size_t initial_number_of_cycles, Vector3d initialZMP, Vector3d targetZMP){
    for (size_t i = 0; i+1 < initial_number_of_cycles; i++) {
      double t = static_cast<double>(i) / static_cast<double>(initial_number_of_cycles - 1); // why? it should be only initial_number_of_cycles
      Vector3d zmp = initialZMP*(1.0-t) + targetZMP*t;
      getZMPReferenceBuffer().push(zmp);
    }
}

void ZMPPlanner2018::execute(){
    Step& planningStep = getStepBuffer().last();

    Vector3d zmp;
    Vector3d other_zmp;
    if(planningStep.footStep.liftingFoot() == FootStep::NONE)
    {
      if(parameters.newZMP_ON) {
        zmp = calculateStableCoMByFeet(planningStep.footStep.end(), parameters.bezierZMP.offsetX);
      } else {
        zmp = calculateStableCoMByFeet(planningStep.footStep.end(), parameters.simpleZMP.offsetX);
      }
    } else {
        if(planningStep.planningCycle == 0){ // only need to be done once per step
            Pose3D startFoot, targetFoot;
            InverseKinematic::FeetPose begin = planningStep.footStep.begin();
            InverseKinematic::FeetPose end   = planningStep.footStep.end();
            if(planningStep.footStep.liftingFoot() == FootStep::LEFT){
                begin.localInRightFoot();
                end.localInRightFoot();

                startFoot  = begin.left;
                targetFoot = end.left;
            } else {
                begin.localInLeftFoot();
                end.localInLeftFoot();

                startFoot  = begin.right;
                targetFoot = end.right;
            }

            Vector2d currentStepLength = targetFoot.projectXY().translation - startFoot.projectXY().translation;

            // HACK: hard coded maximal step lengths
            // TODO: replace maxHipOffsetBasedOnStepLength by a scaling factor
            //       alternative: remove it at all, it isn't really used and its effectiveness is questionable
            double maxStepLength = 50;
            double maxCtrlLength = 80;

            PLOT("Walk:hipOffsetBasedOnStepLength.x", parameters.stabilization.maxHipOffsetBasedOnStepLength.x * std::abs(currentStepLength.x)/maxStepLength);

            // TODO: should it be a part of the Step?
            // TODO: hipOffsetBasedOnStepLength.y?
            if (planningStep.type == Step::STEP_CONTROL && planningStep.walkRequest.stepControl.type == WalkRequest::StepControlRequest::KICKSTEP) {
                zmpOffsetX = parameters.simpleZMP.offsetX
                           + parameters.stabilization.maxHipOffsetBasedOnStepLengthForKicks.x * ((currentStepLength.x > 0) ? currentStepLength.x / maxCtrlLength : 0);
                zmpOffsetY = parameters.simpleZMP.kickOffsetY
                           + parameters.ZMPOffsetYByCharacter * (1-planningStep.walkRequest.character);

                newZMPOffsetX = parameters.bezierZMP.offsetXForKicks
                              + parameters.stabilization.maxHipOffsetBasedOnStepLengthForKicks.x * ((currentStepLength.x > 0) ? currentStepLength.x / maxCtrlLength : 0);
                newZMPOffsetY = parameters.bezierZMP.offsetYForKicks
                              + parameters.ZMPOffsetYByCharacter * (1-planningStep.walkRequest.character);
            } else {
                zmpOffsetX = parameters.simpleZMP.offsetX
                           + parameters.stabilization.maxHipOffsetBasedOnStepLength.x * ((currentStepLength.x > 0) ? currentStepLength.x / maxStepLength : 0);
                zmpOffsetY = parameters.simpleZMP.offsetY
                           + parameters.ZMPOffsetYByCharacter * (1-planningStep.walkRequest.character);

                newZMPOffsetX = parameters.bezierZMP.offsetX
                              + parameters.stabilization.maxHipOffsetBasedOnStepLength.x * ((currentStepLength.x > 0) ? currentStepLength.x / maxStepLength : 0);
                newZMPOffsetY = parameters.bezierZMP.offsetY
                              + parameters.ZMPOffsetYByCharacter * (1-planningStep.walkRequest.character);
            }
        }

        Vector2d zmp_new;
        zmp_new = bezierBased(
                    planningStep.footStep,
                    planningStep.planningCycle,
                    planningStep.samplesDoubleSupport,
                    planningStep.samplesSingleSupport,
                    newZMPOffsetX,
                    newZMPOffsetY,
                    parameters.bezierZMP.inFootScalingY,
                    parameters.bezierZMP.inFootSpacing,
                    parameters.bezierZMP.transitionScaling);

        // old zmp
        Vector2d zmp_simple = simplest(planningStep.footStep, zmpOffsetX, zmpOffsetY);

        if(parameters.newZMP_ON)
        {
            other_zmp = Vector3d(zmp_simple.x, zmp_simple.y, parameters.comHeight);
            zmp       = Vector3d(zmp_new.x, zmp_new.y, parameters.comHeight);

            // HACK:
            if (parameters.useZMPHackForKicks && planningStep.type == Step::STEP_CONTROL && planningStep.walkRequest.stepControl.type == WalkRequest::StepControlRequest::KICKSTEP) {
              zmp.x = other_zmp.x;
            }
        } else {
            zmp       = Vector3d(zmp_simple.x, zmp_simple.y, parameters.comHeight);
            other_zmp = Vector3d(zmp_new.x, zmp_new.y, parameters.comHeight);
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
  supFoot.translate(offsetX, offsetY * static_cast<double>(step.liftingFoot()), 0);
  return Vector2d(supFoot.translation.x, supFoot.translation.y);
}

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

  double supFootY = supFoot.translation.y + offsetY * static_cast<double>(step.liftingFoot());

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

Vector3d ZMPPlanner2018::calculateStableCoMByFeet(FeetPose feet, double offsetX) const
{
  feet.left.translate(offsetX, 0, 0);
  feet.right.translate(offsetX, 0, 0);

  Vector3d com;
  com = (feet.left.translation + feet.right.translation) * 0.5;
  com.z = parameters.comHeight;

  return com;
}
