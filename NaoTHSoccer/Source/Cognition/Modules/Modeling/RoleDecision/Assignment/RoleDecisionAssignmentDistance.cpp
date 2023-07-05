#include "RoleDecisionAssignmentDistance.h"
#include "Eigen/Eigen"
#include <limits>
#include "Tools/HungarianAlgorithm.h"

RoleDecisionAssignmentDistance::RoleDecisionAssignmentDistance()
{
    getDebugParameterList().add(&params);

    DEBUG_REQUEST_REGISTER("RoleDecision:Assignment:distance:print_current_decision", "Prints out (std::cout) the role decisions for the current cycle.", false);
}

RoleDecisionAssignmentDistance::~RoleDecisionAssignmentDistance()
{
    getDebugParameterList().remove(&params);
}

void RoleDecisionAssignmentDistance::execute()
{
    // skip "first frame"! (until i add myself to teamcomm)
    if(!getTeamState().hasPlayer(getPlayerInfo().playerNumber)) { return; }

    //
    if(getPlayerInfo().robotState == PlayerInfo::initial)
    {
        // set predefined roles
        for (const auto& i: getTeamState().players) {
            roleChange(i.first, params.assignment_role[i.first]);
        }
    } else {
        // determine the best role for each player in the team context
        std::map<unsigned int, Roles::Static> new_roles;

        // the goalie is always goalie! (should never change)
        keepGoalie(new_roles);

        // determine new static role
        (this->*params.variantFunc)(new_roles);

        // some debug output
        DEBUG_REQUEST("RoleDecision:Assignment:distance:print_current_decision",
            for(const auto& r : new_roles) {
                std::cout << r.first << ":" << Roles::getName(r.second) << ";";
            }
            std::cout << std::endl;
        );

        // apply new role for each player, if it was the same role for at least x times or for at least y seconds
        (this->*params.changingFunc)(new_roles);

        // players, which doesn't got a (new) role, gets the 'unknown' role assigned
        for (const auto& i: getTeamState().players) {
            if(new_roles.find(i.first) == new_roles.cend()) {
                roleChange(i.first, Roles::unknown);
            }
        }
    }
}

void RoleDecisionAssignmentDistance::roleChange(unsigned int playernumber, Roles::Static role) {
    // set the new role for the player
    getRoleDecisionModel().roles[playernumber].role = role;
    // reset (internal) change counter
    role_changes[playernumber].first = 0;
    role_changes[playernumber].second = role;
}

void RoleDecisionAssignmentDistance::keepGoalie(std::map<unsigned int, Roles::Static>& new_roles)
{
    // find the goalie in the teammessages with the lowest playernumber
    unsigned int goalie_msg = std::numeric_limits<unsigned int>::max();
    for (const auto& v: getTeamState().players) {
        if(v.second.robotRole().role == Roles::goalie && goalie_msg > v.first) {
            goalie_msg = v.first;
        }
    }
    // find the player with the goalie role
    const auto& goalie = std::find_if(
                getRoleDecisionModel().roles.cbegin(),
                getRoleDecisionModel().roles.cend(),
                [](const std::pair<const unsigned int,Roles::Role>& r) {return r.second.role == Roles::goalie; });
    // set the new role to goale (again)
    if(goalie != getRoleDecisionModel().roles.end()) {
        // if the communicated goalie is different from my own decision, use the one with the smaller playernumber
        if(goalie_msg < goalie->first) {
            new_roles[goalie_msg] = Roles::goalie;
        } else {
            new_roles[goalie->first] = Roles::goalie;
        }
    }
}

double RoleDecisionAssignmentDistance::getMinChangingTime(unsigned int player) {
    return getTeamMessageStatistics().isStatisticsActive(player) ?
                (getTeamMessageStatistics().getPlayer(player).avgMsgInterval/1000.0) :
                params.minChangingTime;
}

