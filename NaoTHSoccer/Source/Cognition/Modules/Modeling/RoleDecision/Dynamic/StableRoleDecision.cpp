/**
* @file StableRoleDecision.h
*
* @author <a href="mailto:schahin.tofangchi@hu-berlin.de">Schahin Tofangchi</a>
*/

#include "StableRoleDecision.h"

using namespace std;


StableRoleDecision::StableRoleDecision()
{
  getDebugParameterList().add(&params);
}

StableRoleDecision::~StableRoleDecision()
{
  getDebugParameterList().remove(&params);
}

void StableRoleDecision::execute() 
{
  // player numbers for the first and second striker
  unsigned int firstStriker = std::numeric_limits<unsigned int>::max();
  unsigned int secondStriker = std::numeric_limits<unsigned int>::max();

  bool wantsToBeStriker = true;
  //Goalie is not considered
  if (getPlayerInfo().playerNumber == 1) {
    wantsToBeStriker = false; 
  }

  double ownTimeToBall = getSoccerStrategy().timeToBall;
  // bonus for the striker to prevent oscillations
  if (getPlayerInfo().isPlayingStriker) {
    ownTimeToBall -= 300;
  }

  for (auto const& i : getTeamMessage().data)
  {
    unsigned int robotNumber = i.first;
    const TeamMessageData& msg = i.second;

    if (msg.playerNumber != getPlayerInfo().playerNumber && !getTeamMessagePlayersState().isPlaying(robotNumber)) {
      continue;
    }

    double time_bonus = 0;
    if (msg.playerNumber == getRoleDecisionModel().firstStriker) {
      time_bonus = params.strikerBonusTime;
    }

    if (robotNumber == getPlayerInfo().playerNumber && (!getTeamMessagePlayersState().isPlaying(robotNumber) ||
      // ball was seen (> -1) and ball isn't too old
      msg.ballAge < 0 || msg.ballAge > params.maxBallLostTime + time_bonus)) 
    {
      wantsToBeStriker = false;
    }

    if (getTeamMessagePlayersState().isPlaying(robotNumber)
      && msg.ballAge >= 0 // Ball was seen some time ago ...
      && msg.ballAge + getFrameInfo().getTimeSince(msg.frameInfo.getTime()) < params.maxBallLostTime + time_bonus) //Ball is fresh
    { 
      if (msg.custom.wantsToBeStriker) { //Decision of the current round
        // If two robots want to be striker, the one with a smaller number is favoured
        // NOTE: goalie is always favoured for the first striker
        if (robotNumber < firstStriker) {
          firstStriker = robotNumber;
        }
        else if (robotNumber < secondStriker) {
          secondStriker = robotNumber;
        }
      }

      // another player is closer than me
      if (robotNumber != getPlayerInfo().playerNumber && msg.custom.timeToBall < ownTimeToBall) {
        wantsToBeStriker = false; //Preparation for next round's decision
      }
    }
  }//end for
  
  // there is no second striker, if goalie is a striker
  if (firstStriker == 1) {
    secondStriker = std::numeric_limits<unsigned int>::max();
  }

  getRoleDecisionModel().resetStriker();
  getRoleDecisionModel().firstStriker = firstStriker;
  getRoleDecisionModel().secondStriker = secondStriker;
  getRoleDecisionModel().wantsToBeStriker = wantsToBeStriker;
  // set the new striker role
  if(firstStriker < std::numeric_limits<unsigned int>::max() && getRoleDecisionModel().roles.find(firstStriker) != getRoleDecisionModel().roles.cend()) {
      getRoleDecisionModel().roles[firstStriker].dynamic = Roles::striker;
  }
  // set the new striker role
  if(secondStriker < std::numeric_limits<unsigned int>::max() && getRoleDecisionModel().roles.find(secondStriker) != getRoleDecisionModel().roles.cend()) {
      getRoleDecisionModel().roles[secondStriker].dynamic = Roles::striker;
  }

  PLOT(std::string("StableRoleDecision:FirstStrikerDecision"), firstStriker);
  PLOT(std::string("StableRoleDecision:SecondStrikerDecision"), secondStriker);
}
