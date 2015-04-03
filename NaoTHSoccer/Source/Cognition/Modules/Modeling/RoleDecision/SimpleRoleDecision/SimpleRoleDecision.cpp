/**
* @file SimpleRoleDecision.h
*
* @author <a href="mailto:schahin.tofangchi@hu-berlin.de">Schahin Tofangchi</a>
*/

#include "SimpleRoleDecision.h"
#include <PlatformInterface/Platform.h>
#include  <Tools/DataConversion.h>

#include <math.h>

using namespace std;


SimpleRoleDecision::SimpleRoleDecision()
{
  getDebugParameterList().add(&parameters);
}

SimpleRoleDecision::~SimpleRoleDecision()
{
  getDebugParameterList().remove(&parameters);
}

void SimpleRoleDecision::execute() {

  computeStrikers();

}//end execute

void SimpleRoleDecision::computeStrikers() {

  TeamMessage const& tm = getTeamMessage();

  // initialize with max-values. Every Robot must start with same values!
  double shortestDistance = getFieldInfo().xFieldLength;
  unsigned int playerNearestToBall = 0; //nobody near to ball

  //if someone is striker, leave! Goalie can be striker (while f.e. clearing ball)
  for(std::map<unsigned int, TeamMessage::Data>::const_iterator i=tm.data.begin(); i != tm.data.end(); ++i) {
    const TeamMessage::Data& messageData = i->second;
    const unsigned int number = i->first;

    if((getFrameInfo().getTimeSince(i->second.frameInfo.getTime()) < parameters.maximumFreshTime) && // the message is fresh...
        number != getPlayerInfo().gameData.playerNumber && // its not me...
        messageData.wasStriker // the guy wants to be striker...
        ) {
      getRoleDecisionModel().firstStriker = number; // let him go :)      
      PLOT(std::string("SimpleRoleDecision:StrikerDecision"), 0);
      return;
    }
  }//end for

  // all team members except goalie!! otherwise goalie is nearest and all thinks he is striker, but he won't clear ball
  //should check who has best position to goal etc.
  for(std::map<unsigned int, TeamMessage::Data>::const_iterator i=tm.data.begin(); i != tm.data.end(); ++i) {
    const TeamMessage::Data& messageData = i->second;
    const unsigned int number = i->first;

    double time_bonus = messageData.wasStriker?parameters.strikerBonusTime:0.0; //At this point, the only robot that may still have been a striker is us

    if (!messageData.fallen
      && !messageData.isPenalized
      && number != 1 // goalie is not considered
      && getFrameInfo().getTimeSince(i->second.frameInfo.getTime()) < parameters.maximumFreshTime // its fresh
      && (messageData.ballAge >= 0 && messageData.ballAge < parameters.maxBallLostTime+time_bonus )// the guy sees the ball
      ) {
      Vector2d ballPos = messageData.ballPosition;
      double ballDistance = ballPos.abs();

      // striker bonus
      if (messageData.wasStriker)
        ballDistance -= 100;

      // remember the closest guy
      if(ballDistance < shortestDistance) {
        shortestDistance = ballDistance;
        playerNearestToBall = number;
      }    
    }//end if
  }//end for

  getRoleDecisionModel().firstStriker = playerNearestToBall; 
  getRoleDecisionModel().wantsToBeStriker = playerNearestToBall == playerNearestToBall;         
  PLOT(std::string("SimpleRoleDecision:StrikerDecision"), getRoleDecisionModel().wantsToBeStriker);
}
