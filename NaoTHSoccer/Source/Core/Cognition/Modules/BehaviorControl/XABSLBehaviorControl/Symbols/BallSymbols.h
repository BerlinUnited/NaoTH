/**
* @file BallSymbols.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Definition of class BallSymbols
*/

#ifndef _BallSymbols_H_
#define _BallSymbols_H_

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

BEGIN_DECLARE_MODULE(BallSymbols)
  REQUIRE(BallModel)
  REQUIRE(BallPercept)
  REQUIRE(BallPerceptTop)
  REQUIRE(FrameInfo)
  REQUIRE(RobotPose)
  REQUIRE(TeamBallModel)
  REQUIRE(FieldInfo)
  REQUIRE(KinematicChain)
  REQUIRE(SoccerStrategy)
END_DECLARE_MODULE(BallSymbols)

class BallSymbols: public BallSymbolsBase
{

public:
  BallSymbols() {
    theInstance = this;
  }
  virtual ~BallSymbols(){}
  
  void registerSymbols(xabsl::Engine& engine);
  
  virtual void execute();

private:
  static BallSymbols* theInstance;

  // setter and getter
  static double getBallDistance();
  static double getBallAngle();
  static double getBallTimeSinceLastSeen();
  static double getBallTimeSeen();
  static double getTeamBallTimeSinceLastUpdate();
  static double getTeamBallGoalieTimeSinceLastUpdate();
  static double getTeamBallStrikerTimeSinceLastUpdate();

  // some local members
  Vector2d ballPositionField;

  Vector2d ballLeftFoot;
  Vector2d ballRightFoot;


  Vector2d ballPerceptPos;
  bool ballPerceptSeen;

};//end class BallSymbols

#endif // _BallSymbols_H_
