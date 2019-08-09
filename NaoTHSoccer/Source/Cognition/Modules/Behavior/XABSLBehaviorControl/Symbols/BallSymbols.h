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
#include "Representations/Motion/MotionStatus.h"

// representations
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Infrastructure/FieldInfo.h"

#include "Representations/Perception/MultiBallPercept.h"
#include "Representations/Modeling/BallModel.h"
#include "Representations/Modeling/RobotPose.h"
#include "Representations/Modeling/TeamBallModel.h"
#include "Representations/Modeling/OdometryData.h"

#include <Tools/Debug/DebugRequest.h>
#include <Tools/Debug/DebugDrawings.h>

BEGIN_DECLARE_MODULE(BallSymbols)
  PROVIDE(DebugRequest)
  PROVIDE(DebugDrawings)

  REQUIRE(BallModel)
  
  REQUIRE(FrameInfo)
  REQUIRE(FieldInfo)

  REQUIRE(MultiBallPercept)
  REQUIRE(TeamBallModel)

  REQUIRE(RobotPose)
  REQUIRE(OdometryData)
  REQUIRE(MotionStatus)
END_DECLARE_MODULE(BallSymbols)

class BallSymbols: public BallSymbolsBase
{

public:
  BallSymbols() :
    ballPerceptSeen(false)
  {
    theInstance = this;
  }
  virtual ~BallSymbols() { 
  }
  
  void registerSymbols(xabsl::Engine& engine);
  
  virtual void execute();

private:
  static BallSymbols* theInstance;

  // setter and getter
  static double getBallDistance();
  static double getBallAngle();
  static double getBallTimeSinceLastSeen();
  static double getBallTimeSeen();

  // global ball
  Vector2d ballPositionField;

  // percept
  Vector2d ballPerceptPos;
  bool ballPerceptSeen;
  // needed for traching ball percept
  Pose2D lastRobotOdometry;

  Vector2d futureBallPreview;
  
  Vector2d last_known_ball_preview;

  Vector2d interceptionPointPreview;
};//end class BallSymbols

#endif // _BallSymbols_H_
