#include "RoleDecisionPositionDynamic.h"
#include "Tools/Math/Common.h"

RoleDecisionPositionDynamic::RoleDecisionPositionDynamic()
{
    getDebugParameterList().add(&params);

    DEBUG_REQUEST_REGISTER("RoleDecision:Position:draw_positions_dynamic", "draw role positions on the field", false);
    DEBUG_REQUEST_REGISTER("RoleDecision:Position:dynamic_reset_to_default", "draw role positions on the field", false);

    // init positions with default
    reset();
}

RoleDecisionPositionDynamic::~RoleDecisionPositionDynamic()
{
    getDebugParameterList().remove(&params);
}

void RoleDecisionPositionDynamic::reset()
{
    for (const auto& d : getRoles().defaults) {
        getRoleDecisionModel().roles_position[d.first] = d.second;
    }
}

void RoleDecisionPositionDynamic::execute()
{
    // debug drawings
    DEBUG_REQUEST("RoleDecision:Position:dynamic_reset_to_default",
        reset();
    );

    std::map<Roles::Static, Vector2d> new_home_positions;
    //
    for (const auto& d : getRoles().defaults) {
        if(d.first != Roles::goalie) {
            calculateRepellerAttractorPosition(d.first, new_home_positions);
        }
    }

    for(const auto& n : new_home_positions) {
        if(getFieldInfo().fieldRect.inside(n.second)) {
            getRoleDecisionModel().roles_position[n.first].home = n.second;
        }
        // TODO: what should we do with 'invalid' positions?!
    }

    // debug drawings
    DEBUG_REQUEST("RoleDecision:Position:draw_positions_dynamic",
        FIELD_DRAWING_CONTEXT;
        PEN("666666", 20);
        for(const auto& r : getRoleDecisionModel().roles_position) {
            LINE(r.second.home.x, r.second.home.y-20, r.second.home.x, r.second.home.y+20);
            LINE(r.second.home.x-20, r.second.home.y, r.second.home.x+20, r.second.home.y);
        }
    );
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

    for (const auto& d : getRoles().defaults) {
        if(d.first != Roles::goalie && d.first != r) {
            auto mate = getRoleDecisionModel().roles_position[d.first].home;
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
        auto db = ball.abs();
        ball.normalize();
        // TODO: should the ball attractor depend on the ball distance?!?
        auto b_force = params.force_ball;// / db;

        ball *= b_force;
        pos[r] += ball;
    }
}
