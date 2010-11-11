/**
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 * The Platform singleton holds all objects about platform
 */

#ifndef _PLATFORM_H
#define	_PLATFORM_H

#ifndef WIN32
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <net/if.h>
#endif // undef WIN32

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
      _bodyID("Uninitialized"),
      _bodyNickName("Uninitialized"),
      _platformInterface(NULL),
      //theConfigPathInfo(_configPathInfo),
      theHardwareIdentity(_hardwareIdentity),
      theBodyID(_bodyID),
      theBodyNickName(_bodyNickName),
      thePlatformInterface(_platformInterface)
    {
    }

    // cannot be copied
    Platform& operator=( const Platform& ) {}

    string _hardwareIdentity;
    string _bodyID;
    string _bodyNickName;
    Configuration _configuration;
    PlatformBase* _platformInterface;

  public:
    virtual ~Platform()
    {
    }

/* Mac & Windows dont have that define */
#ifdef SIOCGIFHWADDR
  // get Media Access Control address
  static std::string getMACaddress(const std::string& name)
  {
    int s;
    struct ifreq buffer;
    s = socket(PF_INET, SOCK_DGRAM, 0);
    memset(&buffer, 0x00, sizeof (buffer));
    strcpy(buffer.ifr_name, name.c_str());
    ioctl(s, SIOCGIFHWADDR, &buffer);
    close(s);
    char mac[18];
    sprintf(mac, "%.2X_%.2X_%.2X_%.2X_%.2X_%.2X",
      (unsigned char) buffer.ifr_hwaddr.sa_data[0],
      (unsigned char) buffer.ifr_hwaddr.sa_data[1],
      (unsigned char) buffer.ifr_hwaddr.sa_data[2],
      (unsigned char) buffer.ifr_hwaddr.sa_data[3],
      (unsigned char) buffer.ifr_hwaddr.sa_data[4],
      (unsigned char) buffer.ifr_hwaddr.sa_data[5]);

    return std::string(mac);
  }//end getMACaddress
#else
  // do nothing...
  static std::string getMACaddress(const std::string& /*name*/)
  {
    return "unknown mac address";
  }//end getMACaddress
#endif


    void init(PlatformBase* _interface)
    {
      ASSERT(_interface!=NULL);
      // TODO: check the communication?
      //ASSERT(comm!=NULL);
      _platformInterface = _interface;

      // set the the hardware identity according to platform
      _hardwareIdentity = _interface->getHardwareIdentity();
      _bodyID = _interface->getBodyID();
      _bodyNickName = _interface->getBodyNickName();
      std::string configDir = "Config/";
      if(getenv("NAO_CONFIG") != NULL)
      {
        configDir = getenv("NAO_CONFIG");
      }
      _configuration.loadFromDir(configDir, _interface->getName(), _hardwareIdentity);
      std::cout << "test configuration: " << _configuration.getString("test", "test") << std::endl;
  //    // init the camera info
  //    theCameraInfo.init(theConfigPathInfo.camera_parameter + "/camera_info_" + theHardwareIdentity + ".prm");
  //    theMassConfig = ConfigLoader::loadConfig(theConfigPathInfo.mass_info.c_str());
    }//end init

  //  const ConfigPathInfo& theConfigPathInfo;
    const string& theHardwareIdentity; // the string to indentify different robots
    const string& theBodyID;
    const string& theBodyNickName;
    CameraInfoParameter theCameraInfo;
  //  Config theMassConfig;

    PlatformBase* const & thePlatformInterface;

  };
}
#endif	/* _PLATFORM_H */

