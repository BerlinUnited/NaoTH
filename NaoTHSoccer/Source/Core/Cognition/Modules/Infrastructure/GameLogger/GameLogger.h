#ifndef GAMELOGGER_H
#define GAMELOGGER_H

#include <ModuleFramework/Module.h>
#include <Tools/Logfile/LogfileManager.h>

#include <Representations/Infrastructure/FrameInfo.h>
#include <Representations/Modeling/BehaviorStatus.h>

using namespace naoth;

BEGIN_DECLARE_MODULE(GameLogger)
  REQUIRE(FrameInfo)
  REQUIRE(BehaviorStatus)
END_DECLARE_MODULE(GameLogger)

class GameLogger : public GameLoggerBase
{
public:
  GameLogger();
  virtual ~GameLogger();

  virtual void execute();

private:
  LogfileManager < 30 > logfileManager;
};

#endif // GAMELOGGER_H
