#include "GameLogger.h"

GameLogger::GameLogger()
  : 
  logfileManager(true),
  lastCompleteFrameNumber(0),
  ignore_init_state(false)
{
  logfileManager.openFile("/tmp/game.log");

  getDebugParameterList().add(&params);
}

GameLogger::~GameLogger()
{
  logfileManager.closeFile();

  getDebugParameterList().remove(&params);
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

    // write out the complete behavior state when it was just created in this frame
    if(getBehaviorStateComplete().state.framenumber() >= lastCompleteFrameNumber)
    {
      LOGSTUFF(BehaviorStateComplete);
      LOGSTUFF(RobotInfo);


      lastCompleteFrameNumber = getFrameInfo().getFrameNumber();
      something_recorded = true;
    }

    // NOTE: don't record if the internal state of the plyer is set to initial
    //       in this case only first frame of the initial-phase is recorded
    if( (getPlayerInfo().robotState != PlayerInfo::initial || !ignore_init_state) && 
        getBehaviorStateSparse().state.framenumber() == getFrameInfo().getFrameNumber())
    {
      LOGSTUFF(BehaviorStateSparse);

      // proprioception
      LOGSTUFF(OdometryData);
      LOGSTUFF(CameraMatrix);
      LOGSTUFF(CameraMatrixTop);
      if(params.logBodyStatus)
      {
        LOGSTUFF(BodyStatus);
      }

      // perception
      LOGSTUFF(GoalPercept);
      LOGSTUFF(GoalPerceptTop);

      LOGSTUFF(MultiBallPercept)
      
      //LOGSTUFF(BallPercept);
      //LOGSTUFF(BallPerceptTop);
      
      LOGSTUFF(ScanLineEdgelPercept);
      LOGSTUFF(ScanLineEdgelPerceptTop);
      
      if(params.logBallCandidates) {
        LOGSTUFF(BallCandidates);
        LOGSTUFF(BallCandidatesTop);
      }

      LOGSTUFF(TeamMessage);

      something_recorded = true;
    }

    if(something_recorded) {
      LOGSTUFF(FrameInfo);
    }

    ignore_init_state = (getPlayerInfo().robotState == PlayerInfo::initial);
  }
}

