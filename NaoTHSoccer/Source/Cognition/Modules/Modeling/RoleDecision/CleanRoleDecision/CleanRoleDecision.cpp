/**
* @file CleanRoleDecision.h
*
* @author <a href="mailto:schahin.tofangchi@hu-berlin.de">Schahin Tofangchi</a>
*/

#include "CleanRoleDecision.h"
#include <PlatformInterface/Platform.h>
#include <Tools/DataConversion.h>

#include <math.h>
#include <list>

using namespace std;


CleanRoleDecision::CleanRoleDecision()
{
  getDebugParameterList().add(&parameters);
}

CleanRoleDecision::~CleanRoleDecision()
{
  getDebugParameterList().remove(&parameters);
}

void CleanRoleDecision::execute() {
    getRoleDecisionModel().resetRobotStates();
    computeStrikers();

}//end execute

void CleanRoleDecision::computeStrikers()
{
    // skip "first frame"! (until i add myself to teamcomm)
    if(getTeamMessage().data.find(getPlayerInfo().playerNumber) == getTeamMessage().data.end()) { return; }

    // container storing robots, which want to be striker, sorted by their playernumber and their time to ball
    std::map<unsigned int, unsigned int> possible_striker;

    // iterate over all robots(messages)
    for (auto const& i : getTeamMessage().data) {
        unsigned int robotNumber = i.first;
        const TeamMessageData& msg = i.second;

        // if striker lost the ball, he gets a time bonus before he lost the ball completely ...
        double loose_ball_bonus = msg.playerNumber == getRoleDecisionModel().firstStriker?parameters.strikerBonusTime:0.0;

        // check if the robot is able to play and sees the ball
        bool isRobotInactive = msg.fallen
                || msg.custom.isPenalized
                || msg.ballAge < 0 //Ball was never seen
                || (msg.ballAge + getFrameInfo().getTimeSince(msg.frameInfo.getTime()) > parameters.maxBallLostTime + loose_ball_bonus); //Ball isn't fresh

        // ignore "DEAD" and inactive robots
        if(isRobotDead(robotNumber) || isRobotInactive) { continue; }

        // for all active robots, which sees the ball AND previously announced to want to be striker OR is striker ...
        if (msg.custom.wantsToBeStriker || msg.custom.wasStriker) {
            // ... remember them as possible striker with their time to ball
            possible_striker[robotNumber] = msg.custom.timeToBall;
        }
    }//end for

    // get my own message
    auto ownMessage = getTeamMessage().data.at(getPlayerInfo().playerNumber);
    // i want to be striker, if i'm not the goalie and i'm "active" (not fallen/panelized, see the ball)!!!
    getRoleDecisionModel().wantsToBeStriker = ownMessage.playerNumber != 1
                                              && !(ownMessage.fallen || ownMessage.custom.isPenalized || ownMessage.ballAge < 0
                                              || (ownMessage.ballAge + getFrameInfo().getTimeSince(ownMessage.frameInfo.getTime()) > parameters.maxBallLostTime + (getPlayerInfo().playerNumber==getRoleDecisionModel().firstStriker?parameters.strikerBonusTime:0.0)));

    // if i'm striker, i get a time bonus!
    // NOTE: ownTimeToBall can be negative if the robot is close to ball (!)
    double ownTimeToBall = static_cast<double>(ownMessage.custom.timeToBall) - (ownMessage.custom.wasStriker ? 300.0 : 0.0);

    // clear for new striker decision
    getRoleDecisionModel().resetStriker();
    // set the new striker
    for (auto it = possible_striker.cbegin(); it != possible_striker.cend(); ++it) {
        //If two robots want to be striker, the one with a smaller number is favoured => is the first in the map!!
        // NOTE: we're doesn't using 'timeToBall' as decision criteria by intention!
        //       if goalie is striker, he get's the first striker
        if(getRoleDecisionModel().firstStriker == std::numeric_limits<int>::max()) {
            getRoleDecisionModel().firstStriker = it->first;
        } else if (getRoleDecisionModel().secondStriker == std::numeric_limits<int>::max()) {
            getRoleDecisionModel().secondStriker = it->first;
        }
        // if there's a robot closer to the ball than myself, i don't want to be striker!
        if(it->second < ownTimeToBall) {
            getRoleDecisionModel().wantsToBeStriker = false;
        }
    }

    PLOT(std::string("CleanRoleDecision:FirstStrikerDecision"), getRoleDecisionModel().firstStriker);
    PLOT(std::string("CleanRoleDecision:SecondStrikerDecision"), getRoleDecisionModel().secondStriker);
}

bool CleanRoleDecision::isRobotDead(unsigned int robotNumber) {
    double failureProbability = 0.0;
    std::map<unsigned int, double>::const_iterator robotFailure = getTeamMessageStatisticsModel().failureProbabilities.find(robotNumber);
    if (robotFailure != getTeamMessageStatisticsModel().failureProbabilities.end()) {
      failureProbability = robotFailure->second;
    }
    bool isDead = failureProbability > parameters.minFailureProbability && robotNumber != getPlayerInfo().playerNumber;
    // update dead/alive lists
    if (isDead) { //Message is not fresh
      getRoleDecisionModel().deadRobots.push_back(robotNumber);
    } else {
      getRoleDecisionModel().aliveRobots.push_back(robotNumber);
    }
    return isDead;
}
