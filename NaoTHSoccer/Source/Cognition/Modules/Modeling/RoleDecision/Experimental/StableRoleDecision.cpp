/**
* @file StableRoleDecision.h
*
* @author <a href="mailto:schahin.tofangchi@hu-berlin.de">Schahin Tofangchi</a>
*/

#include "StableRoleDecision.h"
#include <PlatformInterface/Platform.h>
#include  <Tools/DataConversion.h>

#include <math.h>
#include <list>

using namespace std;


StableRoleDecision::StableRoleDecision()
{
  getDebugParameterList().add(&parameters);
}

StableRoleDecision::~StableRoleDecision()
{
  getDebugParameterList().remove(&parameters);
}

void StableRoleDecision::execute() {

  computeStrikers();

}//end execute

void StableRoleDecision::computeStrikers() {
  
  getRoleDecisionModel().aliveRobots = *(new std::list<int>());
  getRoleDecisionModel().deadRobots = *(new std::list<int>());

  int firstStriker = std::numeric_limits<int>::max();
  int secondStriker = std::numeric_limits<int>::max();
  bool wantsToBeStriker = true;
  if (getPlayerInfo().gameData.playerNumber == 1) {
    wantsToBeStriker = false; //Goalie is not considered
  }

  TeamMessage const& tm = getTeamMessage();

  double ownTimeToBall = getSoccerStrategy().timeToBall;
  if (getPlayerInfo().isPlayingStriker)
    ownTimeToBall -= 300;

  for (std::map<unsigned int, TeamMessage::Data>::const_iterator i=tm.data.begin(); i != tm.data.end(); ++i) {
    unsigned int robotNumber = i->first;
    const TeamMessage::Data& msg = i->second;

    double failureProbability = 0.0;
    std::map<unsigned int, double>::const_iterator robotFailure = getTeamMessageStatisticsModel().failureProbabilities.find(robotNumber);
    if (robotFailure != getTeamMessageStatisticsModel().failureProbabilities.end()) { 
      failureProbability = robotFailure->second;
    }

    if (failureProbability > parameters.minFailureProbability && msg.playerNum != getPlayerInfo().gameData.playerNumber) { //Message is not fresh
      getRoleDecisionModel().deadRobots.push_back((int)robotNumber);
      continue;
    }
    else {
      getRoleDecisionModel().aliveRobots.push_back((int)robotNumber);
    }

    double time_bonus = (int)msg.playerNum==getRoleDecisionModel().firstStriker?parameters.strikerBonusTime:0.0;

    if (robotNumber == getPlayerInfo().gameData.playerNumber && (msg.fallen || msg.isPenalized || 
      msg.ballAge < 0 || msg.ballAge > parameters.maxBallLostTime + time_bonus)) {
        wantsToBeStriker = false;
    }

    if (!msg.fallen
      && !msg.isPenalized
      && msg.ballAge >= 0 //Ball has been seen
      && msg.ballAge + getFrameInfo().getTimeSince(msg.frameInfo.getTime()) < parameters.maxBallLostTime + time_bonus) { //Ball is fresh

        if (msg.wasStriker) { //Decision of the current round
          if ((int)robotNumber < firstStriker) { //If two robots want to be striker, the one with a smaller number is favoured
            firstStriker = robotNumber;
          }
          else if ((int)robotNumber < secondStriker) {
            secondStriker = robotNumber;
          }
        }
        if (robotNumber != getPlayerInfo().gameData.playerNumber && msg.timeToBall < ownTimeToBall) { 
          wantsToBeStriker = false; //Preparation for next round's decision
        }

      }
  }//end for
  
  getRoleDecisionModel().firstStriker = firstStriker;
  getRoleDecisionModel().secondStriker = secondStriker;
  getRoleDecisionModel().wantsToBeStriker = wantsToBeStriker;

  PLOT(std::string("StableRoleDecision:FirstStrikerDecision"), firstStriker);
  PLOT(std::string("StableRoleDecision:SecondStrikerDecision"), secondStriker);

}
