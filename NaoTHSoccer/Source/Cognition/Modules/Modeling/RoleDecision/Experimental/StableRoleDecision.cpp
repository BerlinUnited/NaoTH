/**
* @file StableRoleDecision.h
*
* @author <a href="mailto:schahin.tofangchi@hu-berlin.de">Schahin Tofangchi</a>
*/

#include "StableRoleDecision.h"
#include <PlatformInterface/Platform.h>
#include  <Tools/DataConversion.h>

#include <math.h>

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

  int firstStriker = std::numeric_limits<int>::max();
  int secondStriker = std::numeric_limits<int>::max();
  bool wantsToBeStriker = true;

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

    if (!msg.isPenalized
      && failureProbability < parameters.minFailureProbability //Message is fresh
      && msg.ballAge >= 0 //Ball has been seen
      && msg.ballAge + getFrameInfo().getTimeSince(msg.frameInfo.getTime()) < parameters.maxBallLostTime) { //Ball is fresh

        if (msg.wasStriker) { //Decision of the current round
          if ((int)robotNumber < firstStriker) { //If two robots want to be first striker, let the smaller robot number decide
            firstStriker = robotNumber;
          }
          else if ((int)robotNumber < secondStriker) {
            secondStriker = robotNumber;
          }
        }
        if (msg.timeToBall < ownTimeToBall) { 
          wantsToBeStriker = false; //Preparation for the next round's decision
        }

      }
  }//end for
  
  getRoleDecisionModel().firstStriker = firstStriker;
  getRoleDecisionModel().secondStriker = secondStriker;
  getRoleDecisionModel().wantsToBeStriker = wantsToBeStriker;

  PLOT(std::string("StableRoleDecision:FirstStrikerDecision"), firstStriker);
  PLOT(std::string("StableRoleDecision:SecondStrikerDecision"), secondStriker);

}
