#ifndef ROLEDECISIONPOSITIONPOTENTIALFIELD_H
#define ROLEDECISIONPOSITIONPOTENTIALFIELD_H

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


BEGIN_DECLARE_MODULE(RoleDecisionPositionPotentialField)
  PROVIDE(DebugRequest)
  PROVIDE(DebugDrawings)
  PROVIDE(DebugParameterList)

  REQUIRE(FieldInfo)
  REQUIRE(Roles)
  REQUIRE(TeamBallModel)

  PROVIDE(RoleDecisionModel)
END_DECLARE_MODULE(RoleDecisionPositionPotentialField);

class RoleDecisionPositionPotentialField : public RoleDecisionPositionPotentialFieldBase
{
    typedef double (RoleDecisionPositionPotentialField::*ForceFn)(double, double, double) const;

public:
    RoleDecisionPositionPotentialField();
    virtual ~RoleDecisionPositionPotentialField();
    virtual void execute();

private:
    class Parameters: public ParameterList
    {
    public:
        Parameters() : ParameterList("RoleDecisionPositionPotentialField")
        {
            PARAMETER_REGISTER(enableSideline) = true;
            PARAMETER_REGISTER(enableTeammates) = true;
            PARAMETER_REGISTER(enableBall) = true;
            PARAMETER_REGISTER(enableDefaultPosition) = true;

            PARAMETER_REGISTER(force_sideline) = 4000;
            PARAMETER_REGISTER(force_sideline_scale) = 1;
            PARAMETER_REGISTER(force_teammates) = 4000;
            PARAMETER_REGISTER(force_teammates_scale) = 500;
            PARAMETER_REGISTER(force_ball) = 4000;
            PARAMETER_REGISTER(force_ball_scale) = 2;
            PARAMETER_REGISTER(force_default_position) = 0.8;
            PARAMETER_REGISTER(force_default_position_scale) = -1;

            PARAMETER_REGISTER(update_speed) = 2.0;

            PARAMETER_REGISTER(restrictToGrid) = false;

            // const, linear, square exp, exprec, expsquare, cos
            PARAMETER_REGISTER(force_method_sideline, &Parameters::setSidelineMethod) = "exprec";
            PARAMETER_REGISTER(force_method_teammate, &Parameters::setTeammateMethod) = "expnegrec";
            PARAMETER_REGISTER(force_method_ball,     &Parameters::setBallMethod)     = "linear";
            PARAMETER_REGISTER(force_method_position, &Parameters::setPositionMethod) = "const";

            // load from the file after registering all parameters
            syncWithConfig();
        }

        bool enableSideline;
        double force_sideline;
        double force_sideline_scale;

        bool enableTeammates;
        double force_teammates;
        double force_teammates_scale;

        bool enableBall;
        double force_ball;
        double force_ball_scale;

        bool enableDefaultPosition;
        double force_default_position;
        double force_default_position_scale;

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
            if(method == "linear")          { methodVar = &RoleDecisionPositionPotentialField::forceLinear; }
            else if (method == "square")    { methodVar = &RoleDecisionPositionPotentialField::forceSquare; }
            else if (method == "ratio")     { methodVar = &RoleDecisionPositionPotentialField::forceRatio; }
            else if (method == "exp")       { methodVar = &RoleDecisionPositionPotentialField::forceExp; }
            else if (method == "exprec")    { methodVar = &RoleDecisionPositionPotentialField::forceExpRec; }
            else if (method == "exprec2")   { methodVar = &RoleDecisionPositionPotentialField::forceExpRecSquare; }
            else if (method == "expnegrec") { methodVar = &RoleDecisionPositionPotentialField::forceExpNegRec; }
            else if (method == "expnegrec2"){ methodVar = &RoleDecisionPositionPotentialField::forceExpNegRecSquare; }
            else if (method == "cos")       { methodVar = &RoleDecisionPositionPotentialField::forceCos; }
            else                            { methodVar = &RoleDecisionPositionPotentialField::forceConst; }
        }
    } params;

    std::map<Roles::Static, Geometry::Rect2d> positionGrid;

    /**
     * @brief Resets the positions to the default (starting) positions.
     */
    void resetPositions();

    /**
     * @brief Initializes the positions grid.
     *        The field is divded into a grid, where each role has a certain grid cell where it can move.
     *        The role is not allowed to leave the grid cell, if restrictToGrid == true.
     */
    void initPositionsGrid();

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
     * @brief Calculates the repulsion force in point to the repeller point with the given force.
     *        The force calculation can be tuned via the respecting 'force_method' parameter.
     *
     * @param point     the point, where the force should be calculated on
     * @param repeller  the repeller point, which has the ("negative") force
     * @param force     the force, the repeller has
     * @return          the force, which should effect the point
     */
    Vector2d calculate(ForceFn method, const Vector2d& point, Vector2d obstacle, double force, double scale) const;

    /**
     * @brief Some debug drawings for this module.
     */
    void debugDrawings() const;
    void debugDrawings(std::string debugRequestName, Roles::Static r) const;

    /** Returns the force as constant $\operatorname{force} = s \cdot f$. */
    double forceConst(double f, double /*d*/, double s) const { return s*f; }
    /** Returns the force as a linear function: $\operatorname{force} = \begin{cases} 0 &d>f\\f-d &\end{cases}$ */
    double forceLinear(double f, double d, double s) const { return d>f||d==0.0 ? 0 : -s; }
    /** Returns the force as a square function: $\operatorname{force} = \begin{cases} 0 &d^2>f\\f-d^2 &\end{cases}$ */
    double forceSquare(double f, double d, double s) const { return (d*d)>f ? 0 : -2*s*d; }
    /** Returns the force as a square function: $\operatorname{force} = \frac{s \cdot f}{d}$ */
    double forceRatio(double f, double d, double s) const { return d == 0.0 ? 0 : -s*f / (d*d); }
    /** Returns the force as a square function: $\operatorname{force} = f \cdot e^{s \cdot d}$ */
    double forceExp(double f, double d, double s) const { return - s * f * exp(-s * d); }
    /** Returns the force as a square function: $\operatorname{force} = s \cdot e^{\frac{f}{d}}$ */
    double forceExpRec(double f, double d, double s) const { return s*exp(f/d) * (-f/(d*d)); }
    /** Returns the force as a square function: $\operatorname{force} = s \cdot e^{\frac{f}{d^2}}$ */
    double forceExpRecSquare(double f, double d, double s) const { return s*exp(f/(d*d)) * ((-2*f)/(d*d*d)); }
    /** Returns the force as a square function: $\operatorname{force} = f \cdot e^{-\frac{d}{s}}$ */
    double forceExpNegRec(double f, double d, double s) const { return f*exp(-d/s) * (-1/s); }
    /** Returns the force as a square function: $\operatorname{force} = f \cdot e^{-\frac{d^2}{s}}$ */
    double forceExpNegRecSquare(double f, double d, double s) const { return f*exp(-(d*d)/s)*((-2*d)/s); }
    /** Returns the force as the cos function: $\operatorname{force} = f \cdot \cos({s \cdot d})$ */
    double forceCos(double f, double d, double s) const { return (s * d)>=(Math::pi_2)?0.0:-s*f*sin(s*d); }
};

#endif // ROLEDECISIONPOSITIONPOTENTIALFIELD_H
