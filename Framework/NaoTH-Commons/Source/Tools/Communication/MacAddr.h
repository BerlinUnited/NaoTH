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
#include <ifaddrs.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#endif // undef WIN32

namespace naoth
{
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
    return "unknow";
#endif
  }//end getMACaddress

  static std::string getIP4(const std::string& name)
  {
    string ip("unknown");

    /* Mac & Windows dont have that define */
#ifdef SIOCGIFHWADDR
    struct ifaddrs * ifAddrStruct=NULL;
    struct ifaddrs * ifa=NULL;
    void * tmpAddrPtr=NULL;

    getifaddrs(&ifAddrStruct);

    for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
      if (ifa ->ifa_addr->sa_family==AF_INET) { // check it is IP4
        // is a valid IP4 Address
        tmpAddrPtr=&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
        char addressBuffer[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
        string n(ifa->ifa_name);
        if ( n == name )
        {
          ip = string(addressBuffer);
        }
      }
    }
    if (ifAddrStruct!=NULL) freeifaddrs(ifAddrStruct);
#endif
    return ip;
  }

  static std::string getBroadcastAddr(const std::string& name)
  {
    string ip("unknown");
    /* Mac & Windows dont have that define */
#ifdef SIOCGIFHWADDR
    struct ifaddrs * ifAddrStruct=NULL;
    struct ifaddrs * ifa=NULL;
    void * tmpAddrPtr=NULL;

    getifaddrs(&ifAddrStruct);

    for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
      if (ifa ->ifa_addr->sa_family==AF_INET) { // check it is IP4
        // is a valid IP4 Address
        tmpAddrPtr=&((struct sockaddr_in *)ifa->ifa_ifu.ifu_broadaddr)->sin_addr;
        char addressBuffer[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
        string n(ifa->ifa_name);
        if ( n == name )
        {
          ip = string(addressBuffer);
        }
      }
    }
    if (ifAddrStruct!=NULL) freeifaddrs(ifAddrStruct);
#endif
    return ip;
  }

} // namespace naoth

#endif //_MAC_ADDR_H_
