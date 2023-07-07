#ifndef GAMELOGGER_H
#define GAMELOGGER_H

#include <ModuleFramework/Module.h>
#include <Tools/Logfile/LogfileManager.h>

#include <Representations/Infrastructure/FrameInfo.h>
#include <Representations/Infrastructure/Image.h>
#include <Representations/Infrastructure/UltraSoundData.h>
#include <Representations/Infrastructure/RobotInfo.h>
#include <Representations/Modeling/BehaviorStateComplete.h>
#include <Representations/Modeling/BehaviorStateSparse.h>
#include <Representations/Modeling/PlayerInfo.h>

#include <Representations/Perception/GoalPercept.h>
#include <Representations/Perception/ScanLineEdgelPercept.h>
#include <Representations/Perception/LinePercept2018.h>
#include <Representations/Perception/FieldPercept.h>

#include <Representations/Modeling/OdometryData.h>
#include <Representations/Perception/CameraMatrix.h>
#include "Representations/Modeling/TeamState.h"
#include "Representations/Modeling/TeamMessageDecision.h"
#include "Representations/Modeling/BodyStatus.h"
#include "Representations/Motion/MotionStatus.h"
#include "Representations/Modeling/BallModel.h"

#include "Representations/Perception/BallCandidates.h"
#include "Representations/Perception/MultiBallPercept.h"

#include <Representations/Infrastructure/AudioData.h>
#include <Representations/Infrastructure/AudioControl.h>
#include "Representations/Perception/WhistlePercept.h"

// tools
#include "Tools/Debug/DebugParameterList.h"

using namespace naoth;

BEGIN_DECLARE_MODULE(GameLogger)
  PROVIDE(DebugParameterList)

  REQUIRE(FrameInfo)
  REQUIRE(PlayerInfo)
  REQUIRE(RobotInfo)

  REQUIRE(BehaviorStateSparse)
  REQUIRE(BehaviorStateComplete)

  REQUIRE(Image)
  REQUIRE(ImageTop)

  REQUIRE(OdometryData)
  REQUIRE(CameraMatrix)
  REQUIRE(CameraMatrixTop)
  REQUIRE(GoalPercept)
  REQUIRE(GoalPerceptTop)

  REQUIRE(FieldPercept)
  REQUIRE(FieldPerceptTop)

  REQUIRE(ScanLineEdgelPercept)
  REQUIRE(ScanLineEdgelPerceptTop)

  REQUIRE(RansacLinePercept)
  REQUIRE(ShortLinePercept)
  REQUIRE(RansacCirclePercept2018)

  REQUIRE(BodyStatus)
  REQUIRE(MotionStatus)

  REQUIRE(UltraSoundReceiveData)

  REQUIRE(MultiBallPercept)
  REQUIRE(BallModel)

  REQUIRE(BallCandidates)
  REQUIRE(BallCandidatesTop)

  REQUIRE(TeamState)
  REQUIRE(TeamMessageDecision)

  REQUIRE(AudioData)
  PROVIDE(AudioControl) // needed to keep the device open for a bit after it's requested to stop

  REQUIRE(WhistlePercept)
END_DECLARE_MODULE(GameLogger)

class GameLogger : public GameLoggerBase
{
public:
  GameLogger();
  virtual ~GameLogger();

  virtual void execute();

private:
  struct Parameters: public ParameterList
  {
    Parameters() : ParameterList("GameLogger")
    {
      PARAMETER_REGISTER(logAudioData) = false;
      PARAMETER_REGISTER(logBallCandidates) = true;
      PARAMETER_REGISTER(logBodyStatus) = false;
      PARAMETER_REGISTER(logPlainImages) = true;
      PARAMETER_REGISTER(logPlainImagesDelay) = 2000; // ms
      PARAMETER_REGISTER(logUltraSound) = false;
      syncWithConfig();
    }

    bool logAudioData;
    bool logBallCandidates;
    bool logBodyStatus;
    bool logPlainImages;
    int logPlainImagesDelay;
    bool logUltraSound;
  } params;

private:
  // TODO: make a memory aware LogfileManager that flushes whenever a certain memory
  // treshold is reached.
  LogfileManager logfileManager;

  std::ofstream imageOutFile;
  FrameInfo lastTimeImageRecorded;

  unsigned int lastCompleteFrameNumber;
  
  PlayerInfo::RobotState oldState;
  bool firstRecording;
  
  // audio data
  unsigned long lastAudioDataTimestamp;
  FrameInfo timeOfLastCapture;
};

#endif // GAMELOGGER_H
