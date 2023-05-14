/**
* @file TeamBallLocator.h
*
* Declaration of class TeamBallLocator
*/

#ifndef __TeamBallLocatorMedian_h_
#define __TeamBallLocatorMedian_h_

#include <ModuleFramework/Module.h>
#include <algorithm>

// Debug
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugDrawings.h"
#include "Tools/Debug/DebugModify.h"
#include "Tools/Debug/DebugParameterList.h"

// Representations
#include "Representations/Modeling/PlayerInfo.h"
#include "Representations/Modeling/TeamState.h"
#include "Representations/Modeling/TeamMessagePlayersState.h"
#include "Representations/Modeling/RobotPose.h"
#include "Representations/Modeling/TeamBallModel.h"
#include "Representations/Infrastructure/GameData.h"
#include "Representations/Infrastructure/FrameInfo.h"

// Tools
#include "Tools/DataStructures/ParameterList.h"

BEGIN_DECLARE_MODULE(TeamBallLocatorMedian)
  PROVIDE(DebugModify)
  PROVIDE(DebugRequest)
  PROVIDE(DebugDrawings)
  PROVIDE(DebugParameterList)
  
  REQUIRE(PlayerInfo)
  REQUIRE(TeamState)
  REQUIRE(TeamMessagePlayersState)
  REQUIRE(RobotPose)
  REQUIRE(GameData)
  REQUIRE(FrameInfo)

  PROVIDE(TeamBallModel)
END_DECLARE_MODULE(TeamBallLocatorMedian)

class TeamBallLocatorMedian : protected TeamBallLocatorMedianBase
{
  public:
    // time stamped Vector2d
    class Vector2dTS
    {
      public:
        Vector2dTS() {}
        // this may be implemented the wrong way round
        bool operator< (const Vector2dTS &v2) const
        {
          return this->t < v2.t;
        }
      public:
        Vector2d vec;
        unsigned int t;
    };
    // parameters
    class Parameters: public ParameterList
    {
    public:
      Parameters() : ParameterList("TeamBallMedianParameters")
      {
        PARAMETER_REGISTER(maxTimeOffset) = 1000;
        PARAMETER_REGISTER(maxTimeValid) = 5000;
        syncWithConfig();
      }
      
      unsigned int maxTimeOffset;
      unsigned int maxTimeValid;

    } params;


  public:
    TeamBallLocatorMedian();
    ~TeamBallLocatorMedian();

    virtual void execute();

  private:
    std::vector<Vector2dTS> ballPosHist;
    std::vector<Vector2dTS> ownballPosHist;
    std::map<unsigned int, unsigned long long> lastMessages;
};

#endif //__TeamBallLocatorMedian_h_
