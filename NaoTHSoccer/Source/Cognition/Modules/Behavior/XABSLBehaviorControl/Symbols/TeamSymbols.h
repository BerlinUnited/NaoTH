/**
 * @file: TeamSymbols.h
 * @author: <a href="mailto:scheunem@informatik.hu-berlin.de">Marcus Scheunemann</a>
 *
 * First created on 9. April 2009, 18:10
 */

#ifndef _TEAMSYMBOLS_H
#define _TEAMSYMBOLS_H

#include <ModuleFramework/Module.h>
#include <XabslEngine/XabslEngine.h>

#include "Representations/Modeling/TeamMessage.h"
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Infrastructure/FieldInfo.h"
#include "Representations/Modeling/BallModel.h"
#include "Representations/Modeling/PlayerInfo.h"
#include "Representations/Modeling/RobotPose.h"
#include "Representations/Modeling/BodyState.h"
#include "Representations/Modeling/SoccerStrategy.h"
#include "Representations/Motion/MotionStatus.h"

#include <Tools/DataStructures/ParameterList.h>
#include "Tools/Debug/DebugParameterList.h"

BEGIN_DECLARE_MODULE(TeamSymbols)

  PROVIDE(DebugParameterList)

  REQUIRE(TeamMessage)
  REQUIRE(FrameInfo)
  REQUIRE(BallModel)
  REQUIRE(RobotPose)
  REQUIRE(FieldInfo)
  REQUIRE(BodyState)
  REQUIRE(SoccerStrategy)
  REQUIRE(MotionStatus)

  PROVIDE(PlayerInfo)
END_DECLARE_MODULE(TeamSymbols)

class TeamSymbols: public TeamSymbolsBase
{
public:

  TeamSymbols()
  {
    theInstance = this;
    getDebugParameterList().add(&parameters);
  }

  void registerSymbols(xabsl::Engine& engine);

  virtual void execute();

  virtual ~TeamSymbols();

private:
  class Parameters: public ParameterList
  {
  public: 
    Parameters(): ParameterList("TeamSymbols")
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
    
    virtual ~Parameters() {
    }
  } parameters;

private:
  static TeamSymbols* theInstance;
  static double getTeamMembersAliveCount();
  static bool calculateIfStriker();
  static bool calculateIfStrikerByTimeToBall();
  static int whoIsFastestToBall();
  static bool getWasStriker();
  static void setWasStriker(bool striker);
  static bool calculateIfTheLast();
};

#endif  /* _TEAMSYMBOLS_H */
