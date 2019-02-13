#ifndef ROLEDECISIONDYNAMICGOALIE_H
#define ROLEDECISIONDYNAMICGOALIE_H

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


BEGIN_DECLARE_MODULE(RoleDecisionPositionDynamicGoalie)
  PROVIDE(DebugRequest)
  PROVIDE(DebugDrawings)
  PROVIDE(DebugParameterList)

  REQUIRE(FieldInfo)
  REQUIRE(Roles)
  REQUIRE(TeamBallModel)

  PROVIDE(RoleDecisionModel)
END_DECLARE_MODULE(RoleDecisionPositionDynamicGoalie);


class RoleDecisionPositionDynamicGoalie : public RoleDecisionPositionDynamicGoalieBase
{
public:
    RoleDecisionPositionDynamicGoalie();
    virtual ~RoleDecisionPositionDynamicGoalie();
    virtual void execute();

private:
    class Parameters: public ParameterList
    {
    public:
        Parameters() : ParameterList("RoleDecisionPositionDynamicGoalie")
        {
            PARAMETER_REGISTER(goalie_defense_min_x) = 0;
            // load from the file after registering all parameters
            syncWithConfig();
        }

        double goalie_defense_min_x;
    } params;
};

#endif // ROLEDECISIONDYNAMICGOALIE_H
