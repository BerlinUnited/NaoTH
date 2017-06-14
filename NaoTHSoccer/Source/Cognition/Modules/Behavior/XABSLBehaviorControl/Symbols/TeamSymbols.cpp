/**
 * @file: TeamSymbols.cpp
 * @author: <a href="mailto:scheunem@informatik.hu-berlin.de">Marcus Scheunemann</a>
 *
 * First created on 9. April 2009, 18:10
 */

#include "TeamSymbols.h"

void TeamSymbols::registerSymbols(xabsl::Engine& engine)
{
  engine.registerDecimalInputSymbol("team.members_alive_count", &getTeamMembersAliveCount);
  engine.registerBooleanInputSymbol("team.calc_if_is_striker", &calculateIfStriker);
  engine.registerBooleanInputSymbol("team.calc_if_is_secondstriker", &calculateIfSecondStriker);
  engine.registerBooleanOutputSymbol("team.is_playing_as_striker",&setWasStriker, &getWasStriker);
  engine.registerBooleanInputSymbol("team.calc_if_is_the_last", &calculateIfTheLast);
}


TeamSymbols* TeamSymbols::theInstance = NULL;

void TeamSymbols::execute()
{
}

double TeamSymbols::getTeamMembersAliveCount()
{
  int counter = 0;

  for(auto const &it : theInstance->getTeamMessage().data) {
    const TeamMessageData& messageData = it.second;

    // "alive" means sent something in the last n seconds
    if(theInstance->getFrameInfo().getTimeSince(messageData.frameInfo.getTime()) < theInstance->parameters.maximumFreshTime)
    {
      counter++;
    }
  }//end for

  return (double) counter;
}//end getTeamMembersAliveCount

bool TeamSymbols::getWasStriker()
{
  return theInstance->getPlayerInfo().isPlayingStriker;
}

void TeamSymbols::setWasStriker(bool striker)
{
  theInstance->getPlayerInfo().isPlayingStriker = striker;

  // priorize the decision to become a striker by the goalie
  if (striker && theInstance->getPlayerInfo().playerNumber == 1) {
    theInstance->getRoleDecisionModel().wantsToBeStriker = true;
  }
}

bool TeamSymbols::calculateIfStriker()
{
  return theInstance->getRoleDecisionModel().firstStriker == theInstance->getPlayerInfo().playerNumber;
}

bool TeamSymbols::calculateIfSecondStriker()
{
  return theInstance->getRoleDecisionModel().secondStriker == theInstance->getPlayerInfo().playerNumber;
}

TeamSymbols::~TeamSymbols()
{
}

/** the robot which is closest to own goal is defined as the last one */
bool TeamSymbols::calculateIfTheLast()
{
  // initialize with own values
  double shortestDistance = (theInstance->getRobotPose().translation - theInstance->getFieldInfo().ownGoalCenter).abs();

  double secondShortestDistance = std::numeric_limits<double>::max();

  unsigned int playerNearestToOwnGoal = theInstance->getPlayerInfo().playerNumber;
  unsigned int playerAlmostNearestToOwnGoal = std::numeric_limits<unsigned int>::max();


  // check all non-penalized and non-striker team members
  for(auto const &it : theInstance->getTeamMessage().data) {
    const TeamMessageData& messageData = it.second;
    const int number = it.first;

    if ((theInstance->getFrameInfo().getTimeSince(messageData.frameInfo.getTime())
         < theInstance->parameters.maximumFreshTime) && // alive?
        !messageData.custom.isPenalized && // not penalized?
        !messageData.custom.wantsToBeStriker &&
        number != 1 && // no goalie
        // we are already considered by the initial values
        messageData.playerNumber != theInstance->getPlayerInfo().playerNumber
        )
    {
      Vector2d robotpos = messageData.pose.translation;
      double d = (robotpos-theInstance->getFieldInfo().ownGoalCenter).abs();
      if ( d < shortestDistance )
      {
       // std::cout << "(old) secDist=" << secondShortestDistance << " secNum="
       //           << playerAlmostNearestToOwnGoal << " firstDist=" << shortestDistance
       //           << " firstNum=" << playerNearestToOwnGoal << std::endl;

        // exchange the second shortest distance
        secondShortestDistance = shortestDistance;
        playerAlmostNearestToOwnGoal = playerNearestToOwnGoal;

        //std::cout << "(after exchange) secDist=" << secondShortestDistance << " secNum="
        //          << playerAlmostNearestToOwnGoal << " firstDist=" << shortestDistance
        //          << " firstNum=" << playerNearestToOwnGoal << std::endl;

        // set new nearest
        shortestDistance = d;
        playerNearestToOwnGoal = number;

        //std::cout << "(new) secDist=" << secondShortestDistance << " secNum="
        //          << playerAlmostNearestToOwnGoal << " firstDist=" << shortestDistance
        //          << " firstNum=" << playerNearestToOwnGoal << std::endl;
      }
    }//end if
  }//end for

//  std::cout << "==========" << std::endl;

  if(fabs(secondShortestDistance-shortestDistance) < 500)
  {
    // distance of distance is less than half a meter, choose if we have the
    // lowest player number
    if(playerNearestToOwnGoal == theInstance->getPlayerInfo().playerNumber)
    {
      return playerNearestToOwnGoal < playerAlmostNearestToOwnGoal;
    }
    else if(playerAlmostNearestToOwnGoal == theInstance->getPlayerInfo().playerNumber)
    {
      return playerAlmostNearestToOwnGoal < playerNearestToOwnGoal;
    }
    else
    {
      return false;
    }
  }
  else
  {
    // is it me?
    return playerNearestToOwnGoal == theInstance->getPlayerInfo().playerNumber;
  }
}//end calculateIfTheLast
