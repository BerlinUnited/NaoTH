#ifndef GAME_IMAGE_LOGGER_H
#define GAME_IMAGE_LOGGER_H

#include <future>
#include <chrono>
using namespace std::chrono_literals;

#include <ModuleFramework/Module.h>
#include <Tools/Logfile/LogfileManager.h>

#include <Representations/Infrastructure/FrameInfo.h>
#include <Representations/Infrastructure/Image.h>
#include <Representations/Infrastructure/ImageJPEG.h>

// tools
#include "Tools/Debug/DebugParameterList.h"

using namespace naoth;

BEGIN_DECLARE_MODULE(GameImageLogger)
  PROVIDE(DebugParameterList)

  REQUIRE(FrameInfo)

  REQUIRE(Image)
  REQUIRE(ImageTop)
  REQUIRE(ImageJPEG)
  REQUIRE(ImageJPEGTop)

END_DECLARE_MODULE(GameImageLogger)

class GameImageLogger : public GameImageLoggerBase
{
public:
  GameImageLogger() {
    const std::string imageLogPath = "/home/nao/images_jpeg.log";
    //imageOutFile.open(imageLogPath, std::ios::out | std::ios::binary);

    logfileManager.openFile(imageLogPath);

    getDebugParameterList().add(&params);
  }
  
  virtual ~GameImageLogger() {
    logfileManager.closeFile();
    //imageOutFile.close();
    getDebugParameterList().remove(&params);
  }

#define LOGSTUFF(name) \
  { std::ostream& dataStream = logfileManager.log(getFrameInfo().getFrameNumber(), #name); \
  Serializer<name>::serialize(get##name(), dataStream); } ((void)0)

  virtual void execute()
  {
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

    if(params.logJPEGImages) 
    {
      process = std::async([&] 
      {
        unsigned int frameNumber = getFrameInfo().getFrameNumber();

        auto a_bot = std::async(std::launch::deferred, [&]{ getImageJPEG().compressYUYV(); } );
        auto a_top = std::async(std::launch::deferred, [&]{ getImageJPEGTop().compressYUYV(); } );
        
        LOGSTUFF(FrameInfo);

        a_bot.wait();
        LOGSTUFF(ImageJPEG);

        // first image: bottom
        //imageOutFile.write((const char*)(&frameNumber), sizeof(unsigned int));
        //imageOutFile.write((const char*)getImageJPEG().getJPEG(), getImageJPEG().getJPEGSize());
        

        a_top.wait();
        LOGSTUFF(ImageJPEGTop);

        // second image: top
        //imageOutFile.write((const char*)(&frameNumber), sizeof(unsigned int));
        //imageOutFile.write((const char*)getImageJPEGTop().getJPEG(), getImageJPEGTop().getJPEGSize());

        //imageOutFile.flush();
      });
    }
  }

private:
  struct Parameters: public ParameterList
  {
    Parameters() : ParameterList("GameImageLogger")
    {
      PARAMETER_REGISTER(logJPEGImages) = false;
      PARAMETER_REGISTER(logPlainImages) = true;
      PARAMETER_REGISTER(logPlainImagesDelay) = 2000; // ms
      syncWithConfig();
    }

    bool logJPEGImages;
    bool logPlainImages;
    int logPlainImagesDelay;
  } params;

private:
  // TODO: make a memory aware LogfileManager that flushes whenever a certain memory
  // treshold is reached.
  LogfileManager logfileManager;

  std::ofstream imageOutFile;
  FrameInfo lastTimeImageRecorded;

  std::future<void> process;
};

#endif // GAME_IMAGE_LOGGER_H
