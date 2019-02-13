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


BEGIN_DECLARE_MODULE(RoleDecisionPositionDynamic)
  PROVIDE(DebugRequest)
  PROVIDE(DebugDrawings)
  PROVIDE(DebugParameterList)

  REQUIRE(FieldInfo)
  REQUIRE(Roles)
  REQUIRE(TeamBallModel)

  PROVIDE(RoleDecisionModel)
END_DECLARE_MODULE(RoleDecisionPositionDynamic);


typedef RoleDecisionModel RM;


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
            // load from the file after registering all parameters
            syncWithConfig();
        }

        double force_sideline;
        double force_teammates;
        double force_ball;
    } params;

    void reset();
    void calculateRepellerAttractorPosition(Roles::Static r, std::map<Roles::Static, Vector2d> &pos);
};

#endif // ROLEDECISIONPOSITIONDYNAMIC_H
