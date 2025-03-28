/**
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 * The Platform singleton holds all objects about platform
 */

#ifndef PLATFORM_H
#define PLATFORM_H

#include "PlatformBase.h"
#include <Tools/DataStructures/Singleton.h>
#include <Tools/Debug/NaoTHAssert.h>
#include <Representations/Infrastructure/Configuration.h>

#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>

namespace naoth
{

class Platform : public Singleton<Platform>
{
protected:
  friend class Singleton<Platform>;

private:
  Platform()
  {
    configPaths.directory = "Config/";

    // only if the config directory doesn't exists locally ...
    if(!std::filesystem::is_directory(configPaths.directory)) {
      // retrieve the config dir from environment var
      if(const char* env = std::getenv("NAOTH_CONFIGDIR")) {
        configPaths.directory = std::string(env);
        // append a trailing slash if there was none
        configPaths.directory += (configPaths.directory.back() != '/' ? "/" : "");
      }
    }
  }

  // cannot be copied
  Platform& operator=( const Platform& ) { return *this; }

  void readStringFromFile(std::string file_path, std::string& str) {
      // try to read the scheme name from file
      std::ifstream fstream(file_path);
      if(fstream.is_open() && fstream.good()) {
        fstream >> str;
      }
  }

public:
  virtual ~Platform(){}

  void init(PlatformBase* base)
  {
    ASSERT(base != NULL);

    // set the the hardware identity according to platform
    configPaths.robotName = base->getRobotName();
    configPaths.platform  = base->getPlatformName(); // set to platform by default

    // reading the config structure
    readStringFromFile(configPaths.directory + "competition.cfg", configPaths.competition);
    readStringFromFile(configPaths.directory + "scheme.cfg",      configPaths.scheme);
    readStringFromFile(configPaths.directory + "strategy.cfg",    configPaths.strategy);
      

    // TODO: load config from separate folders
    // load config
    theConfiguration.loadFromDir(
      configPaths.directory, 
      configPaths.platform, 
      configPaths.competition, 
      configPaths.scheme, 
      configPaths.strategy, 
      configPaths.robotName);

  }// end init

private:

  struct ConfigutationPaths {
    std::string directory;

    std::string competition;
    std::string scheme;
    std::string strategy;

    // robot info
    std::string platform;
    std::string robotName;
  } configPaths;

public: // configuration
  Configuration theConfiguration;

  const ConfigutationPaths& getConfigPaths() const {
    return configPaths;
  }

};
}
#endif  /* PLATFORM_H */

