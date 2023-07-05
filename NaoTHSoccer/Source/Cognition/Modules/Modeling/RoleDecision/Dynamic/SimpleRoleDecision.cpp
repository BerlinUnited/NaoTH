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

  // initialize with max-values. Every Robot must start with same values!
  double shortestDistance = getFieldInfo().xFieldLength;
  unsigned int playerNearestToBall = 0; //nobody near to ball

  //if someone is striker, leave! Goalie can be striker (while f.e. clearing ball)
  for (const auto& it: getTeamState().players) {
    const auto& player = it.second;
    const unsigned int number = it.first;

    if((getFrameInfo().getTimeSince(player.messageFrameInfo.getTime()) < params.maximumFreshTime) && // the message is fresh...
        number != getPlayerInfo().playerNumber && // its not me...
        player.wasStriker() // the guy wants to be striker...
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
  for (const auto& it: getTeamState().players) {
      const auto& player = it.second;
      const unsigned int number = it.first;

      double time_bonus = player.wasStriker() ? params.strikerBonusTime : 0.0; //At this point, the only robot that may still have been a striker is us

    if (!player.fallen()
      && player.state() == PlayerInfo::playing
      && number != 1 // goalie is not considered
      && getFrameInfo().getTimeSince(player.messageFrameInfo.getTime()) < params.maximumFreshTime // its fresh
      && (player.ballAge() >= 0 && player.ballAge() < params.maxBallLostTime+time_bonus )// the guy sees the ball
      ) 
    {
      Vector2d ballPos = player.ballPosition();
      double ballDistance = ballPos.abs();

      // striker bonus
      if (player.wasStriker())
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
