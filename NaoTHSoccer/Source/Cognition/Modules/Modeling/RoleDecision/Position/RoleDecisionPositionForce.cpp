#include "RoleDecisionPositionForce.h"
#include "Tools/Math/Common.h"
#include "Tools/Math/Vector3.h"

#define ROLE_GROUPS 3

RoleDecisionPositionForce::RoleDecisionPositionForce()
{
    getDebugParameterList().add(&params);

    DEBUG_REQUEST_REGISTER("RoleDecision:Position:force:dynamic_reset_to_default", "draw role positions on the field", false);
    DEBUG_REQUEST_REGISTER("RoleDecision:Position:force:draw_active_positions_dynamic", "draw role positions on the field", false);
    DEBUG_REQUEST_REGISTER("RoleDecision:Position:force:draw_inactive_positions_dynamic", "draw role positions on the field", false);
    DEBUG_REQUEST_REGISTER("RoleDecision:Position:force:draw_grid_dynamic", "draws the role grid on the field", false);
    DEBUG_REQUEST_REGISTER("RoleDecision:Position:force:draw_default_position", "draws the default role position on the field", false);
    DEBUG_REQUEST_REGISTER("RoleDecision:Position:force:draw_force_sideline", "draws the repeller force of the sidelines", false);
    DEBUG_REQUEST_REGISTER("RoleDecision:Position:force:draw_forces", "draws the applied forces", false);
    //DEBUG_REQUEST_REGISTER("RoleDecision:Position:force:draw_force_teammates", "draws the repeller force of the teammates", false);
    DEBUG_REQUEST_REGISTER("RoleDecision:Position:force:draw_force_ball", "draws the attractor force of the ball", false);

    // init positions with default
    resetPositions();
    initPositionsGrid();
}

RoleDecisionPositionForce::~RoleDecisionPositionForce()
{
    getDebugParameterList().remove(&params);
}

void RoleDecisionPositionForce::resetPositions()
{
    for (const auto& d : getRoles().defaults) {
        getRoleDecisionModel().roles_position[d.first] = d.second;
    }
}

void RoleDecisionPositionForce::initPositionsGrid()
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

