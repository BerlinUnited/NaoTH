#include "RoleDecision.h"

void RoleDecision::execute()
{
    // skip "first frame"! (until i add myself to teamcomm)
    if(getTeamMessage().data.find(getPlayerInfo().playerNumber) == getTeamMessage().data.end()) { return; }

    if(!(getPlayerInfo().robotState == PlayerInfo::playing || getGameData().gameState == GameData::playing)) {
        // we're not in playing - use predefined roles
        // TODO/NOTE: should we use the predefined roles only in the beginning of a half???
        //            then we could "optimize" the wallking back of an goal ...

        for (const auto& i : getTeamMessage().data) {
            // set the default positions of the predefined role
            roleChange(i.first, params.assignment_role[i.first]);
        }
    } else {
        // determine the best role for each player in the team context
        std::map<unsigned int, RM::StaticRole> new_roles;

        // - more roles than players
        // - same number of roles and players
        // - more players than roles?!

        // - prioritize roles
        // - optimize role selection
        //   - simple: the robot closest to the role's position, takes the role => not optimal!
        //   - find an optimal solution?

        // the goalie is always goalie! (should never change)
        keepGoalie(new_roles);

        roleAssignmentBySmallestDistance(new_roles);

        // based on smallest distance
        // based on smallest summed distance of the whole team
        // based on the smallest time

        // apply new role for each player, if it was the same role for at least x times or for at least y seconds
        // TODO: ..
        roleChangeByCycle(new_roles);
        // TODO: players, which doesn't get a (new) role, should the 'unknown' role assigned
        for (const auto& i : getTeamMessage().data) {
            if(new_roles.find(i.first) == new_roles.cend()) {
                roleChange(i.first, RM::unknown);
            }
        }
    }
}

void RoleDecision::roleAssignmentBySmallestDistance(std::map<unsigned int, RM::StaticRole>& new_roles)
{
    // determine the distance between each robot/role and assign the role nearest to the robot
    for (const auto& r : params.priority_role) {
        std::pair<unsigned int, double> smallest(0, std::numeric_limits<double>::max());
        for (const auto& i : getTeamMessage().data) {
            // only active players and players without an role yet, are used for this role assignment
            if(getTeamMessagePlayersState().isActive(i.first) && new_roles.find(i.first) == new_roles.cend()) {
                double distance = (params.default_roles[r].home - i.second.pose.translation).abs2();
                if(distance < smallest.second) {
                    smallest.first = i.first;
                    smallest.second = distance;
                }
            }
        }
        // there's no player left, who could take a role
        if(smallest.first == 0) {
            break;
        } else {
            new_roles[smallest.first] = r;
        }
    }
}

void RoleDecision::roleAssignmentBySmallestTeamDistance(std::map<unsigned int, RM::StaticRole>& new_roles) {
    // TODO: the sum of distances the whole team has to move must be minizied
    unsigned int i = 1;
    for (const auto& r : params.priority_role) {
        new_roles[i] = r;
    }
}

void RoleDecision::roleChange(unsigned int playernumber, RM::StaticRole role) {
    // set the new role for the player
    getRoleDecisionModel().roles[playernumber] = params.default_roles[role];
    getRoleDecisionModel().roles[playernumber].dynamic = RM::none;
    // reset change counter
    role_changes[playernumber].first = 0;
    role_changes[playernumber].second = role;
}

void RoleDecision::keepGoalie(std::map<unsigned int, RM::StaticRole>& new_roles) {
    // find the player with the goalie role
    const auto& goalie = std::find_if(
                getRoleDecisionModel().roles.cbegin(),
                getRoleDecisionModel().roles.cend(),
                [](const std::pair<const unsigned int,RM::Role>& r) {return r.second.role == RM::goalie; });
    // set the new role to goale (again)
    if(goalie != getRoleDecisionModel().roles.end()) {
        new_roles[goalie->first] = RM::goalie;
    }
}

void RoleDecision::roleChangeByCycle(std::map<unsigned int, RM::StaticRole>& new_roles)
{
    for(const auto& n : new_roles) {
        if(getRoleDecisionModel().roles[n.first].role == n.second) {
            // no change, reset change counter
            role_changes[n.first].first = 0;
        } else {
            // role should change
            if(role_changes[n.first].second == n.second) {
                if(role_changes[n.first].first >= params.minChangingCycles) {
                    // waited long enough and still want to change the role - do it!
                    roleChange(n.first, n.second);
                } else {
                    // increase change counter
                    role_changes[n.first].first++;
                }
            } else {
                // the role decision is different from the previous decision - reset change counter
                role_changes[n.first].first = 0;
                role_changes[n.first].second = n.second;
            }
        }
    }
}

void RoleDecision::roleChangeByTime(std::map<unsigned int, RM::StaticRole>& new_roles)
{
    for(const auto& a : new_roles) {
        if(getRoleDecisionModel().roles[a.first].role == a.second) {
            // TODO: update timestamp
        } else {
            // TODO: check if we wanted to change the role to the same role for the last x seconds
        }
    }
    //
}
