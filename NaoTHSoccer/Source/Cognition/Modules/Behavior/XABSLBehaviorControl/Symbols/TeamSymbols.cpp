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
  engine.registerBooleanInputSymbol("team.calc_if_is_striker_by_time_to_ball", &calculateIfStrikerByTimeToBall);
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

  for(std::map<unsigned int, TeamMessage::Data>::const_iterator i=theInstance->getTeamMessage().data.begin();
    i != theInstance->getTeamMessage().data.end(); ++i)
  {
    const TeamMessage::Data& messageData = i->second;

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
}

bool TeamSymbols::calculateIfStriker()
{
  TeamMessage const& tm = theInstance->getTeamMessage();

  // initialize with max-values. Every Robot must start with same values!
  double shortestDistance = theInstance->getFieldInfo().xFieldLength;
  unsigned int playerNearestToBall = 0; //nobody near to ball

  //if someone is striker, leave! Goalie can be striker (while f.e. clearing ball)
  for(std::map<unsigned int, TeamMessage::Data>::const_iterator i=tm.data.begin();
    i != tm.data.end(); ++i)
  {
    const TeamMessage::Data& messageData = i->second;
    const unsigned int number = i->first;

    if((theInstance->getFrameInfo().getTimeSince(i->second.frameInfo.getTime()) < theInstance->parameters.maximumFreshTime) && // the message is fresh...
        number != theInstance->getPlayerInfo().gameData.playerNumber && // its not me...
        messageData.wasStriker // the guy wants to be striker...
        )
    {
      return false; // let him go :)
    }
  }//end for

  // all team members except goalie!! otherwise goalie is nearest and all thinks he is striker, but he won't clear ball
  //should check who has best position to goal etc.
  for(std::map<unsigned int, TeamMessage::Data>::const_iterator i=tm.data.begin();
    i != tm.data.end(); ++i)
  {
    const TeamMessage::Data& messageData = i->second;
    const unsigned int number = i->first;

    double time_bonus = messageData.wasStriker?theInstance->parameters.strikerBonusTime:0.0;

    if(
        !messageData.fallen
        && !messageData.isPenalized
        && number != 1 // goalie is not considered
        && theInstance->getFrameInfo().getTimeSince(i->second.frameInfo.getTime()) < theInstance->parameters.maximumFreshTime // its fresh
        && (messageData.ballAge >= 0 && messageData.ballAge < theInstance->parameters.maxBallLostTime+time_bonus )// the guy sees the ball
      )
    {
      Vector2d ballPos = messageData.ballPosition;
      double ballDistance = ballPos.abs();

      // striker bonus
      if (messageData.wasStriker)
        ballDistance -= 100;

      // remember the closest guy
      if(ballDistance < shortestDistance)
      {
        shortestDistance = ballDistance;
        playerNearestToBall = number;
      }    
    }//end if
  }//end for

  // am I the closest one?
  return playerNearestToBall == theInstance->getPlayerInfo().gameData.playerNumber;
}//end calculateIfStriker

bool TeamSymbols::calculateIfStrikerByTimeToBall()
{
  TeamMessage const& tm = theInstance->getTeamMessage();

  double shortestTime = theInstance->getSoccerStrategy().timeToBall;
  if ( theInstance->getPlayerInfo().isPlayingStriker ) shortestTime-=100;

  for(std::map<unsigned int, TeamMessage::Data>::const_iterator i=tm.data.begin();
      i != tm.data.end(); ++i)
  {
    const TeamMessage::Data& msg = i->second;
    if (
      i->first != theInstance->getPlayerInfo().gameData.playerNumber
      && msg.wasStriker
      && !msg.isPenalized
      && msg.ballAge >= 0
      && msg.ballAge + theInstance->getFrameInfo().getTimeSince(i->second.frameInfo.getTime())
        < theInstance->parameters.maximumFreshTime
      )
      {
        if(msg.timeToBall < shortestTime)
        {
          return false;
        }
      }
  }//end for

  return true;

}//end calculateIfStriker

TeamSymbols::~TeamSymbols()
{
}

/** the robot which is closest to own goal is defined as the last one */
bool TeamSymbols::calculateIfTheLast()
{
  TeamMessage const& tm = theInstance->getTeamMessage();

  // initialize with own values
  double shortestDistance = (theInstance->getRobotPose().translation - theInstance->getFieldInfo().ownGoalCenter).abs();

  double secondShortestDistance = std::numeric_limits<double>::max();

  unsigned int playerNearestToOwnGoal = theInstance->getPlayerInfo().gameData.playerNumber;
  unsigned int playerAlmostNearestToOwnGoal = std::numeric_limits<unsigned int>::max();


  // check all non-penalized and non-striker team members
  for(std::map<unsigned int, TeamMessage::Data>::const_iterator i=tm.data.begin();
    i != tm.data.end(); ++i)
  {
    const TeamMessage::Data& messageData = i->second;
    const int number = i->first;

    if ((theInstance->getFrameInfo().getTimeSince(messageData.frameInfo.getTime())
         < theInstance->parameters.maximumFreshTime) && // alive?
        !messageData.isPenalized && // not penalized?
        !messageData.wasStriker &&
        number != 1 && // no goalie
        // we are already considered by the initial values
        messageData.playerNum != theInstance->getPlayerInfo().gameData.playerNumber
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
    if(playerNearestToOwnGoal == theInstance->getPlayerInfo().gameData.playerNumber)
    {
      return playerNearestToOwnGoal < playerAlmostNearestToOwnGoal;
    }
    else if(playerAlmostNearestToOwnGoal == theInstance->getPlayerInfo().gameData.playerNumber)
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
    return playerNearestToOwnGoal == theInstance->getPlayerInfo().gameData.playerNumber;
  }
}//end calculateIfTheLast
