/**
* @file BallSymbols.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Definition of class BallSymbols
*/

#ifndef __BallSymbols2011_H_
#define __BallSymbols2011_H_

#include <ModuleFramework/Module.h>
#include <XabslEngine/XabslEngine.h>

// representations
#include "Representations/Modeling/BallModel.h"
#include "Representations/Perception/BallPercept.h"
#include "Representations/Modeling/TeamBallModel.h"
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Infrastructure/FieldInfo.h"
#include "Representations/Modeling/RobotPose.h"
#include "Representations/Modeling/KinematicChain.h"

BEGIN_DECLARE_MODULE(BallSymbols2011)
  REQUIRE(BallModel)
  REQUIRE(BallPercept)
  REQUIRE(FrameInfo)
  REQUIRE(RobotPose)
  REQUIRE(TeamBallModel)
  REQUIRE(FieldInfo)
  REQUIRE(KinematicChain)
END_DECLARE_MODULE(BallSymbols2011)

class BallSymbols2011: public BallSymbols2011Base
{

public:
  BallSymbols2011()
    :
    ballModel(getBallModel()),
    ballPercept(getBallPercept()),
    frameInfo(getFrameInfo()),
    robotPose(getRobotPose()),
    teamBallModel(getTeamBallModel()),
    fieldInfo(getFieldInfo()),
    kinematicChain(getKinematicChain()),

    futureBallPosX_t(0.0),
    futureBallPosY_t(0.0),
    futureBallDistance_t(0.0)

  {
    theInstance = this;
  };
  ~BallSymbols2011(){};
  
  /** registers the symbols at an engine */
  void registerSymbols(xabsl::Engine& engine);
  
  virtual void execute();

private:

  static BallSymbols2011* theInstance;

  // representations
  BallModel const& ballModel;
  BallPercept const& ballPercept;
  FrameInfo const& frameInfo;
  RobotPose const& robotPose;
  TeamBallModel const& teamBallModel;
  FieldInfo const& fieldInfo;
  const KinematicChain& kinematicChain;

  // seter and getter
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
  static double getBallSpeed();

  // some local members
  Vector2 <double> ballPositionField;

  Vector2 <double> ballLeftFoot;
  Vector2 <double> ballRightFoot;
};//end class BallSymbols

#endif // __BallSymbols2011_H_
