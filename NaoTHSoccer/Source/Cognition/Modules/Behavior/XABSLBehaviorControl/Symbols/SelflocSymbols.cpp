/**
* @file SelflocSymbols.cpp
*
* @author <a href="mailto:martius@informatik.hu-berlin.de">Martin Martius</a>
* Implementation of class SelflocSymbols
*/

#include "SelflocSymbols.h"

SelflocSymbols* SelflocSymbols::theInstance = NULL;

void SelflocSymbols::registerSymbols(xabsl::Engine& engine)
{
  engine.registerDecimalInputSymbol("gps.x",&getGPSData().data.translation.x);
  engine.registerDecimalInputSymbol("gps.y",&getGPSData().data.translation.y);

  // goal symbols based on self localization
  engine.registerDecimalInputSymbol("goal.opp.x", &oppGoal.center.x);
  engine.registerDecimalInputSymbol("goal.opp.y", &oppGoal.center.y);
  engine.registerDecimalInputSymbol("goal.opp.angle", &oppGoal.angleToCenter);
  engine.registerDecimalInputSymbol("goal.opp.distance", &oppGoal.distanceToCenter);

  engine.registerDecimalInputSymbol("goal.opp.left_post.x", &oppGoal.leftPost.x);
  engine.registerDecimalInputSymbol("goal.opp.left_post.y", &oppGoal.leftPost.y);
  engine.registerDecimalInputSymbol("goal.opp.right_post.x", &oppGoal.rightPost.x);
  engine.registerDecimalInputSymbol("goal.opp.right_post.y", &oppGoal.rightPost.y);


  engine.registerDecimalInputSymbol("goal.own.x", &ownGoal.center.x);
  engine.registerDecimalInputSymbol("goal.own.y", &ownGoal.center.y);
  engine.registerDecimalInputSymbol("goal.own.angle", &ownGoal.angleToCenter);
  engine.registerDecimalInputSymbol("goal.own.distance", &ownGoal.distanceToCenter);

  engine.registerDecimalInputSymbol("goal.own.left_post.x", &ownGoal.leftPost.x);
  engine.registerDecimalInputSymbol("goal.own.left_post.y", &ownGoal.leftPost.y);
  engine.registerDecimalInputSymbol("goal.own.right_post.x", &ownGoal.rightPost.x);
  engine.registerDecimalInputSymbol("goal.own.right_post.y", &ownGoal.rightPost.y);



  engine.registerBooleanInputSymbol("robot_pose.is_valid", &getRobotPose().isValid);
  engine.registerBooleanInputSymbol("robot_pose.is_mirrored", &getRobotPose().globallyMirrored);

  engine.registerDecimalInputSymbol("robot_pose.x",&getRobotPose().translation.x);
  engine.registerDecimalInputSymbol("robot_pose.y",&getRobotPose().translation.y);
  engine.registerDecimalInputSymbol("robot_pose.rotation",&angleOnField);

  engine.registerDecimalInputSymbol("robot_pose.planned.x",&robotPosePlanned.translation.x);
  engine.registerDecimalInputSymbol("robot_pose.planned.y",&robotPosePlanned.translation.y);
  engine.registerDecimalInputSymbol("robot_pose.planned.rotation",&angleOnFieldPlanned);


  engine.registerDecimalInputSymbol("robot_pose.charging_target.x",&targetLocal.translation.x);
  engine.registerDecimalInputSymbol("robot_pose.charging_target.y",&targetLocal.translation.y);
  engine.registerDecimalInputSymbol("robot_pose.charging_target.rotation",&targetLocalRotation);
  

  // "field_to_relative.x"
  engine.registerDecimalInputSymbol("locator.field_to_relative.x", &getFieldToRelativeX);
  engine.registerDecimalInputSymbolDecimalParameter("locator.field_to_relative.x", "locator.field_to_relative.x.x", &parameterVector.x);
  engine.registerDecimalInputSymbolDecimalParameter("locator.field_to_relative.x", "locator.field_to_relative.x.y", &parameterVector.y);

  // "field_to_relative.y"
  engine.registerDecimalInputSymbol("locator.field_to_relative.y", &getFieldToRelativeY);
  engine.registerDecimalInputSymbolDecimalParameter("locator.field_to_relative.y", "locator.field_to_relative.y.x", &parameterVector.x);
  engine.registerDecimalInputSymbolDecimalParameter("locator.field_to_relative.y", "locator.field_to_relative.y.y", &parameterVector.y);

  engine.registerDecimalInputSymbol("look_in_direction_factor",&look_in_direction_factor);

  engine.registerBooleanInputSymbol("robot_pose.bdr.close_to_border", &toCloseToBorder);
  engine.registerDecimalInputSymbol("robot_pose.bdr.close_to_border.correction.x", &safePoint.x);
  engine.registerDecimalInputSymbol("robot_pose.bdr.close_to_border.correction.y", &safePoint.y);

  engine.registerDecimalInputSymbol("field.center.x", &fieldCenter.x);
  engine.registerDecimalInputSymbol("field.center.y", &fieldCenter.y);

  DEBUG_REQUEST_REGISTER("XABSL:draw_selfloc_goal", "draw the position of the goals calculated using the selflocalization", false);
}//end registerSymbols


