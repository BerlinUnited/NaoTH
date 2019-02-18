#include "RoleDecisionPositionDynamic.h"
#include "Tools/Math/Common.h"

#define ROLE_GROUPS 3

RoleDecisionPositionDynamic::RoleDecisionPositionDynamic()
{
    getDebugParameterList().add(&params);

    DEBUG_REQUEST_REGISTER("RoleDecision:Position:dynamic_reset_to_default", "draw role positions on the field", false);
    DEBUG_REQUEST_REGISTER("RoleDecision:Position:draw_active_positions_dynamic", "draw role positions on the field", false);
    DEBUG_REQUEST_REGISTER("RoleDecision:Position:draw_inactive_positions_dynamic", "draw role positions on the field", false);
    DEBUG_REQUEST_REGISTER("RoleDecision:Position:draw_grid_dynamic", "draws the role grid on the field", false);

    // init positions with default
    resetPositions();
    initPositionsGrid();
}

RoleDecisionPositionDynamic::~RoleDecisionPositionDynamic()
{
    getDebugParameterList().remove(&params);
}

void RoleDecisionPositionDynamic::resetPositions()
{
    for (const auto& d : getRoles().defaults) {
        getRoleDecisionModel().roles_position[d.first] = d.second;
    }
}

void RoleDecisionPositionDynamic::initPositionsGrid()
{
    // make sure the grid is empty
    positionGrid.clear();

    // count how many active roles are in the respective group (without goalie)
    std::map<int, unsigned int> partition;
    for(const auto& r : getRoles().active) {
        if(r != Roles::goalie) {
            // NOTE: assuming groups of similiar roles of size 3!
            //       3 defender, 3 midfielder, 3 forwards
            partition[r/ROLE_GROUPS]++;
        }
    }

    int prevGroup = -1;
    int yIdx = -1;
    // use only the first 3/4 of the field for the grid positioning
    double gridX = (getFieldInfo().xLength - getFieldInfo().xLength*0.25) / static_cast<double>(partition.size());
    for(int i = 0; i < Roles::numOfStaticRoles-1; ++i)
    {
        Roles::Static r = static_cast<Roles::Static>(i);
        if(!getRoles().isRoleActive(r)) { continue; }

        int group = i/ROLE_GROUPS;
        double gridY = getFieldInfo().yLength / partition[group];

        if(prevGroup != group) {
            yIdx = 0;
            prevGroup = group;
        } else {
            yIdx++;
        }

        positionGrid[r] = {
            {getFieldInfo().xPosOwnGroundline + gridX*group, getFieldInfo().yPosLeftSideline - gridY*yIdx},
            {getFieldInfo().xPosOwnGroundline + gridX*(group+1), getFieldInfo().yPosLeftSideline - gridY*(yIdx+1)}
        };
    }
}

void RoleDecisionPositionDynamic::execute()
{
    DEBUG_REQUEST("RoleDecision:Position:dynamic_reset_to_default", resetPositions(); );

    // container for the new positions,
    std::map<Roles::Static, Vector2d> new_home_positions;

    // determine the new position
    for (const auto& r : getRoles().active) {
        if(r != Roles::goalie) {
            calculateRepellerAttractorPosition(r, new_home_positions);
        }
    }

    // restrict the calculated positions to a grid
    if(params.restrictToGrid) {
        for(auto& it : new_home_positions) {
            if(positionGrid.find(it.first) != positionGrid.cend()) {
                positionGrid[it.first].clamp(it.second);

            }
        }
    }

    // apply the new positions
    for(const auto& n : new_home_positions) {
        if(getFieldInfo().fieldRect.inside(n.second)) {
            getRoleDecisionModel().roles_position[n.first].home = n.second;
        }
        // TODO: what should we do with 'invalid' positions?!
    }

    debugDrawings();
}

void RoleDecisionPositionDynamic::calculateRepellerAttractorPosition(Roles::Static r, std::map<Roles::Static, Vector2d>& pos)
{
    const auto& p = getRoleDecisionModel().roles_position[r].home;

    Vector2d l_side(p.x, getFieldInfo().yPosLeftSideline);
    Vector2d r_side(p.x, getFieldInfo().yPosRightSideline);
    Vector2d f_side(getFieldInfo().xPosOpponentGroundline, p.y);
    Vector2d b_side(getFieldInfo().xPosOwnGroundline, p.y);

    l_side -= p;
    r_side -= p;
    f_side -= p;
    b_side -= p;

    auto dl = l_side.abs(),
         dr = r_side.abs(),
         df = f_side.abs(),
         db = b_side.abs();

    l_side.normalize();
    r_side.normalize();
    f_side.normalize();
    b_side.normalize();

    auto l_force = -1 * params.force_sideline / (dl*dl);
    auto r_force = -1 * params.force_sideline / (dr*dr);
    auto f_force = -1 * params.force_sideline / (df*df);
    auto b_force = -1 * params.force_sideline / (db*db);

    l_side *= l_force;
    r_side *= r_force;
    f_side *= f_force;
    b_side *= b_force;

    pos[r] = p + l_side + r_side + f_side + b_side;

    for (const auto& a : getRoles().active) {
        if(a != Roles::goalie && a != r) {
            auto mate = getRoleDecisionModel().roles_position[a].home;
            mate -= p;
            auto dm = mate.abs();
            mate.normalize();
            auto m_force = -1 * params.force_teammates / (dm*dm);
            mate *= m_force;
            pos[r] += mate;
        }
    }

    if(getTeamBallModel().valid) {
        Vector2d b_side(getFieldInfo().xPosOwnGroundline, p.y);
        auto ball = getTeamBallModel().positionOnField;
        //ball -= b_side;
        ball -= p;
        //auto db = ball.abs();
        ball.normalize();
        // TODO: should the ball attractor depend on the ball distance?!?
        auto b_force = params.force_ball;// / db;

        ball *= b_force;
        pos[r] += ball;
    }
}

void RoleDecisionPositionDynamic::debugDrawings() const
{
    // debug drawings
    DEBUG_REQUEST("RoleDecision:Position:draw_active_positions_dynamic",
        FIELD_DRAWING_CONTEXT;
        for(const auto& r : getRoleDecisionModel().roles_position) {
            if(getRoles().isRoleActive(r.first)) {
                PEN("666666", 20);
                LINE(r.second.home.x, r.second.home.y-20, r.second.home.x, r.second.home.y+20);
                LINE(r.second.home.x-20, r.second.home.y, r.second.home.x+20, r.second.home.y);
                // mark active positions with a circle
                PEN("666666", 10);
                CIRCLE(r.second.home.x, r.second.home.y, 40);
            }
        }
    );
    DEBUG_REQUEST("RoleDecision:Position:draw_inactive_positions_dynamic",
        FIELD_DRAWING_CONTEXT;
        for(const auto& r : getRoleDecisionModel().roles_position) {
            if(!getRoles().isRoleActive(r.first)) {
                PEN("666666", 20);
                LINE(r.second.home.x, r.second.home.y-20, r.second.home.x, r.second.home.y+20);
                LINE(r.second.home.x-20, r.second.home.y, r.second.home.x+20, r.second.home.y);
            }
        }
    );
    DEBUG_REQUEST("RoleDecision:Position:draw_grid_dynamic",
        FIELD_DRAWING_CONTEXT;
        PEN("000000", 10);
        for(const auto& g : positionGrid) {
            BOX(g.second.min().x, g.second.min().y, g.second.max().x, g.second.max().y);
        }
    );
}
