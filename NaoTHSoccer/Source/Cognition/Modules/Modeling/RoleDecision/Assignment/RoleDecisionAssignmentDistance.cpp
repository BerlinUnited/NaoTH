#include "RoleDecisionAssignmentDistance.h"
#include "Eigen/Eigen"
#include <limits>
#include "Cognition/Modules/Modeling/RoleDecision/HungarianAlgorithm.h"

void RoleDecisionAssignmentDistance::execute()
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
        // determine new static role
        //withPriority(new_roles);
        withDistance(new_roles);

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

void RoleDecisionAssignmentDistance::roleChange(unsigned int playernumber, RM::StaticRole role) {
    // set the new role for the player
    getRoleDecisionModel().roles[playernumber].role = role;
    // reset (internal) change counter
    role_changes[playernumber].first = 0;
    role_changes[playernumber].second = role;
}

void RoleDecisionAssignmentDistance::keepGoalie(std::map<unsigned int, RM::StaticRole>& new_roles) {
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

void RoleDecisionAssignmentDistance::withPriority(std::map<unsigned int, RM::StaticRole>& new_roles) {
    // determine the distance between each robot/role and assign the role nearest to the robot
    for (const auto& r : params.active_roles) {
        std::pair<unsigned int, double> smallest(0, std::numeric_limits<double>::max());
        for (const auto& i : getTeamMessage().data) {
            // only active players and players without an role yet, are used for this role assignment
            if(getTeamMessagePlayersState().isActive(i.first) && new_roles.find(i.first) == new_roles.cend()) {
                double distance = (getRoleDecisionModel().roles_position[r].home - i.second.pose.translation).abs2();
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

void RoleDecisionAssignmentDistance::withDistance(std::map<unsigned int, RM::StaticRole>& new_roles)
{
    // retrieve assignable roles
    std::vector<RM::StaticRole> assignable_roles(params.active_roles);
    // retrieve players, which doesn't already have a role and remove already assigned roles from the assignable vector
    std::vector<unsigned int> assignable_player;
    std::for_each(getTeamMessage().data.cbegin(), getTeamMessage().data.cend(), [&](const std::pair<unsigned int, TeamMessageData>& v)->void{
        auto it = new_roles.find(v.first);
        if(it == new_roles.cend()) { assignable_player.push_back(v.first); }
        else { assignable_roles.erase(std::remove(assignable_roles.begin(), assignable_roles.end(), it->second)); }
    });
    // save the orginal dimensions
    size_t rows = assignable_player.size(),
           cols = assignable_roles.size(),
           n = std::max(rows, cols);
    // create a squared matrix with default values
    Eigen::Array<int, Eigen::Dynamic, Eigen::Dynamic> m = Eigen::Array<int, Eigen::Dynamic, Eigen::Dynamic>::Constant(n,n,0);
    // prepare distance matrix
    for (size_t r = 0; r < rows; ++r) {
        for (size_t c = 0; c < cols; ++c) {
            auto role = assignable_roles[c];
            auto role_pos = getRoleDecisionModel().getStaticRolePosition(role).home;
            auto player = assignable_player[r];
            auto player_pos = getTeamMessage().data.at(player).pose.translation;
            m(r,c) = static_cast<int>((role_pos - player_pos).abs2());
        }
    }

    HungarianAlgorithm<int>::solve(m);

    std::map<unsigned int, std::vector<RM::StaticRole>> double_assignment;
    for (size_t r = 0; r < rows; ++r) {
        std::vector<RM::StaticRole> assignments;
        for (size_t c = 0; c < cols; ++c) {
            if(m(r,c) == 0.0) {
                assignments.push_back(assignable_roles[c]);
            }
        }
        if(assignments.size() == 0) {
            // ???
            std::cout<<"NO ASSIGNMENT "<<assignable_player[r]<<"?"<<std::endl;
            std::cout<<"\n"<<m<<std::endl;
        } else if (assignments.size() == 1) {
            new_roles[assignable_player[r]] = assignments[0];
        } else {
            //double_assignment[assignable_player[r]] = assignments;
            std::cout<<"DOUBLE ASSIGNMENT "<<assignable_player[r]<<"?"<<std::endl;
            for(auto it : assignments) { std::cout<<RM::getName(it)<<", "; }
            std::cout<<"\n"<<m<<std::endl;
        }
    }
}

void RoleDecisionAssignmentDistance::roleChangeByCycle(std::map<unsigned int, RM::StaticRole>& new_roles)
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

void RoleDecisionAssignmentDistance::roleChangeByTime(std::map<unsigned int, RM::StaticRole>& new_roles)
{
    for(const auto& a : new_roles) {
        if(getRoleDecisionModel().roles[a.first].role == a.second) {
            // TODO: update timestamp
        } else {
            // TODO: check if we wanted to change the role to the same role for the last x seconds
        }
    }
}
