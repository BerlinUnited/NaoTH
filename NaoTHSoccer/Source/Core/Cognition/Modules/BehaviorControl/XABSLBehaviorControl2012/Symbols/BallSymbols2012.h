/**
* @file BallSymbols2012.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Definition of class BallSymbols2012
*/

#ifndef __BallSymbols2012_H_
#define __BallSymbols2012_H_

#include <ModuleFramework/Module.h>
#include <XabslEngine/XabslEngine.h>

#include "Tools/Math/Pose2D.h"

// representations
#include "Representations/Modeling/BallModel.h"
#include "Representations/Perception/BallPercept.h"
#include "Representations/Modeling/TeamBallModel.h"
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Infrastructure/FieldInfo.h"
#include "Representations/Modeling/RobotPose.h"
#include "Representations/Modeling/KinematicChain.h"
#include "Representations/Modeling/SoccerStrategy.h"

BEGIN_DECLARE_MODULE(BallSymbols2012)
  REQUIRE(BallModel)
  REQUIRE(BallPercept)
  REQUIRE(FrameInfo)
  REQUIRE(RobotPose)
  REQUIRE(TeamBallModel)
  REQUIRE(FieldInfo)
  REQUIRE(KinematicChain)
  REQUIRE(SoccerStrategy)
END_DECLARE_MODULE(BallSymbols2012)

class BallSymbols2012: public BallSymbols2012Base
{

public:
  BallSymbols2012()
    :
    ballModel(getBallModel()),
    ballPercept(getBallPercept()),
    frameInfo(getFrameInfo()),
    robotPose(getRobotPose()),
    teamBallModel(getTeamBallModel()),
    fieldInfo(getFieldInfo()),
    kinematicChain(getKinematicChain()),
    soccerStrategy(getSoccerStrategy()),

    futureBallPosX_t(0.0),
    futureBallPosY_t(0.0),
    futureBallDistance_t(0.0),

    goToPointX(0.0),
    goToPointY(0.0),
    goToPointDistanceX(0.0),
    goToPointDistanceY(0.0)

  {
    theInstance = this;
  };
  virtual ~BallSymbols2012(){}
  
  /** registers the symbols at an engine */
  void registerSymbols(xabsl::Engine& engine);
  
  virtual void execute();

private:

  static BallSymbols2012* theInstance;

  // representations
  BallModel const& ballModel;
  BallPercept const& ballPercept;
  FrameInfo const& frameInfo;
  RobotPose const& robotPose;
  TeamBallModel const& teamBallModel;
  FieldInfo const& fieldInfo;
  const KinematicChain& kinematicChain;
  SoccerStrategy const& soccerStrategy;

  // setter and getter
  static double getBallDistance();

  static double getFutureBallPosX();
  static double getFutureBallPosY();
  static double getFutureBallDistance();
  double futureBallPosX_t;
  double futureBallPosY_t;
  double futureBallDistance_t;

  static double getBallAngle();
  static double getBallTimeSinceLastSeen();
  static double getBallTimeSeen();
  static double getTeamBallTimeSinceLastUpdate();
  static double getTeamBallGoalieTimeSinceLastUpdate();
  static double getTeamBallStrikerTimeSinceLastUpdate();
  static double getBallSpeed();

  static void calculatePosBehindBallFuture();
  static double getPosBehindBallFutureX();
  static double getPosBehindBallFutureY();
  static double getPosBehindBallFutureRotation();
  Pose2D posBehindBall;
  double goToPointX;
  double goToPointY;
  double goToPointDistanceX;
  double goToPointDistanceY;

  // some local members
  Vector2 <double> ballPositionField;

  Vector2 <double> ballLeftFoot;
  Vector2 <double> ballRightFoot;
};//end class BallSymbols2012

#endif // __BallSymbols2012_H_
