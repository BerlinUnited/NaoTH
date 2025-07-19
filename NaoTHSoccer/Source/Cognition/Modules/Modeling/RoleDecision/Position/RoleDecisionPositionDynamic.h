#ifndef ROLEDECISIONPOSITIONDYNAMIC_H
#define ROLEDECISIONPOSITIONDYNAMIC_H

#include <ModuleFramework/Module.h>
#include <Tools/DataStructures/ParameterList.h>
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugDrawings.h"
#include "Tools/Debug/DebugParameterList.h"
#include "Tools/StringTools.h"

#include "Representations/Infrastructure/FieldInfo.h"
#include "Representations/Infrastructure/Roles.h"
#include "Representations/Modeling/RoleDecisionModel.h"
#include "Representations/Modeling/TeamBallModel.h"
#include "Representations/Modeling/PlayerInfo.h"
#include "Representations/Modeling/TeamState.h"
#include "Representations/Infrastructure/GameData.h"
#include "Representations/Modeling/BallModel.h"
#include "Representations/Modeling/RobotPose.h"

BEGIN_DECLARE_MODULE(RoleDecisionPositionDynamic)
  PROVIDE(DebugRequest)
  PROVIDE(DebugDrawings)
  PROVIDE(DebugParameterList)

  REQUIRE(FieldInfo)
  REQUIRE(Roles)
  REQUIRE(TeamBallModel)
  REQUIRE(PlayerInfo)
  REQUIRE(TeamState)
  REQUIRE(GameData)
  REQUIRE(BallModel)
  REQUIRE(RobotPose)

  PROVIDE(RoleDecisionModel)
END_DECLARE_MODULE(RoleDecisionPositionDynamic);


class RoleDecisionPositionDynamic : public RoleDecisionPositionDynamicBase
{
public:
    RoleDecisionPositionDynamic();
    virtual ~RoleDecisionPositionDynamic();
    virtual void execute();

    void goalie();
    void striker();
    void supporter();

    Vector2d calculateEllipsePoint(const Vector2d& ball);

private:
    class Parameters: public ParameterList
    {
    public:
        Parameters() : ParameterList("RoleDecisionPositionDynamic")
        {
            PARAMETER_REGISTER(goalie_max_come_out) = 300; // 600 -> penalty area
            PARAMETER_REGISTER(goalie_defense_min_x) = -750;
            PARAMETER_REGISTER(goalie_defense_max_x) = 0;

            PARAMETER_REGISTER(supporter_offset) = 1000;
            PARAMETER_REGISTER(supporter_offset_side) = 200;
            PARAMETER_REGISTER(supporter_offense_scaling) = 2.0; // how far the supporter should be in the oppnent half

            PARAMETER_REGISTER(setplay_second_defender_offset) = 500;
            PARAMETER_REGISTER(setplay_second_defender_scaling) = 4.0;

            // load from the file after registering all parameters
            syncWithConfig();
        }

        bool goalie_last_active;
        double goalie_max_come_out;
        double goalie_defense_min_x;
        double goalie_defense_max_x;

        double supporter_offset;
        double supporter_offset_side;
        double supporter_offense_scaling;

        double setplay_second_defender_offset;
        double setplay_second_defender_scaling;
    } params;

    bool isDefendingSetPlay();
    void positionBetweenBallAndGoal();
    void positionOrthogonalToBall();
};

#endif // ROLEDECISIONPOSITIONDYNAMIC_H
