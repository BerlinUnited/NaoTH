/**
 * @file: TeamSymbols.cpp
 * @author: <a href="mailto:scheunem@informatik.hu-berlin.de">Marcus Scheunemann</a>
 *
 * First created on 9. April 2009, 18:10
 */

#include "TeamSymbols.h"
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Modeling/GoalModel.h"
#include "Representations/Modeling/BodyState.h"
#include "Representations/Motion/MotionStatus.h"
#include "Representations/Motion/Request/MotionRequest.h"
#include "Tools/Debug/DebugModify.h"

void TeamSymbols::registerSymbols(xabsl::Engine& engine)
{
  engine.registerDecimalInputSymbol("team.members_alive_count", &getTeamMembersAliveCount);
  engine.registerBooleanInputSymbol("team.calc_if_is_striker", &calculateIfStriker);
  engine.registerBooleanInputSymbol("team.calc_if_is_striker_by_time_to_ball", &calculateIfStrikerByTimeToBall);
  engine.registerBooleanOutputSymbol("team.is_playing_as_striker",&setWasStriker, &getWasStriker);
  engine.registerBooleanInputSymbol("team.calc_if_is_the_last", &calculateIfTheLast);
}//end registerSymbols


TeamSymbols* TeamSymbols::theInstance = NULL;

void TeamSymbols::execute()
{
}

double TeamSymbols::getTeamMembersAliveCount()
{
  int counter = 0;

  for(std::map<unsigned int, TeamMessage::Data>::const_iterator i=theInstance->teamMessage.data.begin();
    i != theInstance->teamMessage.data.end(); ++i)
  {
    const TeamMessage::Data& messageData = i->second;

    // "alive" means sent something in the last n seconds
    if(theInstance->frameInfo.getTimeSince(messageData.frameInfo.getTime()) < theInstance->maximumFreshTime)
    {
      counter++;
    }
  }//end for

  return (double) counter;
}//end getTeamMembersAliveCount

bool TeamSymbols::getWasStriker()
{
  return theInstance->playerInfo.isPlayingStriker;
}//end getWasStriker

void TeamSymbols::setWasStriker(bool striker)
{
  theInstance->playerInfo.isPlayingStriker = striker;
}//end setWasStriker

bool TeamSymbols::calculateIfStriker()
{
  TeamMessage const& tm = theInstance->teamMessage;

  // initialize with max-values. Every Robot must start with same values!
  double shortestDistance = theInstance->fieldInfo.xFieldLength;
  unsigned int playerNearestToBall = 0; //nobody near to ball

  //if someone is striker, leave! Goalie can be striker (while f.e. clearing ball)
  for(std::map<unsigned int, TeamMessage::Data>::const_iterator i=tm.data.begin();
    i != tm.data.end(); ++i)
  {
    const TeamMessage::Data& messageData = i->second;
    const unsigned int number = i->first;

    if((theInstance->frameInfo.getTimeSince(i->second.frameInfo.getTime()) < theInstance->maximumFreshTime) && // the message is fresh...
        number != theInstance->playerInfo.gameData.playerNumber && // its not me...
        messageData.message.wasstriker() // the guy wants to be striker...
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

    if(number == 1) continue; // goalie is not considered
  
    double time_bonus = messageData.message.wasstriker()?4000.0:0.0;

    if(
        theInstance->frameInfo.getTimeSince(i->second.frameInfo.getTime()) < theInstance->maximumFreshTime // its fresh
        && !messageData.message.isfallendown()
        && (messageData.message.timesinceballwasseen() < 1000+time_bonus )// the guy sees the ball
      )
    {
      Vector2<double> ballPos;
      DataConversion::fromMessage(messageData.message.ballposition(), ballPos);
      double ballDistance = ballPos.abs();

      // striker bonus
      if (messageData.message.wasstriker())
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
  return playerNearestToBall == theInstance->playerInfo.gameData.playerNumber;
}//end calculateIfStriker

bool TeamSymbols::calculateIfStrikerByTimeToBall()
{
  TeamMessage const& tm = theInstance->teamMessage;

  double shortestTime = theInstance->soccerStrategy.timeToBall;
  if ( theInstance->playerInfo.isPlayingStriker ) shortestTime-=100;

  for(std::map<unsigned int, TeamMessage::Data>::const_iterator i=tm.data.begin();
      i != tm.data.end(); ++i)
  {
    const naothmessages::TeamCommMessage& msg = i->second.message;
    if (
      i->first != theInstance->playerInfo.gameData.playerNumber
      && msg.wasstriker()
      && msg.timesinceballwasseen() + theInstance->frameInfo.getTimeSince(i->second.frameInfo.getTime())
        < theInstance->maximumFreshTime
      )
      {
        if(msg.timetoball() < shortestTime)
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
  TeamMessage const& tm = theInstance->teamMessage;

  // initialize with own values
  double shortestDistance = (theInstance->robotPose.translation - theInstance->fieldInfo.ownGoalCenter).abs();

  double secondShortestDistance = std::numeric_limits<double>::max();

  unsigned int playerNearestToOwnGoal = theInstance->playerInfo.gameData.playerNumber;
  unsigned int playerAlmostNearestToOwnGoal = std::numeric_limits<unsigned int>::max();


  // check all non-penalized and non-striker team members
  for(std::map<unsigned int, TeamMessage::Data>::const_iterator i=tm.data.begin();
    i != tm.data.end(); ++i)
  {
    const TeamMessage::Data& messageData = i->second;
    const int number = i->first;

    if ((theInstance->frameInfo.getTimeSince(messageData.frameInfo.getTime())
         < theInstance->maximumFreshTime) && // alive?
        !messageData.message.ispenalized() && // not penalized?
        !messageData.message.wasstriker() &&
        number != 1 && // no goalie
        // we are already considered by the initial values
        messageData.message.playernumber() != theInstance->playerInfo.gameData.playerNumber
        )
    {
      Vector2<double> robotpos;
      robotpos.x = messageData.message.positiononfield().translation().x();
      robotpos.y = messageData.message.positiononfield().translation().y();
      double d = (robotpos-theInstance->fieldInfo.ownGoalCenter).abs();
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

  if(abs(secondShortestDistance-shortestDistance) < 500)
  {
    // distance of distance is less than half a meter, choose if we have the
    // lowest player number
    if(playerNearestToOwnGoal == theInstance->playerInfo.gameData.playerNumber)
    {
      return playerNearestToOwnGoal < playerAlmostNearestToOwnGoal;
    }
    else if(playerAlmostNearestToOwnGoal == theInstance->playerInfo.gameData.playerNumber)
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
    return playerNearestToOwnGoal == theInstance->playerInfo.gameData.playerNumber;
  }
}//end calculateIfTheLast