void SelflocSymbols::execute()
{
  angleOnField = Math::toDegrees(getRobotPose().rotation);

  const GoalModel::Goal& ownGoalModel = getSelfLocGoalModel().getOwnGoal(getCompassDirection(), getFieldInfo());
  const GoalModel::Goal& oppGoalModel = getSelfLocGoalModel().getOppGoal(getCompassDirection(), getFieldInfo());
  ownGoal = Goal(ownGoalModel.leftPost, ownGoalModel.rightPost);
  oppGoal = Goal(oppGoalModel.leftPost, oppGoalModel.rightPost);


  // planed pose
  robotPosePlanned = getRobotPose() + getMotionStatus().plannedMotion.hip;
  angleOnFieldPlanned = Math::toDegrees(robotPosePlanned.rotation);

  const Pose3D& hip = getKinematicChain().getLink(KinematicChain::Hip).M;
  
  // feet in local coordinates
  const Pose3D& lfoot = getKinematicChain().getLink(KinematicChain::LFoot).M;
  Pose2D lfoot2d = hip.local(lfoot).projectXY() + getMotionStatus().plannedMotion.lFoot;

  const Pose3D& rfoot = getKinematicChain().getLink(KinematicChain::RFoot).M;
  Pose2D rfoot2d = hip.local(rfoot).projectXY() + getMotionStatus().plannedMotion.rFoot;

  Pose2D plannedOrigin(Math::meanAngle(lfoot2d.getAngle(),rfoot2d.getAngle()), (lfoot2d.translation + rfoot2d.translation)*0.5);

  robotPosePlanned = getRobotPose() + plannedOrigin;
  angleOnFieldPlanned = Math::toDegrees(robotPosePlanned.rotation);

  // TODO: das soll in die BDR symbols
  Pose2D targetGlobal = parameters.targetPose;
  if (getRobotPose().globallyMirrored) {
    targetGlobal.rotation *= -1;
    targetGlobal.translation.y *= -1;
  }

  targetLocal = robotPosePlanned.invert()*targetGlobal;
  targetLocalRotation = Math::toDegrees(targetLocal.rotation);

  DEBUG_REQUEST("SelflocSymbols:draw_global_origin",
    FIELD_DRAWING_CONTEXT;
    //PEN("FFFFFF", 20);
    //ROBOT(robotPosePlanned.translation.x, robotPosePlanned.translation.y, robotPosePlanned.rotation);
    PEN("000000", 20);
    CIRCLE(robotPosePlanned.translation.x, robotPosePlanned.translation.y, 10);

    PEN("FF0000", 20);
    CIRCLE(targetLocal.translation.x, targetLocal.translation.y, 10);
    ARROW(targetLocal.translation.x, targetLocal.translation.y, 
          targetLocal.translation.x + 100*cos(targetLocal.rotation), 
          targetLocal.translation.y + 100*sin(targetLocal.rotation));

    PEN("0000FF", 20);
    CIRCLE(targetGlobal.translation.x, targetGlobal.translation.y, 10);
    ARROW(targetGlobal.translation.x, targetGlobal.translation.y, 
          targetGlobal.translation.x + 100*cos(targetGlobal.rotation), 
          targetGlobal.translation.y + 100*sin(targetGlobal.rotation));
  );

  // calculate the distance to the sidelines
  double distance2back = getFieldInfo().xLength/2.0 + (std::abs(getRobotPose().translation.x) * Math::sgn(getRobotPose().translation.x));
  double distance2right = getFieldInfo().yLength/2.0 + (std::abs(getRobotPose().translation.y) * Math::sgn(getRobotPose().translation.y));
  // clip too large/small distance (out of bounds)
  distance2back = distance2back < 0 ? 0 : (distance2back > getFieldInfo().xLength? getFieldInfo().xLength : distance2back);
  distance2right = distance2right < 0 ? 0 : (distance2right > getFieldInfo().yLength ? getFieldInfo().yLength : distance2right);
  // distance in the other direction is the "complement"
  double distance2front = getFieldInfo().xLength - distance2back;
  double distance2left = getFieldInfo().yLength - distance2right;
  // a factor describing the impact of each distance
  double distance_factor_left = (distance2left / getFieldInfo().yLength) * 0.5;
  double distance_factor_right = (distance2right / getFieldInfo().yLength) * 0.5;
  double distance_factor_front = (distance2front / getFieldInfo().xLength) * 0.5;
  double distance_factor_back = (distance2back / getFieldInfo().xLength) * 0.5;
  // determines the direction where the robot is turned to
  double angle_left = cos(Math::fromDegrees(90 - angleOnField));
  double angle_front = cos(Math::fromDegrees(0 - angleOnField));
  double angle_right = cos(Math::fromDegrees(-90 - angleOnField));
  double angle_back = cos(Math::fromDegrees(180 - angleOnField));
  // factor describing how long the robot should look in the current direction and search for the ball
  look_in_direction_factor =
          (angle_left < 0 ? 0 : angle_left)  * distance_factor_left +
          (angle_front < 0 ? 0 : angle_front)* distance_factor_front +
          (angle_right < 0 ? 0 : angle_right)* distance_factor_right +
          (angle_back < 0 ? 0 : angle_back)  * distance_factor_back;



  {
    const double margin = 350;

    toCloseToBorder = false;

    if(getRobotPose().isValid)
    {
      Pose2D global = getRobotPose().getGlobalPose() + plannedOrigin;
      safePoint = global.translation;

      double diffMinX = global.translation.x - getFieldInfo().bdrCarpetRect.min().x;
      if(diffMinX < margin) {
        toCloseToBorder = true;
        safePoint.x += (margin*2.0 - diffMinX);
      }

      double diffMaxX = getFieldInfo().bdrCarpetRect.max().x - global.translation.x;
      if(diffMaxX < margin) {
        toCloseToBorder = true;
        safePoint.x -= (margin*2.0 - diffMaxX);
      }

      double diffMinY = global.translation.y - getFieldInfo().bdrCarpetRect.min().y;
      if(diffMinY < margin) {
        toCloseToBorder = true;
        safePoint.y += (margin*2.0 - diffMinY);
      }

      double diffMaxY = getFieldInfo().bdrCarpetRect.max().y - global.translation.y;
      if(diffMaxY < margin) {
        toCloseToBorder = true;
        safePoint.y -= (margin*2.0 - diffMaxY);
      }

      DEBUG_REQUEST("SelflocSymbols:draw_global_origin",
        FIELD_DRAWING_CONTEXT;
        PEN("000000", 20);
        ARROW(global.translation.x, global.translation.y, safePoint.x, safePoint.y);
      );

      // make local
      safePoint = global / safePoint;
    }
  }

  fieldCenter = getRobotPose().invert().translation;

}//end execute

double SelflocSymbols::getFieldToRelativeX()
{
  return (theInstance->getRobotPose()/theInstance->parameterVector).x;
}

double SelflocSymbols::getFieldToRelativeY()
{
  return (theInstance->getRobotPose()/theInstance->parameterVector).y;
}


