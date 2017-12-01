/**
* @file StableRoleDecision.h
*
* @author <a href="mailto:schahin.tofangchi@hu-berlin.de">Schahin Tofangchi</a>
*/

#include "StableRoleDecision.h"

using namespace std;


StableRoleDecision::StableRoleDecision()
{
  getDebugParameterList().add(&parameters);
}

StableRoleDecision::~StableRoleDecision()
{
  getDebugParameterList().remove(&parameters);
}

void StableRoleDecision::execute() 
{
  getRoleDecisionModel().aliveRobots.clear();
  getRoleDecisionModel().deadRobots.clear();

  // player numbers for the first and second striker
  int firstStriker = std::numeric_limits<int>::max();
  int secondStriker = std::numeric_limits<int>::max();

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

    double failureProbability = getTeamMessageStatisticsModel().getFailureProbability(robotNumber);

    if (failureProbability > parameters.minFailureProbability && msg.playerNumber != getPlayerInfo().playerNumber) { //Message is not fresh
      getRoleDecisionModel().deadRobots.push_back((int)robotNumber);
      continue;
    } else {
      getRoleDecisionModel().aliveRobots.push_back((int)robotNumber);
    }

    double time_bonus = 0;
    if (msg.playerNumber == getRoleDecisionModel().firstStriker) {
      time_bonus = parameters.strikerBonusTime;
    }

    if (robotNumber == getPlayerInfo().playerNumber && (msg.fallen || msg.custom.isPenalized || 
      // ball was seen (> -1) and ball isn't too old
      msg.ballAge < 0 || msg.ballAge > parameters.maxBallLostTime + time_bonus)) 
    {
      wantsToBeStriker = false;
    }

    if (!msg.fallen
      && !msg.custom.isPenalized
      && msg.ballAge >= 0 // Ball was seen some time ago ...
      && msg.ballAge + getFrameInfo().getTimeSince(msg.frameInfo.getTime()) < parameters.maxBallLostTime + time_bonus) //Ball is fresh
    { 
      if (msg.custom.wantsToBeStriker) { //Decision of the current round
        // If two robots want to be striker, the one with a smaller number is favoured
        // NOTE: goalie is always favoured for the first striker
        if ((int)robotNumber < firstStriker) { 
          firstStriker = robotNumber;
        }
        else if ((int)robotNumber < secondStriker) {
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
    secondStriker = std::numeric_limits<int>::max();
  }

  getRoleDecisionModel().firstStriker = firstStriker;
  getRoleDecisionModel().secondStriker = secondStriker;
  getRoleDecisionModel().wantsToBeStriker = wantsToBeStriker;

  PLOT(std::string("StableRoleDecision:FirstStrikerDecision"), firstStriker);
  PLOT(std::string("StableRoleDecision:SecondStrikerDecision"), secondStriker);
}
