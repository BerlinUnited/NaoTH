#include "GameLogger.h"

GameLogger::GameLogger()
  : 
  logfileManager(true),
  lastCompleteFrameNumber(0),
  ignore_init_state(false)
{
  logfileManager.openFile("/tmp/game.log");
}

#define LOGSTUFF(name) \
  { std::stringstream& streamBehavior = logfileManager.log(getFrameInfo().getFrameNumber(), #name); \
  Serializer<name>::serialize(get##name(), streamBehavior); }

void GameLogger::execute()
{

  if( getBehaviorStateComplete().state.IsInitialized() &&
      getBehaviorStateSparse().state.IsInitialized())
  {
    bool something_recorded = false;
    //std::stringstream& streamFrameInfo = logfileManager.log(getFrameInfo().getFrameNumber(), "FrameInfo");
    //Serializer<FrameInfo>::serialize(getFrameInfo(), streamFrameInfo);

    // write out the complete behavior state when it was just created in this frame
    if(getBehaviorStateComplete().state.framenumber() >= lastCompleteFrameNumber)
    {
      //std::stringstream& streamBehavior = logfileManager.log(getFrameInfo().getFrameNumber(), "BehaviorStateComplete");
      //Serializer<BehaviorStateComplete>::serialize(getBehaviorStateComplete(), streamBehavior);
      LOGSTUFF(BehaviorStateComplete);

      lastCompleteFrameNumber = getFrameInfo().getFrameNumber();
      something_recorded = true;
    }

    // NOTE: don't record if the internal state of the plyer is set to initial
    //       in this case only first frame of the initial-phase is recorded
    if( (getPlayerInfo().gameData.gameState != GameData::initial || !ignore_init_state) && 
        getBehaviorStateSparse().state.framenumber() == getFrameInfo().getFrameNumber())
    {
      /*
      std::stringstream& streamBehavior = logfileManager.log(getFrameInfo().getFrameNumber(), "BehaviorStateSparse");
      Serializer<BehaviorStateSparse>::serialize(getBehaviorStateSparse(), streamBehavior);
      */
      LOGSTUFF(BehaviorStateSparse);

      LOGSTUFF(OdometryData);
      LOGSTUFF(CameraMatrix);
      LOGSTUFF(CameraMatrixTop);
      LOGSTUFF(GoalPercept);
      LOGSTUFF(GoalPerceptTop);
      LOGSTUFF(BallPercept);
      LOGSTUFF(BallPerceptTop);
      LOGSTUFF(ScanLineEdgelPercept);
      LOGSTUFF(ScanLineEdgelPerceptTop);

      LOGSTUFF(TeamMessage);

      something_recorded = true;
    }

    if(something_recorded) {
      LOGSTUFF(FrameInfo);
    }

    ignore_init_state = (getPlayerInfo().gameData.gameState == GameData::initial);
  }
}

GameLogger::~GameLogger()
{
  logfileManager.closeFile();
}
