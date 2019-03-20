#include "RoleDecisionPositionPotentialField.h"
#include <iomanip>

#define ROLE_GROUPS 3

RoleDecisionPositionPotentialField::RoleDecisionPositionPotentialField()
{
    getDebugParameterList().add(&params);

    DEBUG_REQUEST_REGISTER("RoleDecision:Position:pf_dynamic_reset_to_default", "draw role positions on the field", false);
    DEBUG_REQUEST_REGISTER("RoleDecision:Position:pf_draw_active_positions_dynamic", "draw role positions on the field", false);
    DEBUG_REQUEST_REGISTER("RoleDecision:Position:pf_draw_inactive_positions_dynamic", "draw role positions on the field", false);
    DEBUG_REQUEST_REGISTER("RoleDecision:Position:pf_draw_grid_dynamic", "draws the role grid on the field", false);
    DEBUG_REQUEST_REGISTER("RoleDecision:Position:pf_draw_default_position", "draws the default role position on the field", false);
    for (const auto& r : getRoles().active) {
        DEBUG_REQUEST_REGISTER("RoleDecision:Position:pf_draw_field_"+Roles::getName(r), "draws the potential field of the " + Roles::getName(r), false);
    }

    // init positions with default
    resetPositions();
    initPositionsGrid();

}

RoleDecisionPositionPotentialField::~RoleDecisionPositionPotentialField()
{
    getDebugParameterList().remove(&params);
}

void RoleDecisionPositionPotentialField::resetPositions()
{
    for (const auto& d : getRoles().defaults) {
        getRoleDecisionModel().roles_position[d.first] = d.second;
    }
}

void RoleDecisionPositionPotentialField::initPositionsGrid()
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

