#ifndef __StrategySymbols_H_
#define __StrategySymbols_H_

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
#include "Representations/Motion/MotionStatus.h"

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
END_DECLARE_MODULE(StrategySymbols)

class StrategySymbols: public StrategySymbolsBase
{

public:
  StrategySymbols()
  : 
    attentionModel(getAttentionModel()),
    playersModel(getPlayersModel()),
    frameInfo(getFrameInfo()),
    ballModel(getBallModel()),
    robotPose(getRobotPose()),
    goalModel(getSelfLocGoalModel()),
    playerInfo(getPlayerInfo()),
    fieldInfo(getFieldInfo()),
    localGoalModel(getLocalGoalModel()),
    soccerStrategy(getSoccerStrategy()),
    motionStatus(getMotionStatus()),

    goalKickPlayer(0.0),
    goalKickPositionLeft(false)
  {
    theInstance = this;
  };
  ~StrategySymbols(){};
  
  /** registers the symbols at an engine */
  void registerSymbols(xabsl::Engine& engine);

  virtual void execute();

private:

  static StrategySymbols* theInstance;

  // representations
  AttentionModel const& attentionModel;
  PlayersModel const& playersModel;
  FrameInfo const& frameInfo;
  BallModel const& ballModel;
  RobotPose const& robotPose;
  SelfLocGoalModel const& goalModel;
  PlayerInfo const& playerInfo;
  FieldInfo const& fieldInfo;
  LocalGoalModel const& localGoalModel;
  SoccerStrategy const& soccerStrategy;
  MotionStatus const& motionStatus;

  // calculate the best foot to approach the ball with
  // true if the right foot ich choosen, false else
  static bool getApproachingWithRightFoot();

  // seter and getter
  static double getOppClosestToBallNumber();
  static double getOppClosestToBallTimeSinceLastSeen();
  static double getOppClosestToBallDistanceToBall();
  static bool getOpponentAtTheBall();

  static double getOwnClosestToBallNumber();
  static double getOwnClosestToBallTimeSinceLastSeen();
  static double getOwnClosestToBallDistanceToBall();

  Pose2D calculateDefensePose();
  static double defensePoseX();
  static double defensePoseY();
  static double defensePoseA();

  bool calculateSetPiecePose();
  void testArrangeRobots();
  void arrangeRobots(
    const std::vector<Vector2<double> >& robots, 
    const std::vector<Vector2<double> >& places, 
    std::vector<int>& placesToRobots );

  static double setpiecePoseX();
  static double setpiecePoseY();
  static bool   setpieceParticipation();
  bool arrange2Robots(Vector2<double> setPiecePoints[2], unsigned int excludePlayer);
  bool arrange3Robots(Vector2<double> setPiecePoints[3], unsigned int excludePlayer);
  Vector2<double> setpiecePosition;
  double goalKickPlayer;
  bool goalKickPositionLeft;

  static Vector2<double> calculatePlayerPotentialField( const Vector2<double>& player, const Vector2<double>& ball);


  /** returns x-position on circle between to points */
  static double circleX();
  /** returns y-position on circle between to points */
  static double circleY();

  Vector2<double> calculateGoalieGuardPosition();
  Vector2<double> calculatePenaltyGoalieGuardPosition();
  static double goalieGuardPositionX();
  static double goalieGuardPositionY();
  static double penaltyGoalieGuardPositionX();
  static double penaltyGoalieGuardPositionY();

  struct CycleParameter
  {
    /** The parameter "circlex.directiony" and "circley.directiony" of the functions "circlex" and "circley" */
    Vector2<double> in_direction;

    /** The parameter "circlex.x" and "circley.x" of the functions "circlex" and "circley" */
    Vector2<double> target;

    /** The parameter "circlex.robot.x" and "circley.robot.x" of the functions "circlex" and "circley" */
    Vector2<double> robot_pose;
  } cycle;

public:
  /** @return the suggested attack direction in local coordinates */
  static double calculateAttackDirection();

};//end class StrategySymbols

#endif // __StrategySymbols_H_
