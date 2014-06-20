#include "GameLogger.h"

GameLogger::GameLogger()
  : logfileManager(true)
{
  logfileManager.openFile("game.log");
}

void GameLogger::execute()
{

  if(getBehaviorStateComplete().state.IsInitialized() &&
          getBehaviorStateSparse().state.IsInitialized())
  {
      std::stringstream& streamFrameInfo =
          logfileManager.log(getFrameInfo().getFrameNumber(), "FrameInfo");
      Serializer<FrameInfo>::serialize(getFrameInfo(), streamFrameInfo);

      // write out the complete behavior state when it was just created in this frame
      if(getBehaviorStateComplete().state.framenumber() == getFrameInfo().getFrameNumber())
      {
        std::stringstream& streamBehavior =
          logfileManager.log(getFrameInfo().getFrameNumber(), "BehaviorStateComplete");
        Serializer<BehaviorStateComplete>::serialize(getBehaviorStateComplete(), streamBehavior);
      }

      // always write out the sparse state
      std::stringstream& streamBehavior =
        logfileManager.log(getFrameInfo().getFrameNumber(), "BehaviorStateSparse");
      Serializer<BehaviorStateSparse>::serialize(getBehaviorStateSparse(), streamBehavior);

  }
}

GameLogger::~GameLogger()
{
  logfileManager.closeFile();
}
