#include "RoleDecisionPositionDynamic.h"
#include "Tools/Math/Common.h"

RoleDecisionPositionDynamic::RoleDecisionPositionDynamic()
{
    DEBUG_REQUEST_REGISTER("RoleDecision:Dynamic:supporter_position", "draws the supporter position", false);
    getDebugParameterList().add(&params);
}

RoleDecisionPositionDynamic::~RoleDecisionPositionDynamic()
{
    getDebugParameterList().remove(&params);
}

void RoleDecisionPositionDynamic::execute()
{
    const auto& role = getRoleDecisionModel().getRole(getPlayerInfo().playerNumber);

    // only update the position for myself
    switch (role.dynamic)
    {
        case Roles::supporter:
            supporter();
            break;
        case Roles::goalie_supporter:
            // goalieSupporter();
            break;
        default:
            // do nothing
            break;
    }
}

void RoleDecisionPositionDynamic::supporter()
{
    // Get the striker's player number and position
    PlayerNumber strikerNumber = getRoleDecisionModel().getPlayerNumber(Roles::striker);
    if(strikerNumber == 0) { return; }

    const auto& strikerPlayer = getTeamState().getPlayer(strikerNumber);
    Vector2d strikerPos = strikerPlayer.pose().translation;

    // Get the supporter's position
    const auto& supporterPlayer = getTeamState().getPlayer(getPlayerInfo().playerNumber);
    Vector2d supporterPosCurrent = supporterPlayer.pose().translation;

    // Direction from own goal to striker
    Vector2d ownGoal(getFieldInfo().xPosOwnGroundline, 0);
    Vector2d strikerToGoal = (strikerPos - ownGoal).normalize();

    // Vector from striker to supporter
    Vector2d strikerToSupporter = supporterPosCurrent - strikerPos;
    double distanceToStriker = strikerToSupporter.abs();

    // Target position: offset behind striker
    double offset = std::max(params.supporter_offset, distanceToStriker);
    Vector2d supporterPos = strikerPos - strikerToGoal * offset;

    // Compute the normal (perpendicular) vector to the defending line
    Vector2d normal(-strikerToGoal.y, strikerToGoal.x); // 90 degree rotation

    // Determine which side the supporter is currently on
    double side = (strikerToGoal.x * strikerToSupporter.y - strikerToGoal.y * strikerToSupporter.x) >= 0 ? 1.0 : -1.0;

    // Offset along the normal (left/right of the line)
    supporterPos += normal * side * params.supporter_side_offset;

    // Set the computed position
    getRoleDecisionModel().roles_position[getRoleDecisionModel().getRole(getPlayerInfo().playerNumber).role].home = supporterPos;

    // Debug drawing
    DEBUG_REQUEST("RoleDecision:Dynamic:supporter_position",
        FIELD_DRAWING_CONTEXT;
        PEN("00ff00", 20);
        CIRCLE(supporterPos.x, supporterPos.y, 100);
        PEN("0000ff", 20);
        CIRCLE(strikerPos.x, strikerPos.y, 100);
        PEN("0000aa", 20);
        CIRCLE(strikerPos.x, strikerPos.y, params.supporter_offset);
        PEN("ff0000", 20);
        CIRCLE(ownGoal.x, ownGoal.y, 100);
        LINE(ownGoal.x, ownGoal.y, strikerPos.x, strikerPos.y);
        LINE(supporterPos.x, supporterPos.y, strikerPos.x, strikerPos.y);
    );
}