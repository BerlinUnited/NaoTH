#ifndef GAMELOGGER_H
#define GAMELOGGER_H

#include <ModuleFramework/Module.h>
#include <Tools/Logfile/LogfileManager.h>

#include <Representations/Infrastructure/FrameInfo.h>
#include <Representations/Modeling/BehaviorStateComplete.h>
#include <Representations/Modeling/BehaviorStateSparse.h>
#include <Representations/Modeling/PlayerInfo.h>

#include <Representations/Perception/BallPercept.h>
#include <Representations/Perception/GoalPercept.h>
#include <Representations/Perception/ScanLineEdgelPercept.h>
#include <Representations/Modeling/OdometryData.h>
#include <Representations/Perception/CameraMatrix.h>
#include "Representations/Modeling/TeamMessage.h"

using namespace naoth;

BEGIN_DECLARE_MODULE(GameLogger)
  REQUIRE(FrameInfo)
  REQUIRE(PlayerInfo)

  REQUIRE(BehaviorStateSparse)
  REQUIRE(BehaviorStateComplete)

  REQUIRE(OdometryData)
  REQUIRE(CameraMatrix)
  REQUIRE(CameraMatrixTop)
  REQUIRE(GoalPercept)
  REQUIRE(GoalPerceptTop)
  REQUIRE(BallPercept)
  REQUIRE(BallPerceptTop)
  REQUIRE(ScanLineEdgelPercept)
  REQUIRE(ScanLineEdgelPerceptTop)

  REQUIRE(TeamMessage)
END_DECLARE_MODULE(GameLogger)

class GameLogger : public GameLoggerBase
{
public:
  GameLogger();
  virtual ~GameLogger();

  virtual void execute();

private:
  // TODO: make a memory aware LogfileManager that flushes whenever a certain memory
  // treshold is reached.
  LogfileManager < 30 > logfileManager;

  unsigned int lastCompleteFrameNumber;
  bool ignore_init_state;
};

#endif // GAMELOGGER_H