void RoleDecisionPositionPotentialField::execute()
{
    DEBUG_REQUEST("RoleDecision:Position:pf_dynamic_reset_to_default", resetPositions(); );

    // container for the new positions,
    std::map<Roles::Static, Vector2d> new_home_positions;

    // determine the new position for each role
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

void RoleDecisionPositionPotentialField::calculateRepellerAttractorPosition(Roles::Static r, std::map<Roles::Static, Vector2d>& pos)
{
    const auto& p = getRoleDecisionModel().roles_position[r].home;
    pos[r] = p + calculateRepellerAttractorForce(p, r) * params.update_speed;
}

Vector2d RoleDecisionPositionPotentialField::calculateRepellerAttractorForce(const Vector2d& p, Roles::Static r) const
{
    Vector2d shift;
    if(params.enableSideline) {
        shift = calculate(params.forceSideline, p, {p.x, getFieldInfo().yPosLeftSideline}, params.force_sideline, params.force_sideline_scale)
              + calculate(params.forceSideline, p, {p.x, getFieldInfo().yPosRightSideline}, params.force_sideline, params.force_sideline_scale)
              + calculate(params.forceSideline, p, {getFieldInfo().xPosOpponentGroundline, p.y}, params.force_sideline, params.force_sideline_scale)
              + calculate(params.forceSideline, p, {getFieldInfo().xPosOwnGroundline, p.y}, params.force_sideline, params.force_sideline_scale);
    }

    for (const auto& a : getRoles().active) {
        // other roles repel the role r
        if(params.enableTeammates && a != Roles::goalie && a != r) {
            shift += calculate(params.forceTeammate, p, getRoleDecisionModel().roles_position[a].home, params.force_teammates, params.force_teammates_scale);
        }
        // the roles default position attracts the role r
        if(params.enableDefaultPosition && a == r) {
            shift -= calculate(params.forcePosition, p, getRoles().defaults.at(a).home, params.force_default_position, params.force_default_position_scale);
        }
    }

    if(params.enableBall && getTeamBallModel().valid) {
        shift -= calculate(params.forceBall, p, getTeamBallModel().positionOnField, params.force_ball, params.force_ball_scale);
    }

    return shift;
}

Vector2d RoleDecisionPositionPotentialField::calculate(ForceFn method, const Vector2d& point, Vector2d obstacle, double force, double scale) const
{
    auto distance = (obstacle - point).abs();
    auto f = (this->*method)(force, distance, scale);
    obstacle -=point;
    return obstacle.normalize() * f;
}


void RoleDecisionPositionPotentialField::debugDrawings() const
{

    DEBUG_REQUEST("RoleDecision:Position:pf_draw_default_position",
        FIELD_DRAWING_CONTEXT;
        for(const auto& r : getRoles().defaults) {
            if(getRoles().isRoleActive(r.first)) {
                PEN("969300", 10);
                LINE(r.second.home.x, r.second.home.y-20, r.second.home.x, r.second.home.y+20);
                LINE(r.second.home.x-20, r.second.home.y, r.second.home.x+20, r.second.home.y);
            }
        }
    );

    DEBUG_REQUEST("RoleDecision:Position:pf_draw_active_positions_dynamic",
        FIELD_DRAWING_CONTEXT;
        for(const auto& r : getRoleDecisionModel().roles_position) {
            if(getRoles().isRoleActive(r.first)) {
                // more functional presentation
                PEN("666666", 20);
                LINE(r.second.home.x, r.second.home.y-20, r.second.home.x, r.second.home.y+20);
                LINE(r.second.home.x-20, r.second.home.y, r.second.home.x+20, r.second.home.y);
                // mark active positions with a circle
                PEN("666666", 10);
                CIRCLE(r.second.home.x, r.second.home.y, 40);

                // nicer representation (eg. for images)
                //PEN("0000ff", 20);
                //ROBOT(r.second.home.x, r.second.home.y, 0);
                //TEXT_DRAWING2(r.second.home.x, r.second.home.y-250, 0.6, Roles::getName(r.first));
            }
        }
    );

    DEBUG_REQUEST("RoleDecision:Position:pf_draw_inactive_positions_dynamic",
        FIELD_DRAWING_CONTEXT;
        for(const auto& r : getRoleDecisionModel().roles_position) {
            if(!getRoles().isRoleActive(r.first)) {
                // more functional presentation
                PEN("666666", 20);
                LINE(r.second.home.x, r.second.home.y-20, r.second.home.x, r.second.home.y+20);
                LINE(r.second.home.x-20, r.second.home.y, r.second.home.x+20, r.second.home.y);

                // nicer representation (eg. for images)
                //PEN("0000ff", 20);
                //ROBOT(r.second.home.x, r.second.home.y, 0);
                //TEXT_DRAWING2(r.second.home.x, r.second.home.y-250, 0.6, Roles::getName(r.first));
            }
        }
    );

    DEBUG_REQUEST("RoleDecision:Position:pf_draw_grid_dynamic",
        FIELD_DRAWING_CONTEXT;
        PEN("000000", 10);
        for(const auto& g : positionGrid) {
            BOX(g.second.min().x, g.second.min().y, g.second.max().x, g.second.max().y);
        }
    );

    for (const auto& r : getRoles().active) {
        debugDrawings("RoleDecision:Position:pf_draw_field_"+Roles::getName(r), r);
    }

}

void RoleDecisionPositionPotentialField::debugDrawings(std::string debugRequestName, Roles::Static r) const
{
    const bool& _debug_request_is_active_ = getDebugRequest().getValueReference(debugRequestName);
    if(_debug_request_is_active_) {
        FIELD_DRAWING_CONTEXT;
        PEN("ffffff", 10);
        const double stepX = getFieldInfo().xFieldLength/100.0;
        const double stepY = getFieldInfo().yFieldLength/100.0;

        Vector2d p;
        for (p.x = -getFieldInfo().xFieldLength/2.0;
             p.x <= getFieldInfo().xFieldLength/2.0;
             p.x += stepX)
        {
            for (p.y = -getFieldInfo().yFieldLength/2.0;
                 p.y <= getFieldInfo().yFieldLength/2.0;
                 p.y += stepY)
            {
                auto f = calculateRepellerAttractorForce(p, r);
                auto d = f.abs();

                f.normalize(50);
                f+=p;
                PEN(Color(1.0, 0.0, 0.0, d / std::max({params.force_sideline, params.force_teammates, params.force_ball})), 10);
                //ARROW(p.x, p.y, f.x, f.y);
                CIRCLE(p.x, p.y, 5);
            }
        }
    }
}
