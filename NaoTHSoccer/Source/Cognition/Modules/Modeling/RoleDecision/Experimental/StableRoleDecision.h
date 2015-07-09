/**
* @file StableRoleDecision.h
*
* @author <a href="mailto:schahin.tofangchi@hu-berlin.de">Schahin Tofangchi</a>
*/

#ifndef _StableRoleDecision_h_
#define _StableRoleDecision_h_

#include <ModuleFramework/Module.h>
#include <XabslEngine/XabslEngine.h>

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
  PROVIDE(DebugParameterList)

  REQUIRE(FrameInfo)
  REQUIRE(FieldInfo)
  REQUIRE(PlayerInfo)
  REQUIRE(SoccerStrategy)
  REQUIRE(TeamMessage)
  REQUIRE(TeamMessageStatisticsModel)

  PROVIDE(RoleDecisionModel)
END_DECLARE_MODULE(StableRoleDecision)

//////////////////// END MODULE INTERFACE DECLARATION //////////////////////

class StableRoleDecision : public StableRoleDecisionBase
{
public: 

  StableRoleDecision();
  virtual ~StableRoleDecision();

  /** executes the module */
  virtual void execute();

  void computeStrikers();
  
protected:
  class Parameters: public ParameterList
  {
  public: 
    Parameters(): ParameterList("StableRoleDecision")
    {
      PARAMETER_REGISTER(maximumFreshTime) = 2000;
      PARAMETER_REGISTER(strikerBonusTime) = 4000;
      PARAMETER_REGISTER(maxBallLostTime) = 1000;
      PARAMETER_REGISTER(minFailureProbability) = 0.85;
      PARAMETER_REGISTER(timeToBall_uncertainty) = 750;
      
      // load from the file after registering all parameters
      syncWithConfig();
    }

    int maximumFreshTime;
    int strikerBonusTime;
    int maxBallLostTime;
    double minFailureProbability;
    int timeToBall_uncertainty; // If the difference between the time_to_ball of two robots lies within the uncertainty, both will go to the ball
    
    virtual ~Parameters() {}
  } parameters;

};

#endif //__StableRoleDecision_h_
