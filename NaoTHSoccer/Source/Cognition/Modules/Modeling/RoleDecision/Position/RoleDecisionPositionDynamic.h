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
            PARAMETER_REGISTER(force_sideline) = 3000;
            PARAMETER_REGISTER(force_teammates) = 2500;
            PARAMETER_REGISTER(force_ball) = 5;
            PARAMETER_REGISTER(force_exp_scale) = 0.2;
            PARAMETER_REGISTER(restrictToGrid) = true;
            PARAMETER_REGISTER(repeller_method, &Parameters::setRepellerMethod) = "exp"; // linear, square exp, expsquare
            // load from the file after registering all parameters
            syncWithConfig();
        }

        double force_sideline;
        double force_teammates;
        double force_ball;
        double force_exp_scale;

        bool restrictToGrid;

        std::string repeller_method;
        double (RoleDecisionPositionDynamic::*repellerForce)(double, double);

        void setRepellerMethod(std::string method) {
            if(method == "linear") { repellerForce = &RoleDecisionPositionDynamic::repellerLinear; }
            else if (method == "square") { repellerForce = &RoleDecisionPositionDynamic::repellerSquare; }
            else if (method == "exp") { repellerForce = &RoleDecisionPositionDynamic::repellerExp; }
            else if (method == "expsquare") { repellerForce = &RoleDecisionPositionDynamic::repellerExpSquare; }
            else { repellerForce = &RoleDecisionPositionDynamic::repellerConst; }
        }
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

    /**
     * @brief Calculates the repeller force in point to the repeller point with the given force.
     *        The force calculation can be tuned via the 'repeller_method' parameter and use one of the
     *        calculation methods below.
     *
     * @param point     the point, where the force should be calculated on
     * @param repeller  the repeller point, which has the ("negative") force
     * @param force     the force, the repeller has
     * @return          the force, which should effect the point
     */
    Vector2d calculateRepeller(const Vector2d& point, Vector2d repeller, double force);

    /** Returns the force as constant. */
    double repellerConst(double f, double /*d*/) { return f; }
    /** Returns the force as the reciprocal of a linear function. */
    double repellerLinear(double f, double d) { return (f / d); }
    /** Returns the force as the reciprocal of a square function. */
    double repellerSquare(double f, double d) { return (f / (d * d)); }
    /** Returns the force as the exponential of a linear function. */
    double repellerExp(double f, double d) { return params.force_exp_scale * exp(f / d); }
    /** Returns the force as the exponential of a squared function. */
    double repellerExpSquare(double f, double d) { return params.force_exp_scale * exp(f / (d*d)); }
};

#endif // ROLEDECISIONPOSITIONDYNAMIC_H
