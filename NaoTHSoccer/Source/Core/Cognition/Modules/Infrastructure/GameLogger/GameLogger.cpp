#include "GameLogger.h"

GameLogger::GameLogger()
{
  logfileManager.openFile("/home/nao/game.log");
}

void GameLogger::execute()
{
  std::stringstream& stream =
      logfileManager.log(getFrameInfo().getFrameNumber(), "BehaviorStatus");
  Serializer<BehaviorStatus>::serialize(getBehaviorStatus(), stream);
}

GameLogger::~GameLogger()
{
  logfileManager.closeFile();
}
