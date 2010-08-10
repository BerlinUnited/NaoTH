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

#include "PlatformInterface.h"
#include "Tools/DataStructures/Singleton.h"
#include "Representations/Infrastructure/CameraInfo.h"
//#include "Representations/Infrastructure/ConfigPathInfo.h"
#include "Tools/Communication/CommunicationCollection.h"

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
    _commCollection(NULL),
    //theConfigPathInfo(_configPathInfo),
    theHardwareIdentity(_hardwareIdentity),
    theBodyID(_bodyID),
    theBodyNickName(_bodyNickName),
    thePlatformInterface(_platformInterface),
    commCollection(_commCollection)
  {
  }

  string _hardwareIdentity;
  string _bodyID;
  string _bodyNickName;
  //ConfigPathInfo _configPathInfo;
  PlatformInterface* _platformInterface;
  CommunicationCollection* _commCollection;

public:
  virtual ~Platform()
  {
  }

  // get Media Access Control address
  static std::string getMACaddress(const std::string& name)
  {
    /* Mac & Windows dont have that define */
#ifdef SIOCGIFHWADDR
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
#else
    return "unknown mac address";
#endif
  }

  void init(PlatformInterface* _interface, CommunicationCollection* comm)
  {
    ASSERT(_interface!=NULL);
    ASSERT(comm!=NULL);
    _platformInterface = _interface;
    _commCollection = comm;

    // set the the hardware identity according to platform
    _hardwareIdentity = _interface->getHardwareIdentity();
    _bodyID = _interface->getBodyID();
    _bodyNickName = _interface->getBodyNickName();
//    _configPathInfo.loadPathInfo("Config/"+_interface->getName()+"_init.cfg");
//    // init the camera info
//    theCameraInfo.init(theConfigPathInfo.camera_parameter + "/camera_info_" + theHardwareIdentity + ".prm");
//    theMassConfig = ConfigLoader::loadConfig(theConfigPathInfo.mass_info.c_str());
  }

//  const ConfigPathInfo& theConfigPathInfo;
  const string& theHardwareIdentity; // the string to indentify different robots
  const string& theBodyID;
  const string& theBodyNickName;
  CameraInfoParameter theCameraInfo;
//  Config theMassConfig;

  PlatformInterface* const & thePlatformInterface;
  CommunicationCollection* const & commCollection;

};

#endif	/* _PLATFORM_H */

