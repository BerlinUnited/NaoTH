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
  if (getPlayerInfo().gameData.playerNumber == 1) {
    wantsToBeStriker = false; 
  }

  double ownTimeToBall = getSoccerStrategy().timeToBall;
  // bonus for the striker to prevent oscillations
  if (getPlayerInfo().isPlayingStriker) {
    ownTimeToBall -= 300;
  }

  TeamMessage::PlayerMessageMap::const_iterator i=getTeamMessage().data.begin();
  for (; i != getTeamMessage().data.end(); ++i) 
  {
    unsigned int robotNumber = i->first;
    const TeamMessage::Data& msg = i->second;

    double failureProbability = getTeamMessageStatisticsModel().getFailureProbability(robotNumber);

    if (failureProbability > parameters.minFailureProbability && msg.playerNum != getPlayerInfo().gameData.playerNumber) { //Message is not fresh
      getRoleDecisionModel().deadRobots.push_back((int)robotNumber);
      continue;
    } else {
      getRoleDecisionModel().aliveRobots.push_back((int)robotNumber);
    }

    double time_bonus = (int)msg.playerNum==getRoleDecisionModel().firstStriker?parameters.strikerBonusTime:0.0;

    if (robotNumber == getPlayerInfo().gameData.playerNumber && (msg.fallen || msg.isPenalized || 
      msg.ballAge < 0 || msg.ballAge > parameters.maxBallLostTime + time_bonus)) 
    {
      wantsToBeStriker = false;
    }

    if (!msg.fallen
      && !msg.isPenalized
      && msg.ballAge >= 0 //Ball has been seen
      && msg.ballAge + getFrameInfo().getTimeSince(msg.frameInfo.getTime()) < parameters.maxBallLostTime + time_bonus) //Ball is fresh
    { 
      if (msg.wantsToBeStriker) { //Decision of the current round
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
      if ( robotNumber != getPlayerInfo().gameData.playerNumber && msg.timeToBall < ownTimeToBall ) { 
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
