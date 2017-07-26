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
}//end execute

double SelflocSymbols::getFieldToRelativeX()
{
  return (theInstance->getRobotPose()/theInstance->parameterVector).x;
}

double SelflocSymbols::getFieldToRelativeY()
{
  return (theInstance->getRobotPose()/theInstance->parameterVector).y;
}


