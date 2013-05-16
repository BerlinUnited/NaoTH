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
    theScheme(_scheme)
  {}

  // cannot be copied
  Platform& operator=( const Platform& ) { return *this; }

  // 
  std::string _hardwareIdentity;
  std::string _headHardwareIdentity;
  std::string _configDir;
  std::string _scheme;

public:
  virtual ~Platform(){}

  void init(PlatformBase* base)
  {
    ASSERT(base != NULL);

    // set the the hardware identity according to platform
    _hardwareIdentity = base->getBodyNickName();
    _headHardwareIdentity = base->getHeadNickName();
    _scheme = base->getName(); // set to platform by default
      
    // try to read the scheme name from file
    std::ifstream schemefile((theConfigDirectory + "scheme.cfg").c_str());
    if(schemefile.is_open() && schemefile.good()) {
      std::getline(schemefile, _scheme); 
    }
      
    // load config
    theConfiguration.loadFromDir(
      theConfigDirectory, theScheme, theHardwareIdentity, theHeadHardwareIdentity);
  }//end init

  // const accessors
  const std::string& theConfigDirectory;
  const std::string& theHardwareIdentity; // the string to indentify different robots
  const std::string& theHeadHardwareIdentity; // the string to indentify different robot heads
  const std::string& theScheme;


  Configuration theConfiguration;
};
}
#endif  /* _PLATFORM_H */

