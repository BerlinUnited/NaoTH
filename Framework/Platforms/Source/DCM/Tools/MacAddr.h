/**
 * @file MacAddr.h
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 * get the mac address
 */

#ifndef _MAC_ADDR_H_
#define _MAC_ADDR_H_

#ifndef WIN32
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <net/if.h>
#endif // undef WIN32

namespace naoth
{
  
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
    return "unknown_mac_address";
  }//end getMACaddress
#endif

}

#endif //_MAC_ADDR_H_