void RoleDecisionAssignmentDistance::withPriority(std::map<unsigned int, Roles::Static>& new_roles) {
    // retrieve assignable roles
    std::vector<Roles::Static> assignable_roles(getRoles().active);
    // the priority of roles is defined by the order of the active roles!
    for(const auto& v : getTeamState().players) {
        if(getTeamMessagePlayersState().isActive(v.first) && new_roles.find(v.first) != new_roles.cend()) {
            // remove already assigned roles
            auto r = std::remove(assignable_roles.begin(), assignable_roles.end(), new_roles.at(v.first));
            if(r != assignable_roles.end()) { assignable_roles.erase(r); }
        }
    }
    // determine the distance between each robot/role and assign the role nearest to the robot
    for (const auto& r : assignable_roles) {
        std::pair<unsigned int, double> smallest(0, std::numeric_limits<double>::max());
        for (const auto& i : getTeamState().players) {
            // only active players and players without an role yet, are used for this role assignment
            if(getTeamMessagePlayersState().isActive(i.first) && new_roles.find(i.first) == new_roles.cend()) {
                double distance = (getRoleDecisionModel().roles_position[r].home - i.second.pose().translation).abs2();
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

void RoleDecisionAssignmentDistance::withDistance(std::map<unsigned int, Roles::Static>& new_roles)
{
    // retrieve assignable roles
    std::vector<Roles::Static> assignable_roles(getRoles().active);
    // retrieve players, which doesn't already have a role and remove already assigned roles from the assignable vector
    std::vector<unsigned int> assignable_player;
    for(const auto& v : getTeamMessagePlayersState().data) {
        if(v.second.isActive()) {
            if(new_roles.find(v.first) == new_roles.cend()) { assignable_player.push_back(v.first); }
            else {
                auto r = std::remove(assignable_roles.begin(), assignable_roles.end(), new_roles.at(v.first));
                if(r != assignable_roles.end()) { assignable_roles.erase(r); }
            }
        }
    }

    findOptimalTeamAssignment(assignable_roles, assignable_player, new_roles);
}

void RoleDecisionAssignmentDistance::withPriorityDistance(std::map<unsigned int, Roles::Static>& new_roles)
{
    // retrieve assignable roles
    std::vector<Roles::Static> assignable_roles(getRoles().active);
    // retrieve players, which doesn't already have a role and remove already assigned roles from the assignable vector
    std::vector<unsigned int> assignable_player;
    for(const auto& v : getTeamMessagePlayersState().data) {
        if(v.second.isActive()) {
            if(new_roles.find(v.first) == new_roles.cend()) {
                // player has NO role and is available for new assignment
                assignable_player.push_back(v.first);
            } else {
                // player a role, remove role from the assignable roles
                auto r = std::remove(assignable_roles.begin(), assignable_roles.end(), new_roles.at(v.first));
                if(r != assignable_roles.end()) { assignable_roles.erase(r); }
            }
        }
    }
    // restrict assignment to the higher priorized roles
    if(assignable_player.size() < assignable_roles.size()) {
        assignable_roles.resize(assignable_player.size());
    }

    findOptimalTeamAssignment(assignable_roles, assignable_player, new_roles);
}

void RoleDecisionAssignmentDistance::findOptimalTeamAssignment(const std::vector<Roles::Static>& roles,
                                                               const std::vector<unsigned int>& players,
                                                               std::map<unsigned int, Roles::Static>& new_roles)
{
    // save the orginal dimensions
    size_t rows = players.size(),
           cols = roles.size(),
           n = std::max(rows, cols);
    // create a squared matrix with default values
    Eigen::Array<int, Eigen::Dynamic, Eigen::Dynamic> m = Eigen::Array<int, Eigen::Dynamic, Eigen::Dynamic>::Constant(n,n,std::numeric_limits<int>::max());
    // prepare distance matrix
    for (size_t r = 0; r < rows; ++r) {
        for (size_t c = 0; c < cols; ++c) {
            auto role = roles[c];
            auto role_pos = getRoleDecisionModel().getStaticRolePosition(role).home;
            auto player = players[r];
            auto player_pos = getTeamState().getPlayer(player).pose().translation;
            m(r,c) = static_cast<int>((role_pos - player_pos).abs2());
        }
    }

    // determine the optimal role assignment (smallest distance the team as a whole has to move)
    HungarianAlgorithm<int>::solve(m);

    // assign optimal roles, with additional debugging output, if there's a non-optimal assignment
    for (size_t r = 0; r < rows; ++r) {
        // retrieve assignment per player
        std::vector<Roles::Static> assignments;
        for (size_t c = 0; c < cols; ++c) {
            if(m(r,c) == 0.0) {
                assignments.push_back(roles[c]);
            }
        }
        // check, if we got one assignment
        if(assignments.size() == 1) {
            // assign optimal role
            new_roles[players[r]] = assignments[0];
        } else if (assignments.size() < 1) {
            // DEBUG: print out non-assignments
            std::cout<<"NO ASSIGNMENT "<<players[r]<<"?"<<std::endl;
            std::cout<<"\n"<<m<<std::endl;
        } else {
            // DEBUG: print out mupltiple-assignments
            std::cout<<"DOUBLE ASSIGNMENT "<<players[r]<<"?"<<std::endl;
            for(auto it : assignments) { std::cout<<Roles::getName(it)<<", "; }
            std::cout<<"\n"<<m<<std::endl;
        }
    }
}

void RoleDecisionAssignmentDistance::roleChangeByCycle(std::map<unsigned int, Roles::Static>& new_roles)
{
    for(const auto& n : new_roles) {
        if(getRoleDecisionModel().roles[n.first].role == n.second) {
            // no change, reset change counter
            role_changes[n.first].first = 0;
        } else {
            // is role decision the same as before?
            if(role_changes[n.first].second == n.second) {
                // yes - check if we reached the max. 'waiting' cycles before definitely change
                if(role_changes[n.first].first >= params.minChangingCycles) {
                    // waited long enough and still want to change the role - do it!
                    roleChange(n.first, n.second);
                } else {
                    // increase change counter
                    role_changes[n.first].first++;
                }
            } else {
                // no - the role decision is different from the previous decision - reset change counter
                role_changes[n.first].first = 0;
                role_changes[n.first].second = n.second;
            }
        }
    }
}

void RoleDecisionAssignmentDistance::roleChangeByTime(std::map<unsigned int, Roles::Static>& new_roles)
{
    for(const auto& it : new_roles)
    {
        // helper vars, to make the decision process clearer!
        const auto& player = it.first;
        const auto& role_new = it.second;
        const auto& role_old = getRoleDecisionModel().roles[player].role;
        const auto& role_last= role_changes[player].second;
        const auto time = getFrameInfo().getTimeInSeconds();

        if(role_old == role_new) {
            // no change, reset change time
            role_changes[player].first = time;
        } else {
            // is role decision the same as before?
            if(role_last == role_new) {
                // yes - check if we reached the max. 'waiting' time before definitely change
                if((time - role_changes[player].first) >= getMinChangingTime(player)) {
                    // waited long enough and still want to change the role - do it!
                    roleChange(player, role_new);
                }
            } else {
                // no - the role decision is different from the previous decision - reset change counter
                role_changes[player].first = time;
                role_changes[player].second = role_new;
            }
        }
    }
}
