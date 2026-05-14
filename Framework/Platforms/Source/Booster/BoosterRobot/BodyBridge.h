/**
* @file BodyBridge.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Mellmann, Heinrich</a>
*
* 
* Example for UNIX soccet communication.
* https://github.com/troydhanson/network/blob/master/unixdomain/01.basic/cli.c
*
*/

#ifndef BODY_BRIDGE_H
#define BODY_BRIDGE_H

//#include "Representations/Infrastructure/Image.h"

// this is needed for communication with the UNIX socket on NAO
#ifndef _WIN32
  #include <ext/stdio_filebuf.h>
  #include <sys/socket.h>
  #include <sys/un.h>
  #include <unistd.h>
#endif

#include <sstream>
#include <iostream>
#include <cerrno>
#include <cstring>
#include <thread>
#include <msgpack.hpp>

/** 
Client for the UNIX socket on the NAO robot
*/
#ifdef _WIN32

class BodyBridge 
{
public:
  inline void connectSocket() {}
  inline bool hasError() { return false; }

  inline void setHead(double head_yaw, double head_pitch) {
  }

};

#else
class BodyBridge 
{
  // POSIX file descriptor for the UNIX Socket on NAO
  int fd;
  // C-style file pointer to fd used to write
  //FILE* fp;

  // indicated that the LOLA client is in the error state
  bool error = false;

public:

  BodyBridge() {}  

  void connectSocket() 
  {
    if ( (fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
      std::cerr << "[BODY_BRIDGE] socket error: " << std::strerror(errno) << std::endl;
      //exit(-1);
      error = true;
      return;
    }

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, "/tmp/naoth_body", sizeof(addr.sun_path)-1);

    std::cerr << "[BODY_BRIDGE] connect to socket: " << addr.sun_path << std::endl;
    if (connect(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
      std::cerr << "[BODY_BRIDGE] connect error: " << std::strerror(errno) << std::endl;
      //exit(-1);
      error = true;
      return ;
    }

    //open the file for writing
    //fp = fdopen(fd, "w");
  }

  bool hasError() {
    return error;
  }

  inline void setHead(double head_yaw, double head_pitch) 
  {
    msgpack::sbuffer sbuf;
    msgpack::packer<msgpack::sbuffer> pk(sbuf);

    pk.pack_map(3);
    pk.pack("cmd");   pk.pack("setHead");
    pk.pack("yaw");   pk.pack(head_yaw);
    pk.pack("pitch"); pk.pack(head_pitch);

    if(!send_msgpack(fd, sbuf)) {
      std::cerr << "[BODY_BRIDGE] send msgpack failed: " << std::strerror(errno) << "\n";
      error = true;
    }
  }

private:

  size_t send_exact(int fd, const void* buffer, size_t size)
  {
    const char* ptr = static_cast<const char*>(buffer);
    size_t total = 0;

    while(total < size)
    {
      ssize_t sent = send(fd, ptr + total, size - total, 0);

      if(sent <= 0) {
        if(errno == EINTR) continue;
        return total;
      }

      total += sent;
    }

    return total;
  }

  inline bool send_msgpack(int fd, const msgpack::sbuffer& sbuf)
  {
    uint32_t len = static_cast<uint32_t>(sbuf.size());
    uint32_t len_be = htonl(len);

    if (send_exact(fd, &len_be, sizeof(len_be)) != sizeof(len_be)) return false;
    if (send_exact(fd, sbuf.data(), sbuf.size()) != sbuf.size()) return false;
    return true;
  }
};
#endif


#endif // BODY_BRIDGE_H