void RoleDecisionPositionForce::execute()
{
    DEBUG_REQUEST("RoleDecision:Position:force:dynamic_reset_to_default", resetPositions(); );

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

Vector2d RoleDecisionPositionForce::calculateRepeller(ForceFn method, const Vector2d& point, Vector2d repeller, double force) const
{
    auto distance = (repeller - point).abs();
    auto f = (this->*method)(force, distance);

    Vector3d repeller3d(repeller.x - point.x, repeller.y - point.y, force);
    repeller3d.normalize();
    repeller3d *= f;

    return {-repeller3d.x, -repeller3d.y};
}

Vector2d RoleDecisionPositionForce::calculateAttractor(ForceFn method, const Vector2d& point, Vector2d attractor, double force) const
{
    auto distance = (attractor - point).abs();
    auto f = (this->*method)(force, distance);

    Vector3d attractor3d(attractor.x - point.x, attractor.y - point.y, -force);
    attractor3d.normalize();
    attractor3d *= f;

    return {attractor3d.x, attractor3d.y};
}

Vector2d RoleDecisionPositionForce::calculateRepellerAttractorForce(const Vector2d& p, Roles::Static r) const
{

    Vector2d shift;
    if(params.enableSideline) {
        shift  = calculateRepeller(params.forceSideline, p, {p.x, getFieldInfo().yPosLeftSideline}, params.force_sideline)
           + calculateRepeller(params.forceSideline, p, {p.x, getFieldInfo().yPosRightSideline}, params.force_sideline)
           + calculateRepeller(params.forceSideline, p, {getFieldInfo().xPosOpponentGroundline, p.y}, params.force_sideline)
           + calculateRepeller(params.forceSideline, p, {getFieldInfo().xPosOwnGroundline, p.y}, params.force_sideline);
    }

    for (const auto& a : getRoles().active) {
        // other roles repel the role r
        if(params.enableTeammates && a != Roles::goalie && a != r) {
            shift += calculateRepeller(params.forceTeammate, p, getRoleDecisionModel().roles_position[a].home, params.force_teammates);
        }
        // the roles default position attracts the role r
        if(params.enableDefaultPosition && a == r) {
            auto at = calculateAttractor(params.forcePosition, p, getRoles().defaults.at(a).home, params.force_default_position);
            shift += at;
        }
    }

    if(params.enableBall && getTeamBallModel().valid) {
        auto ball = calculateAttractor(params.forceBall, p, getTeamBallModel().positionOnField, params.force_ball);
        shift += ball;
    }

    return shift * params.update_speed;
}

void RoleDecisionPositionForce::calculateRepellerAttractorPosition(Roles::Static r, std::map<Roles::Static, Vector2d>& pos)
{
    const auto& p = getRoleDecisionModel().roles_position[r].home;
    pos[r] = p + calculateRepellerAttractorForce(p, r);
}

void RoleDecisionPositionForce::debugDrawings() const
{
    // debug drawings
    DEBUG_REQUEST("RoleDecision:Position:force:draw_active_positions_dynamic",
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
    DEBUG_REQUEST("RoleDecision:Position:force:draw_inactive_positions_dynamic",
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
    DEBUG_REQUEST("RoleDecision:Position:force:draw_grid_dynamic",
        FIELD_DRAWING_CONTEXT;
        PEN("000000", 10);
        for(const auto& g : positionGrid) {
            BOX(g.second.min().x, g.second.min().y, g.second.max().x, g.second.max().y);
        }
    );

    DEBUG_REQUEST("RoleDecision:Position:force:draw_default_position",
        FIELD_DRAWING_CONTEXT;
        for(const auto& r : getRoles().defaults) {
            if(getRoles().isRoleActive(r.first)) {
                PEN("969300", 10);
                LINE(r.second.home.x, r.second.home.y-20, r.second.home.x, r.second.home.y+20);
                LINE(r.second.home.x-20, r.second.home.y, r.second.home.x+20, r.second.home.y);
            }
        }
    );

    DEBUG_REQUEST("RoleDecision:Position:force:draw_force_ball",
        FIELD_DRAWING_CONTEXT;
        PEN("ffffff", 10);
        const double stepX = getFieldInfo().xFieldLength/50.0;
        const double stepY = getFieldInfo().yFieldLength/50.0;

//        std::cout << "Ball: ";
        Vector2d p;
        for (p.x = -getFieldInfo().xFieldLength/2.0;
             p.x <= getFieldInfo().xFieldLength/2.0;
             p.x += stepX)
        {
            for (p.y = -getFieldInfo().yFieldLength/2.0;
                 p.y <= getFieldInfo().yFieldLength/2.0;
                 p.y += stepY)
            {
                Vector2d f;
                if(getTeamBallModel().valid) {
                    f += calculateAttractor(params.forceBall, p, getTeamBallModel().positionOnField, params.force_ball);
                }

                auto d = f.abs();
//                std::cout << d << ", ";
                f.normalize(50);
                f+=p;
                PEN(Color(0.0, 0.0, 1.0, d / params.force_ball), 10);
                ARROW(p.x, p.y, f.x, f.y);
            }
        }
//        std::cout << std::endl;
    );

    DEBUG_REQUEST("RoleDecision:Position:force:draw_force_sideline",
        FIELD_DRAWING_CONTEXT;
            const double stepX = getFieldInfo().xLength/50.0;
            const double stepY = getFieldInfo().yLength/50.0;
            Vector2d p;
            for (p.x = -getFieldInfo().xLength/2.0; p.x <= getFieldInfo().xLength/2.0; p.x += stepX)
            {
                for (p.y = -getFieldInfo().yLength/2.0; p.y <= getFieldInfo().yLength/2.0; p.y += stepY)
                {
                    Vector2d f = calculateRepeller(params.forceSideline, p, {p.x, getFieldInfo().yPosLeftSideline}, params.force_sideline)
                               + calculateRepeller(params.forceSideline, p, {p.x, getFieldInfo().yPosRightSideline}, params.force_sideline)
                               + calculateRepeller(params.forceSideline, p, {getFieldInfo().xPosOpponentGroundline, p.y}, params.force_sideline)
                               + calculateRepeller(params.forceSideline, p, {getFieldInfo().xPosOwnGroundline, p.y}, params.force_sideline);

                    auto d = f.abs();
                    f.normalize(50);
                    f+=p;
                    PEN(Color(1.0, 0.0, 0.0, d / params.force_sideline), 10);
                    ARROW(p.x, p.y, f.x, f.y);
                }
            }
    );

    DEBUG_REQUEST("RoleDecision:Position:force:draw_forces",
        FIELD_DRAWING_CONTEXT;
        PEN("ffffff", 10);
        const double stepX = getFieldInfo().xFieldLength/50.0;
        const double stepY = getFieldInfo().yFieldLength/50.0;

        Vector2d p;
        for (p.x = -getFieldInfo().xFieldLength/2.0;
             p.x <= getFieldInfo().xFieldLength/2.0;
             p.x += stepX)
        {
            for (p.y = -getFieldInfo().yFieldLength/2.0;
                 p.y <= getFieldInfo().yFieldLength/2.0;
                 p.y += stepY)
            {
                auto f = calculateRepellerAttractorForce(p, Roles::unknown);
                auto d = f.abs();
                f.normalize(50);
                f+=p;
                PEN(Color(1.0, 0.0, 0.0, d / std::max({params.force_sideline, params.force_teammates, params.force_ball})), 10);
                ARROW(p.x, p.y, f.x, f.y);
            }
        }
    );
}
