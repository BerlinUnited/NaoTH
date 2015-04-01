/**
* @file StableRoleDecision.h
*
* @author <a href="mailto:schahin.tofangchi@hu-berlin.de">Schahin Tofangchi</a>
*/

#ifndef _StableRoleDecision_h_
#define _StableRoleDecision_h_

#include <ModuleFramework/Module.h>
#include <XabslEngine/XabslEngine.h>

#include "Cognition/Modules/Modeling/RoleDecision/RoleDecision.h"

#include "Representations/Modeling/TeamMessage.h"
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Infrastructure/FieldInfo.h"
#include "Representations/Modeling/PlayerInfo.h"
#include "Representations/Modeling/RoleDecisionModel.h"
#include "Representations/Modeling/SoccerStrategy.h"
#include "Representations/Modeling/TeamMessageStatisticsModel.h"

#include <Tools/DataStructures/ParameterList.h>
#include "Tools/Debug/DebugParameterList.h"
#include "Tools/Debug/DebugPlot.h"
#include "Tools/Debug/DebugRequest.h"

//////////////////// BEGIN MODULE INTERFACE DECLARATION ////////////////////

BEGIN_DECLARE_MODULE(StableRoleDecision)
  PROVIDE(DebugPlot)
  PROVIDE(DebugRequest)

  REQUIRE(FrameInfo)
  REQUIRE(FieldInfo)
  REQUIRE(PlayerInfo)
  REQUIRE(SoccerStrategy)
  REQUIRE(TeamMessage)
  REQUIRE(TeamMessageStatisticsModel)

  PROVIDE(RoleDecisionModel)
END_DECLARE_MODULE(StableRoleDecision)

//////////////////// END MODULE INTERFACE DECLARATION //////////////////////

class StableRoleDecision : public StableRoleDecisionBase, public RoleDecision
{
public: 

  StableRoleDecision();
  virtual ~StableRoleDecision(){}

  /** executes the module */
  virtual void execute();

  void computeStrikers();

};

#endif //__StableRoleDecision_h_
