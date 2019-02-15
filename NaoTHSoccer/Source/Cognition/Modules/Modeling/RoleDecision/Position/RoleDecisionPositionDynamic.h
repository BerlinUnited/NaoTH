#ifndef ROLEDECISIONPOSITIONDYNAMIC_H
#define ROLEDECISIONPOSITIONDYNAMIC_H

#include <ModuleFramework/Module.h>
#include <Tools/DataStructures/ParameterList.h>
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugDrawings.h"
#include "Tools/Debug/DebugParameterList.h"
#include "Tools/StringTools.h"
#include "Tools/Math/Geometry.h"

#include "Representations/Infrastructure/FieldInfo.h"
#include "Representations/Infrastructure/Roles.h"
#include "Representations/Modeling/RoleDecisionModel.h"
#include "Representations/Modeling/TeamBallModel.h"


BEGIN_DECLARE_MODULE(RoleDecisionPositionDynamic)
  PROVIDE(DebugRequest)
  PROVIDE(DebugDrawings)
  PROVIDE(DebugParameterList)

  REQUIRE(FieldInfo)
  REQUIRE(Roles)
  REQUIRE(TeamBallModel)

  PROVIDE(RoleDecisionModel)
END_DECLARE_MODULE(RoleDecisionPositionDynamic);


class RoleDecisionPositionDynamic : public RoleDecisionPositionDynamicBase
{
public:
    RoleDecisionPositionDynamic();
    virtual ~RoleDecisionPositionDynamic();
    virtual void execute();

private:
    class Parameters: public ParameterList
    {
    public:
        Parameters() : ParameterList("RoleDecisionPositionDynamic")
        {
            PARAMETER_REGISTER(force_sideline) = 10000000;
            PARAMETER_REGISTER(force_teammates) = 10000000;
            PARAMETER_REGISTER(force_ball) = 10;
            PARAMETER_REGISTER(restrictToGrid) = true;
            // load from the file after registering all parameters
            syncWithConfig();
        }

        double force_sideline;
        double force_teammates;
        double force_ball;
        bool restrictToGrid;
    } params;

    std::map<Roles::Static, Geometry::Rect2d> positionGrid;

    /**
     * @brief Resets the positions to the default (starting) positions.
     */
    void resetPositions();

    /**
     * @brief calculateRepellerAttractorPosition
     * @param r
     * @param pos
     */
    void calculateRepellerAttractorPosition(Roles::Static r, std::map<Roles::Static, Vector2d> &pos);

    /**
     * @brief Initializes the positions grid.
     *        The field is divded into a grid, where each role has a certain grid cell where it can move.
     *        The role is not allowed to leave the grid cell, if restrictToGrid == true.
     */
    void initPositionsGrid();

    /**
     * @brief Some debug drawings for this module.
     */
    void debugDrawings() const;
};

#endif // ROLEDECISIONPOSITIONDYNAMIC_H
