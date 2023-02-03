/**
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 * The Platform singleton holds all objects about platform
 */

#ifndef _PLATFORM_H
#define _PLATFORM_H

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
  : _hardwareIdentity("Uninitialized"),
    _headHardwareIdentity("Uninitialized"),
    _configDir("Config/"),

    theConfigDirectory(_configDir),
    theHardwareIdentity(_hardwareIdentity),
    theHeadHardwareIdentity(_headHardwareIdentity),
    theRobotName(_robotName),
    thePlatform(_platform),
    theScheme(_scheme),
    theStrategy(_strategy)
  {
      // only if the config directory doesn't exists locally ...
      if(!std::filesystem::is_directory(_configDir)) {
          // retrieve the config dir from environment var
          const std::string env = std::getenv("NAOTH_CONFIGDIR") != NULL ? std::string(std::getenv("NAOTH_CONFIGDIR")) : "";
          if(!env.empty()) {
              _configDir = env + (env.back() != '/' ? "/" : "");
          }
      }
  }

  // cannot be copied
  Platform& operator=( const Platform& ) { return *this; }

  void readStringFromFile(std::string file, std::string& str) {
      // try to read the scheme name from file
      std::ifstream fstream((theConfigDirectory + file).c_str());
      if(fstream.is_open() && fstream.good()) {
        std::getline(fstream, str);
      }
  }

  // 
  std::string _hardwareIdentity;
  std::string _headHardwareIdentity;
  std::string _robotName;
  std::string _configDir;
  std::string _platform;
  std::string _scheme;
  std::string _strategy;

public:
  virtual ~Platform(){}

  void init(PlatformBase* base)
  {
    ASSERT(base != NULL);

    // set the the hardware identity according to platform
    _hardwareIdentity = base->getBodyNickName();
    _headHardwareIdentity = base->getHeadNickName();
    _robotName = base->getRobotName();
    _platform = base->getPlatformName(); // set to platform by default
    _scheme = ""; // empty to mark as "no-scheme"
    _strategy = ""; // empty to mark as "default-strategy"

    readStringFromFile("scheme.cfg", _scheme);
    readStringFromFile("strategy.cfg", _strategy);
      
    // load config
    theConfiguration.loadFromDir(
      theConfigDirectory, thePlatform, theScheme, theStrategy, theHardwareIdentity, theHeadHardwareIdentity, theRobotName);
  }//end init

  // NOTE: the identity of the robot is defined by the configuration
  // const accessors
  const std::string& theConfigDirectory;
  const std::string& theHardwareIdentity; // the string to indentify different robots
  const std::string& theHeadHardwareIdentity; // the string to indentify different robot heads
  const std::string& theRobotName;
  const std::string& thePlatform;
  const std::string& theScheme;
  const std::string& theStrategy;


  Configuration theConfiguration;
};
}
#endif  /* _PLATFORM_H */

