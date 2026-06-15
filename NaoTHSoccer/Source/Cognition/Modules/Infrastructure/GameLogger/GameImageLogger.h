#ifndef GAME_IMAGE_LOGGER_H
#define GAME_IMAGE_LOGGER_H

#include <future>
#include <chrono>
using namespace std::chrono_literals;

#include <ModuleFramework/Module.h>
#include <Tools/Logfile/LogfileManager.h>

#include <Representations/Modeling/PlayerInfo.h>
#include "Representations/Motion/MotionStatus.h"

#include <Representations/Infrastructure/FrameInfo.h>
#include <Representations/Infrastructure/Image.h>
#include <Representations/Infrastructure/ImageJPEG.h>

// tools
#include "Tools/Debug/DebugParameterList.h"

using namespace naoth;

BEGIN_DECLARE_MODULE(GameImageLogger)
  PROVIDE(DebugParameterList)

  REQUIRE(FrameInfo)

  REQUIRE(PlayerInfo)
  REQUIRE(MotionStatus)

  PROVIDE(Image)
  PROVIDE(ImageTop)

  PROVIDE(ImageJPEG)
  PROVIDE(ImageJPEGTop)

END_DECLARE_MODULE(GameImageLogger)


class GameImageLogger : public GameImageLoggerBase
{
  
private:
  struct Parameters: public ParameterList
  {
    Parameters() : ParameterList("GameImageLogger")
    {
      PARAMETER_REGISTER(logJPEGImages) = false;
      
      // ACHTUNG: if you change this the collect stick needs to change as well
      PARAMETER_REGISTER(imageLogPath) = "/home/nao/images_jpeg.log";
      
      syncWithConfig();
    }

    bool logJPEGImages;
    std::string imageLogPath;
  } params;  

public:
  GameImageLogger() 
  {
    getImageJPEG().linkTo(getImage());
    getImageJPEGTop().linkTo(getImageTop());

    // NOTE: legacy code for reference and debugging
    //const std::string imageLogPath = "/home/nao/images_jpeg.log";
    //imageOutFile.open(imageLogPath, std::ios::out | std::ios::binary);
    
    logfileManager.openFile(params.imageLogPath);

    getDebugParameterList().add(&params);
  }
  
  virtual ~GameImageLogger() {
    logfileManager.closeFile();
    // NOTE: legacy code for reference and debugging
    //imageOutFile.close();
    getDebugParameterList().remove(&params);
  }

#define LOGSTUFF(name) \
  { std::ostream& dataStream = logfileManager.log(getFrameInfo().getFrameNumber(), #name); \
  Serializer<name>::serialize(get##name(), dataStream); } ((void)0)

  virtual void execute()
  {
    getImageJPEG().compressImageAsync();
    getImageJPEGTop().compressImageAsync();
    
    // HACK: wait a bit before starting recording
    if(!logfileManager.is_ready()) {
      return;
    }

    // make sure the previous call is processed
    if(process.valid()) {
      // wait for max 5 ms
      if ( process.wait_for(5ms) != std::future_status::ready ) {
        // we have to skip this frame and wait until the last one if finised
        return;
      }
    }

    // ignore some states
    bool log_this_frame = true;
    log_this_frame = log_this_frame && getPlayerInfo().robotState != PlayerInfo::initial;
    log_this_frame = log_this_frame && getPlayerInfo().robotState != PlayerInfo::finished;
    log_this_frame = log_this_frame && getPlayerInfo().robotState != PlayerInfo::unstiff;
    log_this_frame = log_this_frame && getMotionStatus().currentMotion != motion::init;

    if(params.logJPEGImages && log_this_frame) 
    {
      process = std::async([&] 
      {
        LOGSTUFF(FrameInfo);
        LOGSTUFF(ImageJPEG);
        LOGSTUFF(ImageJPEGTop);
      });
    }
  }

private:
  // TODO: make a memory aware LogfileManager that flushes whenever a certain memory
  // treshold is reached.
  LogfileManager logfileManager;

  std::ofstream imageOutFile;
  FrameInfo lastTimeImageRecorded;

  std::future<void> process;
};

#endif // GAME_IMAGE_LOGGER_H
