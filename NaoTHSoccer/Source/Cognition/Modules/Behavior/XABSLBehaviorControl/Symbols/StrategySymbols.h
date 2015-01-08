#ifndef _StrategySymbols_H_
#define _StrategySymbols_H_

#include <ModuleFramework/Module.h>
#include <XabslEngine/XabslEngine.h>

// representations
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Modeling/AttentionModel.h"
#include "Representations/Modeling/PlayersModel.h"
#include "Representations/Modeling/BallModel.h"
#include "Representations/Modeling/RobotPose.h"
#include "Representations/Modeling/GoalModel.h"
#include "Representations/Modeling/SoccerStrategy.h"
#include "Representations/Modeling/SituationStatus.h"
#include "Representations/Modeling/ActionModel.h"
#include "Representations/Motion/MotionStatus.h"
#include "Representations/Modeling/CompassDirection.h"

#include "Tools/Debug/DebugDrawings.h"
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugModify.h"
#include "Tools/Debug/DebugPlot.h"
BEGIN_DECLARE_MODULE(StrategySymbols)
  REQUIRE(AttentionModel)
  REQUIRE(PlayersModel)
  REQUIRE(FrameInfo)
  REQUIRE(BallModel)
  REQUIRE(RobotPose)
  REQUIRE(SelfLocGoalModel)
  REQUIRE(PlayerInfo)
  REQUIRE(FieldInfo)
  REQUIRE(LocalGoalModel)
  REQUIRE(SoccerStrategy)
  REQUIRE(MotionStatus)
  REQUIRE(CompassDirection)
  REQUIRE(ActionModel)

  PROVIDE(SituationStatus)
  PROVIDE(DebugRequest)
  PROVIDE(DebugDrawings)
  PROVIDE(DebugModify)
  PROVIDE(DebugPlot)
END_DECLARE_MODULE(StrategySymbols)

class StrategySymbols: public StrategySymbolsBase
{

public:
  StrategySymbols()
  : 
    attackDirection(0.0),
    attackDirectionPreviewHip(0.0),
    attackDirectionPreviewLFoot(0.0),
    attackDirectionPreviewRFoot(0.0)
  {
    theInstance = this;
  }
  virtual ~StrategySymbols(){}
  
  /** registers the symbols at an engine */
  void registerSymbols(xabsl::Engine& engine);

  // this is excuted before any symbol is used
  virtual void execute();

private:

  static StrategySymbols* theInstance;

  // calculate the best foot to approach the ball with
  // true if the right foot ich choosen, false else
  static bool getApproachingWithRightFoot();

  // team stuff S3D
  static double getOppClosestToBallNumber();
  static double getOppClosestToBallTimeSinceLastSeen();
  static double getOppClosestToBallDistanceToBall();
  static bool getOpponentAtTheBall();

  static double getOwnClosestToBallNumber();
  static double getOwnClosestToBallTimeSinceLastSeen();
  static double getOwnClosestToBallDistanceToBall();

  //defender position SPL
  Pose2D calculateSimpleDefensePose();
  Pose2D simpleDefenderPose;
  static double simpleDefensePoseX();
  static double simpleDefensePoseY();
  static double simpleDefensePoseA();

  //find out if the robot is in a certain field half
  static bool getSituationStatusOwnHalf();
  static bool getSituationStatusOppHalf();
  
  //force selflocator to locate in a certain field half
  static void setSituationStatusOwnHalf(bool ownHalf);
  static void setSituationStatusOppHalf(bool oppHalf);

  Pose2D calculateDefensePose();
  static double defensePoseX();
  static double defensePoseY();
  static double defensePoseA();

  /** attack direction and its previews */
  double attackDirection;
  double attackDirectionPreviewHip;
  double attackDirectionPreviewLFoot;
  double attackDirectionPreviewRFoot;

  Vector2d calculateGoalieGuardPosition();
  Vector2d calculatePenaltyGoalieGuardPosition();
  static double goalieGuardPositionX();
  static double goalieGuardPositionY();
  static double penaltyGoalieGuardPositionX();
  static double penaltyGoalieGuardPositionY();

  static int getBestAction();

};//end class StrategySymbols

#endif // _StrategySymbols_H_
