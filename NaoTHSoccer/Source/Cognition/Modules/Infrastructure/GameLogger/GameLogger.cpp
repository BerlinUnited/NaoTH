
#include "GameLogger.h"

using namespace std;

GameLogger::GameLogger()
  : 
  lastCompleteFrameNumber(0),
  oldState(PlayerInfo::initial),
  firstRecording(true),
  lastAudioDataTimestamp(0)
{
  // CAUTION: if you change this the collect stick needs to change as well
  const std::string gameLogPath = "/home/nao/game.log";
  const std::string imageLogPath = "/home/nao/images.log";

  logfileManager.openFile(gameLogPath);
  
  imageOutFile.open(imageLogPath, ios::out | ios::binary);
  lastTimeImageRecorded = getFrameInfo();

  getDebugParameterList().add(&params);
}

GameLogger::~GameLogger()
{
  logfileManager.closeFile();
  imageOutFile.close();
  getDebugParameterList().remove(&params);
}

#define LOGSTUFF(name) \
  { std::ostream& dataStream = logfileManager.log(getFrameInfo().getFrameNumber(), #name); \
  Serializer<name>::serialize(get##name(), dataStream); } ((void)0)

void GameLogger::execute()
{
  // HACK: wait a bit before starting recording
  if(!logfileManager.is_ready()) {
    return;
  }

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

    // condition wheather the current frame should be logged:
    bool log_this_frame = (getBehaviorStateSparse().state.framenumber() == getFrameInfo().getFrameNumber());

    // NOTE: record only the first frame if the state changed to initial or finished
    if(!firstRecording && oldState == getPlayerInfo().robotState) {
      log_this_frame = log_this_frame && getPlayerInfo().robotState != PlayerInfo::initial;
      log_this_frame = log_this_frame && getPlayerInfo().robotState != PlayerInfo::finished;
      log_this_frame = log_this_frame && getPlayerInfo().robotState != PlayerInfo::unstiff;
      log_this_frame = log_this_frame && getMotionStatus().currentMotion != motion::init;
    }

    if(log_this_frame)
    {
      LOGSTUFF(BehaviorStateSparse);

      // proprioception
      LOGSTUFF(OdometryData);
      LOGSTUFF(CameraMatrix);
      LOGSTUFF(CameraMatrixTop);

      if(params.logBodyStatus) {
        LOGSTUFF(BodyStatus);
      }

      // perception
      LOGSTUFF(GoalPercept);
      LOGSTUFF(GoalPerceptTop);

      LOGSTUFF(MultiBallPercept);
      LOGSTUFF(BallModel);
      
      if(params.logUltraSound) {
        LOGSTUFF(UltraSoundReceiveData);
      }

      LOGSTUFF(FieldPercept);
      LOGSTUFF(FieldPerceptTop);

      LOGSTUFF(ScanLineEdgelPercept);
      LOGSTUFF(ScanLineEdgelPerceptTop);
      LOGSTUFF(ShortLinePercept);
      LOGSTUFF(RansacLinePercept);
      LOGSTUFF(RansacCirclePercept2018);
      
      if(params.logJPEGImages) {
        LOGSTUFF(ImageJPEG);
        LOGSTUFF(ImageJPEGTop);
      }
      
      if(params.logBallCandidates) {
        LOGSTUFF(BallCandidates);
        LOGSTUFF(BallCandidatesTop);
      }

      LOGSTUFF(TeamState);
      LOGSTUFF(TeamMessageDecision);


      // keep the audio device open for some time

      if(params.logAudioData) 
      {
        // remember when the capture was on and keep recording for some time after the behavior says stop recording
        if(getAudioControl().capture) {
          timeOfLastCapture = getFrameInfo();
        } else if(getFrameInfo().getTimeSince(timeOfLastCapture.getTime()) < 3000) {
          getAudioControl().capture = true;
        }

        // new data avaliable and capture is still on
        if(lastAudioDataTimestamp < getAudioData().timestamp) {
          LOGSTUFF(AudioData);
          lastAudioDataTimestamp = getAudioData().timestamp;
        }
      }

      
      
      if (getWhistlePercept().whistleDetected) {
        LOGSTUFF(WhistlePercept);
      }

      // record images every n seconds
      if(params.logPlainImages && getFrameInfo().getTimeSince(lastTimeImageRecorded) > params.logPlainImagesDelay && imageOutFile.is_open() && !imageOutFile.fail())
      {
        unsigned int frameNumber = getFrameInfo().getFrameNumber();

        // first image: bottom
        imageOutFile.write((const char*)(&frameNumber), sizeof(unsigned int));
        imageOutFile.write((const char*)getImage().data(), getImage().data_size());

        // second image: top
        imageOutFile.write((const char*)(&frameNumber), sizeof(unsigned int));
        imageOutFile.write((const char*)getImageTop().data(), getImageTop().data_size());

        lastTimeImageRecorded = getFrameInfo();
      }

      something_recorded = true;
    }

    if(something_recorded) {
      LOGSTUFF(FrameInfo);
      firstRecording = false;
    }

    // remember the old state
    oldState = getPlayerInfo().robotState;
  }
}

