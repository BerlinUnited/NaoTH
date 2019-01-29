#ifndef ROLEDECISIONASSIGNMENTTEAMDISTANCE_H
#define ROLEDECISIONASSIGNMENTTEAMDISTANCE_H

#include <ModuleFramework/Module.h>
#include <Tools/DataStructures/ParameterList.h>
#include "Tools/Debug/DebugParameterList.h"
#include "Representations/Modeling/RoleDecisionModel.h"


BEGIN_DECLARE_MODULE(RoleDecisionAssignmentTeamDistance)
  PROVIDE(DebugParameterList)
  PROVIDE(RoleDecisionModel)
END_DECLARE_MODULE(RoleDecisionAssignmentTeamDistance);


class RoleDecisionAssignmentTeamDistance : public RoleDecisionAssignmentTeamDistanceBase
{
public:
    RoleDecisionAssignmentTeamDistance()
    {
        //getDebugParameterList().add(&params);
    }

    virtual ~RoleDecisionAssignmentTeamDistance()
    {
        //getDebugParameterList().remove(&params);
    }

    virtual void execute();

};

#endif // ROLEDECISIONASSIGNMENTTEAMDISTANCE_H
