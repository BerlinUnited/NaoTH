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

  engine.registerBooleanInputSymbol("goal.opp.was_seen", &getOpponentGoalWasSeen);
  engine.registerBooleanInputSymbol("goal.own.was_seen", &getOwnGoalWasSeen);

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

  engine.registerDecimalInputSymbol("robot_pose.x",&getRobotPose().translation.x);
  engine.registerDecimalInputSymbol("robot_pose.y",&getRobotPose().translation.y);
  engine.registerDecimalInputSymbol("robot_pose.rotation",&angleOnField);

  engine.registerDecimalInputSymbol("robot_pose.planned.x",&robotPosePlanned.translation.x);
  engine.registerDecimalInputSymbol("robot_pose.planned.y",&robotPosePlanned.translation.y);
  engine.registerDecimalInputSymbol("robot_pose.planned.rotation",&angleOnFieldPlanned);


  // "field_to_relative.x"
  engine.registerDecimalInputSymbol("locator.field_to_relative.x", &getFieldToRelativeX);
  engine.registerDecimalInputSymbolDecimalParameter("locator.field_to_relative.x", "locator.field_to_relative.x.x", &parameterVector.x);
  engine.registerDecimalInputSymbolDecimalParameter("locator.field_to_relative.x", "locator.field_to_relative.x.y", &parameterVector.y);

  // "field_to_relative.y"
  engine.registerDecimalInputSymbol("locator.field_to_relative.y", &getFieldToRelativeY);
  engine.registerDecimalInputSymbolDecimalParameter("locator.field_to_relative.y", "locator.field_to_relative.y.x", &parameterVector.x);
  engine.registerDecimalInputSymbolDecimalParameter("locator.field_to_relative.y", "locator.field_to_relative.y.y", &parameterVector.y);

  engine.registerDecimalInputSymbol("look_in_direction_factor",&look_in_direction_factor);

  DEBUG_REQUEST_REGISTER("XABSL:draw_selfloc_goal", "draw the position of the goals calculated using the selflocalization", false);
}//end registerSymbols


void SelflocSymbols::execute()
{
  angleOnField = Math::toDegrees(getRobotPose().rotation);

  const GoalModel::Goal& ownGoalModel = getSelfLocGoalModel().getOwnGoal(getCompassDirection(), getFieldInfo());
  const GoalModel::Goal& oppGoalModel = getSelfLocGoalModel().getOppGoal(getCompassDirection(), getFieldInfo());
  ownGoal = Goal(ownGoalModel.leftPost, ownGoalModel.rightPost);
  oppGoal = Goal(oppGoalModel.leftPost, oppGoalModel.rightPost);

  robotPosePlanned = getRobotPose() + getMotionStatus().plannedMotion.hip;
  angleOnFieldPlanned = Math::toDegrees(robotPosePlanned.rotation);

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
}//end execute

double SelflocSymbols::getFieldToRelativeX()
{
  return (theInstance->getRobotPose()/theInstance->parameterVector).x;
}

double SelflocSymbols::getFieldToRelativeY()
{
  return (theInstance->getRobotPose()/theInstance->parameterVector).y;
}

bool SelflocSymbols::getOpponentGoalWasSeen()
{
  return  theInstance->getLocalGoalModel().opponentGoalIsValid && 
          theInstance->getLocalGoalModel().someGoalWasSeen;
}

bool SelflocSymbols::getOwnGoalWasSeen()
{
  return  theInstance->getLocalGoalModel().ownGoalIsValid && 
          theInstance->getLocalGoalModel().someGoalWasSeen;
}
