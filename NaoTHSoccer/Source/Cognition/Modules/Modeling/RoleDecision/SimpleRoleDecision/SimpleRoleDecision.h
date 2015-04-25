/**
* @file SimpleRoleDecision.h
*
* @author <a href="mailto:schahin.tofangchi@hu-berlin.de">Schahin Tofangchi</a>
*/

#ifndef _SimpleRoleDecision_h_
#define _SimpleRoleDecision_h_

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
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugPlot.h"
#include "Tools/Debug/DebugParameterList.h"

//////////////////// BEGIN MODULE INTERFACE DECLARATION ////////////////////

BEGIN_DECLARE_MODULE(SimpleRoleDecision)
  PROVIDE(DebugPlot)
  PROVIDE(DebugRequest)
  PROVIDE(DebugParameterList)

  REQUIRE(FrameInfo)
  REQUIRE(FieldInfo)
  REQUIRE(PlayerInfo)
  REQUIRE(SoccerStrategy)
  REQUIRE(TeamMessage)

  PROVIDE(RoleDecisionModel)
END_DECLARE_MODULE(SimpleRoleDecision)

//////////////////// END MODULE INTERFACE DECLARATION //////////////////////

class SimpleRoleDecision : public SimpleRoleDecisionBase
{
public: 

  SimpleRoleDecision();
  virtual ~SimpleRoleDecision();
  
  /** executes the module */
  virtual void execute();

  void computeStrikers();

  protected:
  class Parameters: public ParameterList
  {
  public: 
    Parameters(): ParameterList("SimpleRoleDecision")
    {
      PARAMETER_REGISTER(maximumFreshTime) = 2000;
      PARAMETER_REGISTER(strikerBonusTime) = 4000;
      PARAMETER_REGISTER(maxBallLostTime) = 1000;
      
      // load from the file after registering all parameters
      syncWithConfig();
    }

    int maximumFreshTime;
    int strikerBonusTime;
    int maxBallLostTime;
    double minFailureProbability;
    
    virtual ~Parameters() {
    }
  } parameters;

};

#endif //__SimpleRoleDecision_h_
