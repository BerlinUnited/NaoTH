/**
* @file SelflocSymbols.cpp
*
* @author <a href="mailto:martius@informatik.hu-berlin.de">Martin Martius</a>
* Implementation of class SelflocSymbols
*/

#include "SelflocSymbols.h"
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugDrawings.h"
#include "Representations/Infrastructure/GPSData.h"

SelflocSymbols* SelflocSymbols::theInstance = NULL;

void SelflocSymbols::registerSymbols(xabsl::Engine& engine)
{
  engine.registerDecimalInputSymbol("gps.x",&gpsData.data.translation.x);
  engine.registerDecimalInputSymbol("gps.y",&gpsData.data.translation.y);

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



  engine.registerBooleanInputSymbol("robot_pose.is_valid", &robotPose.isValid);

  engine.registerDecimalInputSymbol("robot_pose.x",&robotPose.translation.x);
  engine.registerDecimalInputSymbol("robot_pose.y",&robotPose.translation.y);
  engine.registerDecimalInputSymbol("robot_pose.rotation",&angleOnField);

  engine.registerDecimalInputSymbol("robot_pose.planned.x",&robotPosePlanned.translation.x);
  engine.registerDecimalInputSymbol("robot_pose.planned.y",&robotPosePlanned.translation.y);
  engine.registerDecimalInputSymbol("robot_pose.planned.rotation",&angleOnFieldPlanned);




  engine.registerDecimalInputSymbol("rel2fieldX", &getRel2fieldX);
  engine.registerDecimalInputSymbolDecimalParameter("rel2fieldX", "rel2fieldX.x", &rel2fieldX_x);
  engine.registerDecimalInputSymbolDecimalParameter("rel2fieldX", "rel2fieldX.y", &rel2fieldX_y);

  engine.registerDecimalInputSymbol("rel2fieldY", &getRel2fieldY);
  engine.registerDecimalInputSymbolDecimalParameter("rel2fieldY", "rel2fieldY.x", &rel2fieldY_x);
  engine.registerDecimalInputSymbolDecimalParameter("rel2fieldY", "rel2fieldY.y", &rel2fieldY_y);


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
  angleOnField = Math::toDegrees(robotPose.rotation);

  const GoalModel::Goal& ownGoalModel = selfLocGoalModel.getOwnGoal(compassDirection, fieldInfo);
  const GoalModel::Goal& oppGoalModel = selfLocGoalModel.getOppGoal(compassDirection, fieldInfo);
  ownGoal = Goal(ownGoalModel.leftPost, ownGoalModel.rightPost);
  oppGoal = Goal(oppGoalModel.leftPost, oppGoalModel.rightPost);

  robotPosePlanned = robotPose + motionStatus.plannedMotion.hip;
  angleOnFieldPlanned = Math::toDegrees(robotPosePlanned.rotation);
}//end update

double SelflocSymbols::getRel2fieldX()
{
  Pose2D result = theInstance->robotPose;
  Vector2<double> add;
  add.x = theInstance->rel2fieldX_x;
  add.y = theInstance->rel2fieldX_y;

  return (result*add).x;
}

double SelflocSymbols::getRel2fieldY()
{
  Pose2D result = theInstance->robotPose;
  Vector2<double> add;
  add.x = theInstance->rel2fieldY_x;
  add.y = theInstance->rel2fieldY_y;

  return (result*add).y;
}

double SelflocSymbols::getFieldToRelativeX()
{
  return (theInstance->robotPose/theInstance->parameterVector).x;
}//end getFieldToRelativeX

double SelflocSymbols::getFieldToRelativeY()
{
  return (theInstance->robotPose/theInstance->parameterVector).y;
}//end getFieldToRelativeY


