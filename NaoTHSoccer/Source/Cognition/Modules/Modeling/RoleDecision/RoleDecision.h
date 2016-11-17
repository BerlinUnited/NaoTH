/**
* @file RoleDecision.h
*
* @author <a href="mailto:schahin.tofangchi@hu-berlin.de">Schahin Tofangchi</a>
*/

#ifndef _RoleDecision_h_
#define _RoleDecision_h_

#include <ModuleFramework/Module.h>
#include <XabslEngine/XabslEngine.h>

#include "Representations/Modeling/TeamMessage.h"
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Infrastructure/FieldInfo.h"
#include "Representations/Modeling/PlayerInfo.h"
#include "Representations/Modeling/SoccerStrategy.h"
#include "Representations/Modeling/TeamMessageStatisticsModel.h"

#include <Tools/DataStructures/ParameterList.h>
#include "Tools/Debug/DebugParameterList.h"


class RoleDecision
{
public: 

  RoleDecision(){};
  ~RoleDecision(){}

protected:
  class Parameters: public ParameterList
  {
  public: 
    Parameters(): ParameterList("RoleDecision")
    {
      PARAMETER_REGISTER(maximumFreshTime) = 2000;
      PARAMETER_REGISTER(strikerBonusTime) = 4000;
      PARAMETER_REGISTER(maxBallLostTime) = 1000;
      PARAMETER_REGISTER(minFailureProbability) = 0.95; //Only for StableRoleDecision
      
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

  virtual void computeStrikers() = 0;
};

#endif //_RoleDecision_h_
