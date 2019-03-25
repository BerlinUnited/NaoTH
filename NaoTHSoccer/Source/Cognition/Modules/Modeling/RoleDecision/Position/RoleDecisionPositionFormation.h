#ifndef ROLEDECISIONPOSITIONFORMATION_H
#define ROLEDECISIONPOSITIONFORMATION_H

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


BEGIN_DECLARE_MODULE(RoleDecisionPositionFormation)
  PROVIDE(DebugRequest)
  PROVIDE(DebugDrawings)
  PROVIDE(DebugParameterList)

  REQUIRE(FieldInfo)
  REQUIRE(Roles)
  REQUIRE(TeamBallModel)

  PROVIDE(RoleDecisionModel)
END_DECLARE_MODULE(RoleDecisionPositionFormation);


class RoleDecisionPositionFormation : public RoleDecisionPositionFormationBase
{
public:
    RoleDecisionPositionFormation();
    ~RoleDecisionPositionFormation();

    virtual void execute();

private:
    class Parameters: public ParameterList
    {
    public:
        Parameters() : ParameterList("RoleDecisionPositionFormation")
        {
            PARAMETER_REGISTER(defenderMinFactorX) = 0.5;
            PARAMETER_REGISTER(defenderMaxFactorX) = 1.5;
            PARAMETER_REGISTER(defenderMinFactorY) = 0.5;
            PARAMETER_REGISTER(defenderMaxFactorY) = 2.0;
            PARAMETER_ANGLE_REGISTER(defenderMinAngle) = -60.0;
            PARAMETER_ANGLE_REGISTER(defenderMaxAngle) =  60.0;

            PARAMETER_REGISTER(midfielderMinFactorX) = 0.2;
            PARAMETER_REGISTER(midfielderMaxFactorX) = 1.5;
            PARAMETER_REGISTER(midfielderMinFactorY) = 0.5;
            PARAMETER_REGISTER(midfielderMaxFactorY) = 2.0;
            PARAMETER_ANGLE_REGISTER(midfielderMinAngle) = -15.0;
            PARAMETER_ANGLE_REGISTER(midfielderMaxAngle) =  15.0;

            PARAMETER_REGISTER(forwardMinFactorX) = 0.2;
            PARAMETER_REGISTER(forwardMaxFactorX) = 1.5;
            PARAMETER_REGISTER(forwardMinFactorY) = 0.5;
            PARAMETER_REGISTER(forwardMaxFactorY) = 2.0;
            PARAMETER_ANGLE_REGISTER(forwardMinAngle) = -5.0;
            PARAMETER_ANGLE_REGISTER(forwardMaxAngle) =  5.0;
        }

        double defenderMinFactorX;
        double defenderMaxFactorX;
        double defenderMinFactorY;
        double defenderMaxFactorY;
        double defenderMinAngle;
        double defenderMaxAngle;

        double midfielderMinFactorX;
        double midfielderMaxFactorX;
        double midfielderMinFactorY;
        double midfielderMaxFactorY;
        double midfielderMinAngle;
        double midfielderMaxAngle;

        double forwardMinFactorX;
        double forwardMaxFactorX;
        double forwardMinFactorY;
        double forwardMaxFactorY;
        double forwardMinAngle;
        double forwardMaxAngle;

        double minPositionDistance;
    } params;

    void debugDrawings() const;
};

#endif // ROLEDECISIONPOSITIONFORMATION_H
