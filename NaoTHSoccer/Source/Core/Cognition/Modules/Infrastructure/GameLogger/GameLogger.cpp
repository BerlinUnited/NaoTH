#include "GameLogger.h"

GameLogger::GameLogger()
  : logfileManager(true)
{
  logfileManager.openFile("/home/nao/game.log");
}

void GameLogger::execute()
{
  std::stringstream& streamFrameInfo =
      logfileManager.log(getFrameInfo().getFrameNumber(), "FrameInfo");
  Serializer<FrameInfo>::serialize(getFrameInfo(), streamFrameInfo);

  std::stringstream& streamBehavior =
      logfileManager.log(getFrameInfo().getFrameNumber(), "BehaviorStatus");
  Serializer<BehaviorStatus>::serialize(getBehaviorStatus(), streamBehavior);
}

GameLogger::~GameLogger()
{
  logfileManager.closeFile();
}
