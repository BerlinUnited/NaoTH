#ifndef ROLEDECISIONASSIGNMENTTIME_H
#define ROLEDECISIONASSIGNMENTTIME_H

#include <ModuleFramework/Module.h>

#include <Tools/DataStructures/ParameterList.h>
#include "Tools/Debug/DebugParameterList.h"
#include "Representations/Modeling/RoleDecisionModel.h"


BEGIN_DECLARE_MODULE(RoleDecisionAssignmentTime)
  PROVIDE(DebugParameterList)
  PROVIDE(RoleDecisionModel)
END_DECLARE_MODULE(RoleDecisionAssignmentTime);


class RoleDecisionAssignmentTime : public RoleDecisionAssignmentTimeBase
{
public:
    RoleDecisionAssignmentTime()
    {
        //getDebugParameterList().add(&params);
    }

    virtual ~RoleDecisionAssignmentTime()
    {
        //getDebugParameterList().remove(&params);
    }

    virtual void execute();

};

#endif // ROLEDECISIONASSIGNMENTTIME_H
