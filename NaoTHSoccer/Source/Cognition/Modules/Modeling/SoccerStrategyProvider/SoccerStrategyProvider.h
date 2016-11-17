/**
* @file SoccerStrategyProvider.h
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* provides soccer strategy
*/

#ifndef _SoccerStrategyProvider_h_
#define _SoccerStrategyProvider_h_

#include <ModuleFramework/Module.h>

#include "Tools/DataStructures/RingBufferWithSum.h"
#include "Tools/Math/Vector3.h"
#include "Tools/Math/Common.h"
#include "Tools/Debug/DebugPlot.h"
#include "Tools/Debug/DebugRequest.h"

// Representations
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Infrastructure/FieldInfo.h"
#include "Representations/Modeling/BallModel.h"
#include "Representations/Modeling/ObstacleModel.h"
#include "Representations/Modeling/RobotPose.h"
#include "Representations/Modeling/PlayerInfo.h"
#include "Representations/Modeling/SoccerStrategy.h"
#include "Representations/Modeling/RawAttackDirection.h"
#include "Representations/Modeling/BodyState.h"
#include "Representations/Modeling/PlayersModel.h"
#include "Representations/Motion/MotionStatus.h"

//////////////////// BEGIN MODULE INTERFACE DECLARATION ////////////////////

BEGIN_DECLARE_MODULE(SoccerStrategyProvider)
  PROVIDE(DebugPlot)
  PROVIDE(DebugRequest)
  
  REQUIRE(FrameInfo)
  REQUIRE(FieldInfo)
  REQUIRE(BallModel)
  REQUIRE(RobotPose)
  REQUIRE(PlayerInfo)
  REQUIRE(BodyState)
  REQUIRE(PlayersModel)
  REQUIRE(ObstacleModel)
  REQUIRE(RawAttackDirection)
  REQUIRE(MotionStatus)

  PROVIDE(SoccerStrategy)
END_DECLARE_MODULE(SoccerStrategyProvider)

//////////////////// END MODULE INTERFACE DECLARATION //////////////////////

class SoccerStrategyProvider : public SoccerStrategyProviderBase
{
public: 

  class FormationParameters
  {
  public:
    FormationParameters();

    FormationParameters(unsigned int playerNumber);

    ~FormationParameters(){}

    unsigned int num;
    Vector2<double> home;
    Vector3<double> attr;
    Vector2<double> min;
    Vector2<double> max;
    bool behindBall;
    bool penaltyAreaAllowed;
  };

  SoccerStrategyProvider();
  virtual ~SoccerStrategyProvider(){}

  /** executes the module */
  virtual void execute();

protected:
  Vector2<double> calculateForamtion() const;
  
  double estimateTimeToBall() const;

  double estimateTimeToPoint(const Vector2d& p) const;
  //NEW:double estimateTimeToPoint(const Vector2d& p, const double& offensiveness=0.0) const;

  //NEW:double estimateTimeFromBallTo_GoalDir(const Vector2d& attackDir, const Vector2d& p) const;

  //NEW:int timeToGetPastObstacle(const Vector2d& ballPos) const;

  //NEW:bool isSomethingBetweenPoints(const Vector2d& p1, Vector2d& p2) const;
  bool isSomeoneBetweenMeAndPoint(const Vector2d& p) const;

private:
  FormationParameters theFormationParameters;
  RingBufferWithSum<Vector2<double>, 3> attackDirectionBuffer;
};

#endif //__SoccerStrategyProvider_h_
