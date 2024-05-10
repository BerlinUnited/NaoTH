/**
 * @file NetUtils.h
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
 * 
 * Various tools for working with network.
 */

#ifndef NET_UTILS_H
#define NET_UTILS_H

#include <cstdio>
#include <cstring>
#include <string>

#ifdef WIN32
  #include <winsock.h>
#else // Linux/MACOS
  #include <sys/ioctl.h>
  #include <sys/types.h>
  #include <sys/socket.h>
  #include <net/if.h>
  #include <ifaddrs.h>
  #include <netinet/in.h>
  #include <arpa/inet.h>
  #include <sys/socket.h>
#endif // undef WIN32


namespace naoth
{

class NetUtils
{
public:
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
    std::string ip("unknown");

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
        std::string n(ifa->ifa_name);
        if ( n == name )
        {
          ip = std::string(addressBuffer);
        }
      }
    }
    if (ifAddrStruct!=NULL) freeifaddrs(ifAddrStruct);
#endif
    return ip;
  }

  static std::string getBroadcastAddr(const std::string& name)
  {
    std::string ip("unknown");
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
        std::string n(ifa->ifa_name);
        if ( n == name )
        {
          ip = std::string(addressBuffer);
        }
      }
    }
    if (ifAddrStruct!=NULL) freeifaddrs(ifAddrStruct);
#endif
    return ip;
  }

  /**
  * Sets the broadcast flag for the socket. If true, datagram
  * sockets are allowed to send packets to a broadcast address.  
  * This option has no effect on stream-oriented sockets.
  * 
  * GLib supports setting soccet options starting version 2.36.
  *   g_socket_set_broadcast (socket, true);
  *   https://github.com/GNOME/glib/blob/86dd02f48762ae97c7bc805c45e8905cd969bbac/gio/gsocket.c#L1764
  * and ..
  *   g_socket_set_option (...);
  *   https://github.com/GNOME/glib/blob/86dd02f48762ae97c7bc805c45e8905cd969bbac/gio/gsocket.c#L6515
  * 
  * Set the broadcast option directly. 
  * Linux and Windows let you set a single-byte value from an int,
  * but most other platforms don't.
  * https://github.com/GNOME/glib/blob/main/gio/gsocket.c#L6340
  */
  static void my_g_socket_set_broadcast(GSocket* socket, gboolean broadcast)
  {
#ifdef WIN32
    // https://learn.microsoft.com/en-us/windows/win32/api/winsock/nf-winsock-setsockopt
    // https://learn.microsoft.com/en-us/windows/win32/winprog/windows-data-types
    BOOL broadcastFlag = (broadcast ? TRUE : FALSE);
    setsockopt(g_socket_get_fd(socket), SOL_SOCKET, SO_BROADCAST, (const char*)(&broadcastFlag), sizeof(broadcastFlag));
#else // Linux/MACOS
    // TODO: the following might not work on MACOS
    // https://linux.die.net/man/3/setsockopt
    int broadcastFlag = (broadcast ? 1 : 0);
    setsockopt(g_socket_get_fd(socket), SOL_SOCKET, SO_BROADCAST, (const char*)(&broadcastFlag), static_cast<socklen_t> (sizeof(int)));
#endif
  }

};
} // namespace naoth

#endif // NET_UTILS_H
