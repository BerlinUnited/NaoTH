/**
* @file SimpleRoleDecision.h
*
* @author <a href="mailto:schahin.tofangchi@hu-berlin.de">Schahin Tofangchi</a>
*/

#include "SimpleRoleDecision.h"
#include <PlatformInterface/Platform.h>

using namespace std;


SimpleRoleDecision::SimpleRoleDecision()
{
  getDebugParameterList().add(&params);
}

SimpleRoleDecision::~SimpleRoleDecision()
{
  getDebugParameterList().remove(&params);
}

void SimpleRoleDecision::execute() {
  computeStrikers();
}//end execute

void SimpleRoleDecision::computeStrikers() {
  getRoleDecisionModel().resetStriker();
  TeamMessage const& tm = getTeamMessage();

  // initialize with max-values. Every Robot must start with same values!
  double shortestDistance = getFieldInfo().xFieldLength;
  unsigned int playerNearestToBall = 0; //nobody near to ball

  //if someone is striker, leave! Goalie can be striker (while f.e. clearing ball)
  for(auto const &it : tm.data) {
    const TeamMessageData& messageData = it.second;
    const unsigned int number = it.first;

    if((getFrameInfo().getTimeSince(messageData.frameInfo.getTime()) < params.maximumFreshTime) && // the message is fresh...
        number != getPlayerInfo().playerNumber && // its not me...
        messageData.custom.wasStriker // the guy wants to be striker...
        ) {
      getRoleDecisionModel().firstStriker = number; // let him go :)      
      // set the new striker role
      if(getRoleDecisionModel().roles.find(number) != getRoleDecisionModel().roles.cend()) {
          getRoleDecisionModel().roles[number].dynamic = Roles::striker;
      }
      PLOT(std::string("SimpleRoleDecision:StrikerDecision"), 0);
      return;
    }
  }//end for

  // already have a striker and can return
  if(getRoleDecisionModel().firstStriker < std::numeric_limits<unsigned int>::max()) {
      return;
  }

  // all team members except goalie!! otherwise goalie is nearest and all thinks he is striker, but he won't clear ball
  //should check who has best position to goal etc.
  for(auto const &it : tm.data) {
      const TeamMessageData& messageData = it.second;
      const unsigned int number = it.first;

      double time_bonus = messageData.custom.wasStriker ? params.strikerBonusTime : 0.0; //At this point, the only robot that may still have been a striker is us

    if (!messageData.fallen
      && messageData.custom.robotState == PlayerInfo::playing
      && number != 1 // goalie is not considered
      && getFrameInfo().getTimeSince(messageData.frameInfo.getTime()) < params.maximumFreshTime // its fresh
      && (messageData.ballAge >= 0 && messageData.ballAge < params.maxBallLostTime+time_bonus )// the guy sees the ball
      ) 
    {
      Vector2d ballPos = messageData.ballPosition;
      double ballDistance = ballPos.abs();

      // striker bonus
      if (messageData.custom.wasStriker)
        ballDistance -= 100;

      // remember the closest guy
      if(ballDistance < shortestDistance) {
        shortestDistance = ballDistance;
        playerNearestToBall = number;
      }    
    }//end if
  }//end for

  getRoleDecisionModel().firstStriker = playerNearestToBall; 
  getRoleDecisionModel().wantsToBeStriker = true;
  // set the new striker role
  if(getRoleDecisionModel().roles.find(playerNearestToBall) != getRoleDecisionModel().roles.cend()) {
      getRoleDecisionModel().roles[playerNearestToBall].dynamic = Roles::striker;
  }
  PLOT(std::string("SimpleRoleDecision:StrikerDecision"), getRoleDecisionModel().wantsToBeStriker);
}
