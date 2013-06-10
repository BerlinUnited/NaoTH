/**
 * @file GoalSymbols2012.cpp
 *
 * @author <a href="mailto:martius@informatik.hu-berlin.de">Martin Martius</a>
 * Implementation of class GoalSymbols2012
 */



#include "GoalSymbols2012.h"

void GoalSymbols2012::registerSymbols(xabsl::Engine& engine)
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
  engine.registerBooleanInputSymbol("goal.opp.isValid", &localGoalModel.opponentGoalIsValid);


  // 
  engine.registerDecimalInputSymbol("goal.opp.seen_angle", &getAngleToOpponentGoal);
  //engine.registerDecimalInputSymbol("goal.own.seen_angle", &getAngleToOwnGoal);

  engine.registerDecimalInputSymbol("goal.opp.seen_center.x", &localGoalModel.seen_center.x);
  engine.registerDecimalInputSymbol("goal.opp.seen_center.y", &localGoalModel.seen_center.y);

}//end registerSymbols

GoalSymbols2012* GoalSymbols2012::theInstance = NULL;

void GoalSymbols2012::execute()
{
}


double GoalSymbols2012::getTimeSinceWholeOwnGoalSeen()
{
  const GoalModel::Goal& goal = theInstance->getSensingGoalModel().getOwnGoal(theInstance->compassDirection, theInstance->fieldInfo);
  return (double) theInstance->frameInfo.getTimeSince(goal.frameInfoWhenGoalLastSeen.getTime());
}//end getTimeSinceWholeOwnGoalSeen

double GoalSymbols2012::getTimeSinceWholeOppGoalSeen()
{
  const GoalModel::Goal& goal = theInstance->getSensingGoalModel().getOwnGoal(theInstance->compassDirection, theInstance->fieldInfo);
  return (double) theInstance->frameInfo.getTimeSince(goal.frameInfoWhenGoalLastSeen.getTime());
}//end getTimeSinceWholeOppGoalSeen


double GoalSymbols2012::getOpponentGoalX()
{
  return theInstance->localGoalModel.getOppGoal(theInstance->compassDirection, theInstance->fieldInfo).calculateCenter().x;
}//end getOpponentGoalX

double GoalSymbols2012::getOpponentGoalY()
{
  return theInstance->localGoalModel.getOppGoal(theInstance->compassDirection, theInstance->fieldInfo).calculateCenter().y;
}//end getOpponentGoalY

double GoalSymbols2012::getAngleToOpponentGoal()
{
  return Math::toDegrees(theInstance->localGoalModel.seen_angle);
}//end getAngleToOpGoal

double GoalSymbols2012::getDistanceToOpponentGoal()
{
  return theInstance->localGoalModel.getOppGoal(theInstance->compassDirection, theInstance->fieldInfo).calculateCenter().abs();
}//end getDistanceToOpponentGoal

double GoalSymbols2012::getTimeSinceOpponentGoalSeen()
{
  return (double) theInstance->frameInfo.getTimeSince(
    theInstance->localGoalModel.frameWhenOpponentGoalWasSeen.getTime());
}//end getTimeSinceOpponentGoalSeen


double GoalSymbols2012::getOwnGoalX()
{
  return theInstance->localGoalModel.getOwnGoal(theInstance->compassDirection, theInstance->fieldInfo).calculateCenter().x;
}//end getOwnGoalX

double GoalSymbols2012::getOwnGoalY()
{
  return theInstance->localGoalModel.getOwnGoal(theInstance->compassDirection, theInstance->fieldInfo).calculateCenter().y;
}//end getOwnGoalY

double GoalSymbols2012::getGoalCentroidX()
{
  const GoalPercept& m = theInstance->goalPercept;
  return m.goalCentroid.x;
}//end getGoalCentroidX

double GoalSymbols2012::getGoalCentroidY()
{
  const GoalPercept& m = theInstance->goalPercept;
  return m.goalCentroid.y;
}//end getGoalCentroidY

double GoalSymbols2012::getGoalCentroidZ()
{
  const GoalPercept& m = theInstance->goalPercept;
  return m.goalCentroid.z;
}//end getGoalCentroidZ

double GoalSymbols2012::getAngleToOwnGoal()
{
  double radAngle = theInstance->localGoalModel.getOwnGoal(theInstance->compassDirection, theInstance->fieldInfo).calculateCenter().angle();
  return Math::toDegrees(radAngle);
}//end getAngleToOwnGoal

double GoalSymbols2012::getDistanceToOwnGoal()
{
  return theInstance->localGoalModel.getOwnGoal(theInstance->compassDirection, theInstance->fieldInfo).calculateCenter().abs();
}//end getDistanceToOwnGoal

double GoalSymbols2012::getTimeSinceOwnGoalSeen()
{
  return (double) theInstance->frameInfo.getTimeSince(
    theInstance->localGoalModel.getOwnGoal(theInstance->compassDirection, theInstance->fieldInfo).frameInfoWhenGoalLastSeen.getTime());
}//end getTimeSinceOpponentGoalSeen


//FIXME not via color decideable!
bool GoalSymbols2012::getOpponentGoalWasSeen()
{
 /*
  ColorClasses::Color goalColor = (theInstance->playerInfo.gameData.teamColor == GameData::blue)?ColorClasses::yellow:ColorClasses::skyblue;

  for(unsigned int i = 0; i < theInstance->goalPercept.getNumberOfSeenPosts(); i++)
    if(theInstance->goalPercept.getPost(i).color == goalColor && 
       theInstance->goalPercept.getPost(i).positionReliable)
      return true;
*/

  return theInstance->localGoalModel.opponentGoalIsValid && 
    theInstance->localGoalModel.someGoalWasSeen;
}//end getOpponentGoalWasSeen

bool GoalSymbols2012::getOwnGoalWasSeen()
{
  return theInstance->localGoalModel.ownGoalIsValid && 
    theInstance->localGoalModel.someGoalWasSeen;
}//end getOpponentGoalWasSeen

