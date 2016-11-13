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
#include "Representations/Modeling/BodyState.h"
#include "Representations/Modeling/BallModel.h"

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
  REQUIRE(BodyState)
  REQUIRE(BallModel)

  PROVIDE(RoleDecisionModel)
END_DECLARE_MODULE(StableRoleDecision)

//////////////////// END MODULE INTERFACE DECLARATION //////////////////////

class StableRoleDecision : public StableRoleDecisionBase, public RoleDecision
{
public: 

  StableRoleDecision();
  virtual ~StableRoleDecision();

  /** executes the module */
  virtual void execute();

  void computeStrikers();
  
protected:
  bool isRobotDead(unsigned int robotNumber);

  bool amIactive() {
      return getBodyState().fall_down_state == BodyState::upright
          && getPlayerInfo().robotState != PlayerInfo::penalized
          && getBallModel().valid
          && (getFrameInfo().getTimeSince(getBallModel().getFrameInfoWhenBallWasSeen().getTime()) < parameters.maxBallLostTime + myStrikerBonusTime());
  }

  int myStrikerBonusTime() {
      return getRoleDecisionModel().firstStriker == getPlayerInfo().playerNumber ? parameters.strikerBonusTime:0;
  }

  class Parameters: public ParameterList
  {
  public: 
    Parameters(): ParameterList("StableRoleDecision")
    {
      PARAMETER_REGISTER(maximumFreshTime) = 2000;
      PARAMETER_REGISTER(strikerBonusTime) = 4000;
      PARAMETER_REGISTER(maxBallLostTime) = 1000;
      PARAMETER_REGISTER(minFailureProbability) = 0.85;
      
      // load from the file after registering all parameters
      syncWithConfig();
    }

    int maximumFreshTime;
    int strikerBonusTime;
    int maxBallLostTime;
    double minFailureProbability;
    
    virtual ~Parameters() {}
  } parameters;

};

#endif //__StableRoleDecision_h_
