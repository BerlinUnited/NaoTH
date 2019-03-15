#ifndef ROLEDECISIONPOSITIONSIMULATION_H
#define ROLEDECISIONPOSITIONSIMULATION_H

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


BEGIN_DECLARE_MODULE(RoleDecisionPositionSimulation)
  PROVIDE(DebugRequest)
  PROVIDE(DebugDrawings)
  PROVIDE(DebugParameterList)

  REQUIRE(FieldInfo)
  REQUIRE(Roles)
  REQUIRE(TeamBallModel)

  PROVIDE(RoleDecisionModel)
END_DECLARE_MODULE(RoleDecisionPositionSimulation);



class RoleDecisionPositionSimulation : public RoleDecisionPositionSimulationBase
{
public:
    RoleDecisionPositionSimulation();
    ~RoleDecisionPositionSimulation();

    virtual void execute();

private:
    class Parameters: public ParameterList
    {
    public:
        Parameters() : ParameterList("RoleDecisionPositionSimulation")
        {
        }

        int numParticles;

        double ball_speed;
        double ball_speed_std;
        double ball_angle;
        double ball_angle_std;
    } params;

    Vector2d predict(const Vector2d& ball, bool noise);
};

#endif // ROLEDECISIONPOSITIONSIMULATION_H
