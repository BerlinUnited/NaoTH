#ifndef ROLEDECISIONPOSITIONFORCE_H
#define ROLEDECISIONPOSITIONFORCE_H

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


BEGIN_DECLARE_MODULE(RoleDecisionPositionForce)
  PROVIDE(DebugRequest)
  PROVIDE(DebugDrawings)
  PROVIDE(DebugParameterList)

  REQUIRE(FieldInfo)
  REQUIRE(Roles)
  REQUIRE(TeamBallModel)

  PROVIDE(RoleDecisionModel)
END_DECLARE_MODULE(RoleDecisionPositionForce);


class RoleDecisionPositionForce : public RoleDecisionPositionForceBase
{
    typedef double (RoleDecisionPositionForce::*ForceFn)(double, double) const;

public:
    RoleDecisionPositionForce();
    virtual ~RoleDecisionPositionForce();
    virtual void execute();

private:
    class Parameters: public ParameterList
    {
    public:
        Parameters() : ParameterList("RoleDecisionPositionDynamic")
        {
            PARAMETER_REGISTER(enableSideline) = true;
            PARAMETER_REGISTER(enableTeammates) = true;
            PARAMETER_REGISTER(enableBall) = true;
            PARAMETER_REGISTER(enableDefaultPosition) = true;

            PARAMETER_REGISTER(force_sideline) = 4000;
            PARAMETER_REGISTER(force_teammates) = 4000;
            PARAMETER_REGISTER(force_ball) = 4000;
            PARAMETER_REGISTER(force_default_position) = 2000;

            PARAMETER_REGISTER(force_scale) = 0.2;
            PARAMETER_REGISTER(update_speed) = 0.05;

            PARAMETER_REGISTER(restrictToGrid) = false;

            // const, linear, square exp, exprec, expsquare, cos
            PARAMETER_REGISTER(force_method_sideline, &Parameters::setSidelineMethod) = "exprec";
            PARAMETER_REGISTER(force_method_teammate, &Parameters::setTeammateMethod) = "exprec";
            PARAMETER_REGISTER(force_method_ball,     &Parameters::setBallMethod)     = "linear";
            PARAMETER_REGISTER(force_method_position, &Parameters::setPositionMethod) = "const";

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
        double update_speed;
        bool restrictToGrid;

        std::string force_method_sideline;
        std::string force_method_teammate;
        std::string force_method_ball;
        std::string force_method_position;

        ForceFn forceSideline;
        ForceFn forceTeammate;
        ForceFn forceBall;
        ForceFn forcePosition;

        void setSidelineMethod(std::string method) { setMethod(forceSideline, method); }
        void setTeammateMethod(std::string method) { setMethod(forceTeammate, method); }
        void setBallMethod(std::string method)     { setMethod(forceBall, method); }
        void setPositionMethod(std::string method) { setMethod(forcePosition, method); }

        void setMethod(ForceFn& methodVar, std::string method)
        {
            if(method == "linear")          { methodVar = &RoleDecisionPositionForce::forceLinear; }
            else if (method == "square")    { methodVar = &RoleDecisionPositionForce::forceSquare; }
            else if (method == "exp")       { methodVar = &RoleDecisionPositionForce::forceExp; }
            else if (method == "exprec")    { methodVar = &RoleDecisionPositionForce::forceExpRec; }
            else if (method == "expsquare") { methodVar = &RoleDecisionPositionForce::forceExpSquare; }
            else if (method == "cos")       { methodVar = &RoleDecisionPositionForce::forceCos; }
            else                            { methodVar = &RoleDecisionPositionForce::forceConst; }
        }
    } params;

    std::map<Roles::Static, Geometry::Rect2d> positionGrid;

    /**
     * @brief Resets the positions to the default (starting) positions.
     */
    void resetPositions();

    /**
     * @brief Calculates the new home position of the role r.
     *        Different forces have an effect on the new position (see calculateRepellerAttractorPosition)
     *
     * @param r the role, which home position should be calculated
     * @param pos   sets the new position in the 'pos' map
     */
    void calculateRepellerAttractorPosition(Roles::Static r, std::map<Roles::Static, Vector2d> &pos);

    /**
     * @brief Calculates the sum of the applied forces and returns a vector indicating by how much the point should be shifted.
     *        The following forces are applied: sidelines, ball, teammates, default position.
     *
     * @param p the point, where the forces should be calculated
     * @param r the conisdered role
     * @return a vector indicating by how much the point should be shifted
     */
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
     * @brief Calculates the repulsion force in point to the repeller point with the given force.
     *        The force calculation can be tuned via the respecting 'force_method' parameter.
     *
     * @param point     the point, where the force should be calculated on
     * @param repeller  the repeller point, which has the ("negative") force
     * @param force     the force, the repeller has
     * @return          the force, which should effect the point
     */
    Vector2d calculateRepeller(ForceFn method, const Vector2d& point, Vector2d repeller, double force) const;

    /**
     * @brief Calculates the attraction force in the point to the attractor point with the given force.
     *        The force calculation can be tuned via the respecting 'force_method' parameter.
     *
     * @param point     the point, where the force should be calculated on
     * @param repeller  the acttraction point, which has the attracting force
     * @param force     the force, the attractor has
     * @return          the force, which should effect the point
     */
    Vector2d calculateAttractor(ForceFn method, const Vector2d& point, Vector2d attractor, double force) const;

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
};

#endif // ROLEDECISIONPOSITIONFORCE_H
