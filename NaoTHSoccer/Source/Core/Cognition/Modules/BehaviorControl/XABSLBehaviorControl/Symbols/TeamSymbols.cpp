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
  unsigned int fn = theInstance->frameInfo.getFrameNumber();

  for(std::map<unsigned int, TeamMessage::Data>::const_iterator i=theInstance->teamMessage.data.begin();
    i != theInstance->teamMessage.data.end(); ++i)
  {
    const TeamMessage::Data& messageData = i->second;

    // "alive" means sent something in the last 250 frames
    if((fn - messageData.frameInfo.getTime()) < 250)
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

  unsigned int fn = theInstance->frameInfo.getFrameNumber();
  TeamMessage const& tm = theInstance->teamMessage;

  // initialize with max-values. Every Robot must start with same values!
  double shortestDistance = theInstance->fieldInfo.xFieldLength;
  unsigned int playerNearestToBall = 1; //nobody near to ball, goali doesn't look at team.calc_if_striker

  //if someone is striker, leave! Goali can be striker (while f.e. clearing ball)
  for(std::map<unsigned int, TeamMessage::Data>::const_iterator i=tm.data.begin();
    i != tm.data.end(); ++i)
  {
    const TeamMessage::Data& messageData = i->second;
    const unsigned int number = i->first;

    // TODO: 100 frames or 100 ms?!
    if((fn - messageData.frameInfo.getFrameNumber()) < 100 && // the message is fresh...
        number != theInstance->playerInfo.gameData.playerNumber && // its not me...
        messageData.message.wasstriker() // the guy wants to be striker...
        )
    {
      return false; // let him go :)
    }
  }//end for


  

  // all team members except goali!! otherwise goali is nearest and all tinks he is striker, but he won't clear ball
  //should check who has best position to goal etc.
  double count = 0;
  for(std::map<unsigned int, TeamMessage::Data>::const_iterator i=tm.data.begin();
    i != tm.data.end(); ++i)
  {
    const TeamMessage::Data& messageData = i->second;
    const unsigned int number = i->first;

    if(number == 1) continue; // goalie is not considered
  
    Vector2<double> ballPos;
    if(
        messageData.frameInfo.getFrameNumber() != 0 && // for logsimulator?
        (fn - messageData.frameInfo.getFrameNumber()) < 100 && // its fresh
        (messageData.message.timesinceballwasseen() < 1000 || // the guy sees the ball 
         number == theInstance->playerInfo.gameData.playerNumber) // its me
      )
    { 
      count++;
      ballPos.x = messageData.message.ballposition().x();
      ballPos.y = messageData.message.ballposition().y();
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


TeamSymbols::~TeamSymbols()
{
}

/** the robot which is closest to own goal is defined as the last one */
bool TeamSymbols::calculateIfTheLast()
{
  unsigned int fn = theInstance->frameInfo.getFrameNumber();

  TeamMessage const& tm = theInstance->teamMessage;

  // initialize with own values
  double shortestDistance = (theInstance->robotPose.translation - theInstance->fieldInfo.ownGoalCenter).abs();
  unsigned int playerNearestToOwnGoal = theInstance->playerInfo.gameData.playerNumber;

  // check all non-penalized and non-striker team members
  for(std::map<unsigned int, TeamMessage::Data>::const_iterator i=tm.data.begin();
    i != tm.data.end(); ++i)
  {
    const TeamMessage::Data& messageData = i->second;
    const int number = i->first;

    if ((fn - messageData.frameInfo.getFrameNumber()) < 100 && // alive?
        !messageData.message.ispenalized() && // not penalized?
        !messageData.message.wasstriker() )
    {
      Vector2<double> robotpos;
      robotpos.x = messageData.message.positiononfield().translation().x();
      robotpos.y = messageData.message.positiononfield().translation().y();
      double d = (robotpos-theInstance->fieldInfo.ownGoalCenter).abs();
      if ( d < shortestDistance )
      {
        shortestDistance = d;
        playerNearestToOwnGoal = number;
      }
    }//end if
  }//end for

  // is it me?
  return playerNearestToOwnGoal == theInstance->playerInfo.gameData.playerNumber;
}//end calculateIfTheLast
