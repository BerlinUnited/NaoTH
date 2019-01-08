#include "RoleDecision.h"

void RoleDecision::execute()
{
    // skip "first frame"! (until i add myself to teamcomm)
    if(getTeamMessage().data.find(getPlayerInfo().playerNumber) == getTeamMessage().data.end()) { return; }

    if(!(getPlayerInfo().robotState == PlayerInfo::playing || getGameData().gameState == GameData::playing)) {
        // we're not in playing - use predefined roles
        // TODO/NOTE: should we use the predefined roles only in the beginning of a half???
        //            then we could "optimize" the wallking back of an goal ...

        for (auto const& i : getTeamMessage().data) {
            // set the default positions of the predefined role
            getRoleDecisionModel().roles[i.first] = params.default_roles[params.assignment_role[i.first]];
            getRoleDecisionModel().roles[i.first].dynamic = RoleDecisionModel::none;
        }
    } else {
        // determine the best role for each player in the team context

        // - more roles than players
        // - same number of roles and players
        // - more players than roles?!

        // - prioritize roles
        // - optimize role selection
        //   - simple: the robot closest to the role's position, takes the role => not optimal!
        //   - find an optimal solution?
        simpleAssignment();
    }
}

void RoleDecision::simpleAssignment()
{
    // determine the distance between each robot/role and assign the role nearest to the robot
    for (auto const& i : getTeamMessage().data) {
        unsigned int robotNumber = i.first;
        const TeamMessageData& msg = i.second;
        // TODO: slowly changing?
    }
}
