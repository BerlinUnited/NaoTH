#ifndef GAMELOGGER_H
#define GAMELOGGER_H

#include <ModuleFramework/Module.h>
#include <Tools/Logfile/LogfileManager.h>

#include <Representations/Infrastructure/FrameInfo.h>
#include <Representations/Modeling/BehaviorStateComplete.h>
#include <Representations/Modeling/BehaviorStateSparse.h>

using namespace naoth;

BEGIN_DECLARE_MODULE(GameLogger)
  REQUIRE(FrameInfo)
  REQUIRE(BehaviorStateSparse)
  REQUIRE(BehaviorStateComplete)
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
};

#endif // GAMELOGGER_H
