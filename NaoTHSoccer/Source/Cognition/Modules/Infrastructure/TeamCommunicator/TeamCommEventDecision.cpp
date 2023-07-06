#include "TeamCommEventDecision.h"

#include "Representations/Modeling/TeamMessageData.h"
#include <Tools/StringTools.h>
#include "PlatformInterface/Platform.h"

TeamCommEventDecision::TeamCommEventDecision()
{
    getDebugParameterList().add(&params);
}

TeamCommEventDecision::~TeamCommEventDecision()
{
    getDebugParameterList().remove(&params);
}

void TeamCommEventDecision::execute()
{
    getTeamMessageDecision().reset();
    (this->*params.decisionMethod)();
}

void TeamCommEventDecision::byInterval()
{
    if ((unsigned int)getFrameInfo().getTimeSince(params.byInterval_lastSentTimestamp) > params.byInterval_interval)
    {
        getTeamMessageDecision().send_ntpRequests.set();
        getTeamMessageDecision().send_state.set();
        getTeamMessageDecision().send_fallen.set();
        getTeamMessageDecision().send_readyToWalk.set();
        getTeamMessageDecision().send_pose.set();
        getTeamMessageDecision().send_ballAge.set();
        getTeamMessageDecision().send_ballPosition.set();
        getTeamMessageDecision().send_timeToBall.set();
        getTeamMessageDecision().send_wantsToBeStriker.set();
        getTeamMessageDecision().send_wasStriker.set();
        getTeamMessageDecision().send_robotRole.set();

        params.byInterval_lastSentTimestamp = getFrameInfo().getTime();
    }
}

void TeamCommEventDecision::byDistance()
{
    // there is no limit/budget in the initial phase; use this phase for syncing
    if (getGameData().gameState == GameData::initial) {
        getTeamMessageDecision().send_ntpRequests.set();
    }

    // if the robot is not playing (eg. penalized), do not send any message
    if (getPlayerInfo().robotState != PlayerInfo::playing) {
        return;
    }

    // as kind of safety set a lower bound how often we can send messages
    if ((unsigned int)getFrameInfo().getTimeSince(params.byDistance_lastSentTimestamp) < params.byDistance_minInterval) {
        return;
    }

    const auto& role = getRoleDecisionModel().getRole(getPlayerInfo().playerNumber);

    double distance = 1000.0;
    if (Roles::isGoalie(role.role))
    {
        distance = params.byDistance_goalie;
    }
    else if (Roles::isDefender(role.role))
    {
        distance = params.byDistance_defender;
    }
    else if (Roles::isMidfielder(role.role))
    {
        distance = params.byDistance_midfielder;
    }
    else if (Roles::isForward(role.role))
    {
        distance = params.byDistance_forward;
    }

    // if i am striker, i send more often
    if (role.dynamic == Roles::striker) {
        distance = params.byDistance_striker;

        // the striker should send a message regularly, even if he didn't move far enough
        if ((unsigned int)getFrameInfo().getTimeSince(params.byDistance_striker_last_ts) < params.byDistance_striker_minInterval)
        {
            distance = 0.0;
        }
    }

    // send new message only if the robot moved some distance
    if ((getRobotPose() - params.byDistance_lastPose).translation.abs() > distance)
    {
        getTeamMessageDecision().send_pose.set();

        // only send ball model, if it adds value
        if (getBallModel().knows) {
            getTeamMessageDecision().send_ballAge.set();
            getTeamMessageDecision().send_ballPosition.set();
        }

        params.byDistance_lastPose = getRobotPose();

        // update timestamp for the safety condition
        params.byDistance_lastSentTimestamp = getFrameInfo().getTime();
        params.byDistance_striker_last_ts = getFrameInfo().getTime();
    }
}