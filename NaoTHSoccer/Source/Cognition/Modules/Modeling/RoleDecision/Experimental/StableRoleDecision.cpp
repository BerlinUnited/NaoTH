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

}

void StableRoleDecision::execute() {

  computeStrikers();

}//end execute

void StableRoleDecision::computeStrikers() {

  TeamMessage const& tm = getTeamMessage();

  double shortestTime = getSoccerStrategy().timeToBall;
  if (getPlayerInfo().isPlayingStriker)
    shortestTime-=100;

  for (std::map<unsigned int, TeamMessage::Data>::const_iterator i=tm.data.begin(); i != tm.data.end(); ++i) {
    const TeamMessage::Data& msg = i->second;
    unsigned int robotNumber = i->first;
    double failureProbability = 0.0;
    std::map<unsigned int, double>::const_iterator robotFailure = getTeamMessageStatisticsModel().failureProbabilities.find(robotNumber);
    if (robotFailure != getTeamMessageStatisticsModel().failureProbabilities.end()) { 
      failureProbability = robotFailure->second;
    }
    if (robotNumber != getPlayerInfo().gameData.playerNumber
      && msg.wasStriker //Robot considers itself the striker
      && !msg.isPenalized
      && failureProbability < parameters.minFailureProbability //Message is fresh
      && msg.ballAge >= 0 //Ball has been seen
      && msg.ballAge + getFrameInfo().getTimeSince(i->second.frameInfo.getTime()) < parameters.maxBallLostTime) { //Ball is fresh
        if(msg.timeToBall < shortestTime) {
          getRoleDecisionModel().firstStriker = robotNumber;
          PLOT(std::string("StableRoleDecision:StrikerDecision"), 0);
          return;
        }
      }
  }//end for

  getRoleDecisionModel().firstStriker = getPlayerInfo().gameData.playerNumber;
  PLOT(std::string("StableRoleDecision:StrikerDecision"), 1);

}
