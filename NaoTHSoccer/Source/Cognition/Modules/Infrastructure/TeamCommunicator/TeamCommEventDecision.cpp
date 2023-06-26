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
    byInterval();
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
