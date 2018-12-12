#ifndef GAMELOGGER_H
#define GAMELOGGER_H

#include <ModuleFramework/Module.h>
#include <Tools/Logfile/LogfileManager.h>

#include <Representations/Infrastructure/FrameInfo.h>
#include <Representations/Infrastructure/Image.h>
#include <Representations/Infrastructure/RobotInfo.h>
#include <Representations/Modeling/BehaviorStateComplete.h>
#include <Representations/Modeling/BehaviorStateSparse.h>
#include <Representations/Modeling/PlayerInfo.h>

#include <Representations/Perception/BallPercept.h>
#include <Representations/Perception/GoalPercept.h>
#include <Representations/Perception/ScanLineEdgelPercept.h>
#include <Representations/Modeling/OdometryData.h>
#include <Representations/Perception/CameraMatrix.h>
#include "Representations/Modeling/TeamMessage.h"
#include "Representations/Modeling/BodyStatus.h"
#include "Representations/Motion/MotionStatus.h"

#include "Representations/Perception/BallCandidates.h"
#include "Representations/Perception/MultiBallPercept.h"

#include <Representations/Infrastructure/AudioData.h>
#include "Representations/Infrastructure/WhistlePercept.h"

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
  REQUIRE(BallPercept)
  REQUIRE(BallPerceptTop)
  REQUIRE(ScanLineEdgelPercept)
  REQUIRE(ScanLineEdgelPerceptTop)
  REQUIRE(BodyStatus)
  REQUIRE(MotionStatus)

  REQUIRE(AudioData)

  REQUIRE(MultiBallPercept)

  REQUIRE(BallCandidates)
  REQUIRE(BallCandidatesTop)

  REQUIRE(TeamMessage)

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
      PARAMETER_REGISTER(logBallCandidates) = false;
      PARAMETER_REGISTER(logBodyStatus) = false;
      PARAMETER_REGISTER(logPlainImages) = false;
      PARAMETER_REGISTER(logPlainImagesDelay) = 2000;
      syncWithConfig();
    }

    bool logAudioData;
    bool logBallCandidates;
    bool logBodyStatus;
    bool logPlainImages;
    int logPlainImagesDelay;
  } params;

private:
  // TODO: make a memory aware LogfileManager that flushes whenever a certain memory
  // treshold is reached.
  LogfileManager < 30 > logfileManager;
  
  std::ofstream imageOutFile;
  FrameInfo lastTimeImageRecorded;

  unsigned int lastCompleteFrameNumber;
  
  PlayerInfo::RobotState oldState;
  bool firstRecording;
  int lastWhistleCounter;
  unsigned long lastAudioDataTimestamp;

  CameraInfo::CameraID lastRecordedPlainImageID;
};

#endif // GAMELOGGER_H
