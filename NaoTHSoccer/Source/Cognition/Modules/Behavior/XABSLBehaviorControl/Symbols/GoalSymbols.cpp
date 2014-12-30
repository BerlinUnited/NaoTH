/**
 * @file GoalSymbols.cpp
 *
 * @author <a href="mailto:martius@informatik.hu-berlin.de">Martin Martius</a>
 * Implementation of class GoalSymbols
 */

#include "GoalSymbols.h"

void GoalSymbols::registerSymbols(xabsl::Engine& engine)
{
  // a whole goal was seen
  engine.registerDecimalInputSymbol("goal.own.whole.time_since_seen", &getTimeSinceWholeOwnGoalSeen);
  engine.registerDecimalInputSymbol("goal.opp.whole.time_since_seen", &getTimeSinceWholeOppGoalSeen);
  
  // at least one goal post was seen
  engine.registerDecimalInputSymbol("goal.opp.time_since_seen", &getTimeSinceOpponentGoalSeen);
  engine.registerDecimalInputSymbol("goal.own.time_since_seen", &getTimeSinceOwnGoalSeen);


  //engine.registerDecimalInputSymbol("goal.opp.x", &getOpponentGoalX);
  //engine.registerDecimalInputSymbol("goal.opp.y", &getOpponentGoalY);
  //engine.registerDecimalInputSymbol("goal.opp.angle", &getAngleToOpponentGoal);
  //engine.registerDecimalInputSymbol("goal.opp.distance", &getDistanceToOpponentGoal);

  //engine.registerDecimalInputSymbol("goal.own.x", &getOwnGoalX);
  //engine.registerDecimalInputSymbol("goal.own.y", &getOwnGoalY);
  //engine.registerDecimalInputSymbol("goal.own.angle", &getAngleToOwnGoal);
  //engine.registerDecimalInputSymbol("goal.own.distance", &getDistanceToOwnGoal);


  // goal percept symbols
  engine.registerDecimalInputSymbol("goal.centroid.x", &getGoalCentroidX);
  engine.registerDecimalInputSymbol("goal.centroid.y", &getGoalCentroidY);
  engine.registerDecimalInputSymbol("goal.centroid.z", &getGoalCentroidZ);



  engine.registerBooleanInputSymbol("goal.opp.was_seen", &getOpponentGoalWasSeen);
  engine.registerBooleanInputSymbol("goal.own.was_seen", &getOwnGoalWasSeen);

  //to provide that the goal model is valid, when to clusters are build
  engine.registerBooleanInputSymbol("goal.opp.isValid", &getLocalGoalModel().opponentGoalIsValid);


  // 
  engine.registerDecimalInputSymbol("goal.opp.seen_angle", &getAngleToOpponentGoal);
  //engine.registerDecimalInputSymbol("goal.own.seen_angle", &getAngleToOwnGoal);

  engine.registerDecimalInputSymbol("goal.opp.seen_center.x", &getLocalGoalModel().seen_center.x);
  engine.registerDecimalInputSymbol("goal.opp.seen_center.y", &getLocalGoalModel().seen_center.y);

}//end registerSymbols

GoalSymbols* GoalSymbols::theInstance = NULL;

void GoalSymbols::execute()
{
}

double GoalSymbols::getTimeSinceWholeOwnGoalSeen()
{
  const GoalModel::Goal& goal = theInstance->getSensingGoalModel().getOwnGoal(theInstance->getCompassDirection(), theInstance->getFieldInfo());
  return (double) theInstance->getFrameInfo().getTimeSince(goal.frameInfoWhenGoalLastSeen.getTime());
}

double GoalSymbols::getTimeSinceWholeOppGoalSeen()
{
  const GoalModel::Goal& goal = theInstance->getSensingGoalModel().getOwnGoal(theInstance->getCompassDirection(), theInstance->getFieldInfo());
  return (double) theInstance->getFrameInfo().getTimeSince(goal.frameInfoWhenGoalLastSeen.getTime());
}

double GoalSymbols::getOpponentGoalX()
{
  return theInstance->getLocalGoalModel().getOppGoal(theInstance->getCompassDirection(), theInstance->getFieldInfo()).calculateCenter().x;
}

double GoalSymbols::getOpponentGoalY()
{
  return theInstance->getLocalGoalModel().getOppGoal(theInstance->getCompassDirection(), theInstance->getFieldInfo()).calculateCenter().y;
}

double GoalSymbols::getAngleToOpponentGoal()
{
  return Math::toDegrees(theInstance->getLocalGoalModel().seen_angle);
}

double GoalSymbols::getDistanceToOpponentGoal()
{
  return theInstance->getLocalGoalModel().getOppGoal(theInstance->getCompassDirection(), theInstance->getFieldInfo()).calculateCenter().abs();
}

double GoalSymbols::getTimeSinceOpponentGoalSeen()
{
  return (double) theInstance->getFrameInfo().getTimeSince(
    theInstance->getLocalGoalModel().frameWhenOpponentGoalWasSeen.getTime());
}

double GoalSymbols::getOwnGoalX()
{
  return theInstance->getLocalGoalModel().getOwnGoal(theInstance->getCompassDirection(), theInstance->getFieldInfo()).calculateCenter().x;
}

double GoalSymbols::getOwnGoalY()
{
  return theInstance->getLocalGoalModel().getOwnGoal(theInstance->getCompassDirection(), theInstance->getFieldInfo()).calculateCenter().y;
}

double GoalSymbols::getGoalCentroidX()
{
  const GoalPercept& m = theInstance->getGoalPercept();
  return m.goalCentroid.x;
}

double GoalSymbols::getGoalCentroidY()
{
  const GoalPercept& m = theInstance->getGoalPercept();
  return m.goalCentroid.y;
}

double GoalSymbols::getGoalCentroidZ()
{
  const GoalPercept& m = theInstance->getGoalPercept();
  return m.goalCentroid.z;
}

double GoalSymbols::getAngleToOwnGoal()
{
  double radAngle = theInstance->getLocalGoalModel().getOwnGoal(theInstance->getCompassDirection(), theInstance->getFieldInfo()).calculateCenter().angle();
  return Math::toDegrees(radAngle);
}

double GoalSymbols::getDistanceToOwnGoal()
{
  return theInstance->getLocalGoalModel().getOwnGoal(theInstance->getCompassDirection(), theInstance->getFieldInfo()).calculateCenter().abs();
}

double GoalSymbols::getTimeSinceOwnGoalSeen()
{
  return (double) theInstance->getFrameInfo().getTimeSince(
    theInstance->getLocalGoalModel().getOwnGoal(theInstance->getCompassDirection(), theInstance->getFieldInfo()).frameInfoWhenGoalLastSeen.getTime());
}

bool GoalSymbols::getOpponentGoalWasSeen()
{
  return  theInstance->getLocalGoalModel().opponentGoalIsValid && 
          theInstance->getLocalGoalModel().someGoalWasSeen;
}

bool GoalSymbols::getOwnGoalWasSeen()
{
  return  theInstance->getLocalGoalModel().ownGoalIsValid && 
          theInstance->getLocalGoalModel().someGoalWasSeen;
}

