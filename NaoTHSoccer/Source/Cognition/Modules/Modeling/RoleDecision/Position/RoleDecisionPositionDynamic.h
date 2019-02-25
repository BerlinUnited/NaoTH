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
            PARAMETER_REGISTER(enableSideline) = true;
            PARAMETER_REGISTER(force_sideline) = 3000;
            PARAMETER_REGISTER(enableTeammates) = true;
            PARAMETER_REGISTER(force_teammates) = 2500;
            PARAMETER_REGISTER(enableBall) = true;
            PARAMETER_REGISTER(force_ball) = 5;
            PARAMETER_REGISTER(enableDefaultPosition) = true;
            PARAMETER_REGISTER(force_default_position) = 2500;

            PARAMETER_REGISTER(force_scale) = 0.2;
            PARAMETER_REGISTER(restrictToGrid) = true;
            PARAMETER_REGISTER(repeller_method, &Parameters::setRepellerMethod) = "exprec"; // linear, square exp, expsquare
            PARAMETER_REGISTER(attractor_method, &Parameters::setAttractorMethod) = "const"; // linear, square exp, expsquare

            // load from the file after registering all parameters
            syncWithConfig();
        }

        bool enableSideline;
        double force_sideline;

        bool enableTeammates;
        double force_teammates;

        bool enableBall;
        double force_ball;

        bool enableDefaultPosition;
        double force_default_position;

        double force_scale;
        bool restrictToGrid;

        std::string repeller_method;
        std::string attractor_method;
        double (RoleDecisionPositionDynamic::*repellerForce)(double, double) const;
        double (RoleDecisionPositionDynamic::*attractorForce)(double, double) const;

        void setRepellerMethod(std::string method) {
            if(method == "linear") { repellerForce = &RoleDecisionPositionDynamic::forceLinear; }
            else if (method == "square") { repellerForce = &RoleDecisionPositionDynamic::forceSquare; }
            else if (method == "exp") { repellerForce = &RoleDecisionPositionDynamic::forceExp; }
            else if (method == "exprec") { repellerForce = &RoleDecisionPositionDynamic::forceExpRec; }
            else if (method == "expsquare") { repellerForce = &RoleDecisionPositionDynamic::forceExpSquare; }
            else if (method == "cos") { repellerForce = &RoleDecisionPositionDynamic::forceCos; }
            else { repellerForce = &RoleDecisionPositionDynamic::forceConst; }
        }

        void setAttractorMethod(std::string method) {
            if(method == "linear") { attractorForce = &RoleDecisionPositionDynamic::forceLinear; }
            else if (method == "square") { attractorForce = &RoleDecisionPositionDynamic::forceSquare; }
            else if (method == "exp") { attractorForce = &RoleDecisionPositionDynamic::forceExp; }
            else if (method == "exprec") { attractorForce = &RoleDecisionPositionDynamic::forceExpRec; }
            else if (method == "expsquare") { attractorForce = &RoleDecisionPositionDynamic::forceExpSquare; }
            else if (method == "cos") { attractorForce = &RoleDecisionPositionDynamic::forceCos; }
            else { attractorForce = &RoleDecisionPositionDynamic::forceConst; }
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

    Vector2d calculateRepellerAttractorForce(const Vector2d& p, Roles::Static r) const;

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
    Vector2d calculateRepeller(const Vector2d& point, Vector2d repeller, double force) const;

    /** Returns the force as constant. */
    double forceConst(double f, double /*d*/) const { return f; }
    /** Returns the force as the reciprocal of a linear function. */
    double forceLinear(double f, double d) const { return d>f?0:(f-d); }
    /** Returns the force as the reciprocal of a square function. */
    double forceSquare(double f, double d) const { return (d*d)>f?0:(f - (d * d)); }
    /** Returns the force as the exponential of a linear function. */
    double forceExp(double f, double d) const { return f * exp(-(d*d)/params.force_scale); }
    /** Returns the force as the exponential of a squared function. */
    double forceExpRec(double f, double d) const { return params.force_scale * exp(f / d); }
    /**  */
    double forceExpSquare(double f, double d) const { return params.force_scale * exp(f / (d*d)); }
    /** Returns the force as the cos function. */
    double forceCos(double f, double d) const { return (params.force_scale * d)>=(Math::pi_2)?0.0:cos(params.force_scale * d) * f; }

    Vector2d calculateAttractor(const Vector2d& point, Vector2d attractor, double force) const;
};

#endif // ROLEDECISIONPOSITIONDYNAMIC_H
