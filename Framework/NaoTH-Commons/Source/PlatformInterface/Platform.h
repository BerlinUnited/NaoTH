/**
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 * The Platform singleton holds all objects about platform
 */

#ifndef _PLATFORM_H
#define  _PLATFORM_H

#include <string>
#include <iostream>
#include <fstream>

#include <string.h>
#include "PlatformInterface.h"
#include "Tools/DataStructures/Singleton.h"
#include "Representations/Infrastructure/CameraInfo.h"
#include "Representations/Infrastructure/Configuration.h"

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
      _platformInterface(NULL),
      theConfiguration(_configuration),
      theConfigDirectory(_configDir),
      theHardwareIdentity(_hardwareIdentity),
      theHeadHardwareIdentity(_headHardwareIdentity),
      theScheme(_scheme),
      thePlatformInterface(_platformInterface)
    {
      _configDir = "Config/";
    }

    // cannot be copied
    Platform& operator=( const Platform& ) { return *this; }

    std::string _hardwareIdentity;
    std::string _headHardwareIdentity;
    Configuration _configuration;
    std::string _configDir;
    std::string _scheme;
    PlatformBase* _platformInterface;

  public:
    virtual ~Platform()
    {
    }

    void init(PlatformBase* _interface)
    {
      ASSERT(_interface!=NULL);

      _platformInterface = _interface;

      // set the the hardware identity according to platform
      _hardwareIdentity = _interface->getBodyNickName();
      _headHardwareIdentity = _interface->getHeadNickName();
      _scheme = _interface->getName(); // set to platform by default
      std::ifstream schemefile((_configDir + "scheme.cfg").c_str());
      if(schemefile.is_open() && schemefile.good())
      {
        std::getline(schemefile, _scheme);
      }
      
      _configuration.loadFromDir(_configDir, _scheme, _hardwareIdentity, _headHardwareIdentity);
    }//end init

    Configuration& theConfiguration;
    const string& theConfigDirectory;
    const string& theHardwareIdentity; // the string to indentify different robots
    const string& theHeadHardwareIdentity; // the string to indentify different robot heads
    const string& theScheme;

    PlatformBase* const & thePlatformInterface;

  };
}
#endif  /* _PLATFORM_H */

