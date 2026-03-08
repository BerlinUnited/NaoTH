/**
* @file SocketConnector.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Mellmann, Heinrich</a>
*
* 
* Example for UNIX soccet communication.
* https://github.com/troydhanson/network/blob/master/unixdomain/01.basic/cli.c
*
* TODO: this needs to be unified with other various communication channels
*/

#ifndef SOCKET_CONNECTOR_H
#define SOCKET_CONNECTOR_H

// this is needed for communication with the UNIX socket on NAO
#ifndef _WIN32
  #include <ext/stdio_filebuf.h>
  #include <sys/socket.h>
  #include <sys/un.h>
  #include <unistd.h>

  #include <msgpack.hpp>
#endif

#include <sstream>
#include <iostream>
#include <cerrno>
#include <cstring>
#include <thread>

#include "BoosterData.h" 

/** 
Client for the UNIX socket on the NAO robot
*/
#ifdef _WIN32

class SocketConnector 
{
public:
  inline void connect(const char* path) {}
  inline void disconnect() {}
  inline bool hasError() { return false; }

  inline size_t send_exact(const void* buffer, size_t size) {}
  inline bool send_msgpack(const msgpack::sbuffer& sbuf) {}
  inline void send_actuators(const Booster::ActuatorData& data){}
};

#else
class SocketConnector 
{
  // POSIX file descriptor for the UNIX Socket on NAO
  int fd = -1;
  // C-style file pointer to fd used to write
  //FILE* fp;

  // indicated that the LOLA client is in the error state
  bool error = false;

public:

  SocketConnector() {}  

  void connect(const char* path) 
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
    strncpy(addr.sun_path, path, sizeof(addr.sun_path)-1);

    std::cerr << "[BODY_BRIDGE] connect to socket: " << addr.sun_path << std::endl;
    if (::connect(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
      std::cerr << "[BODY_BRIDGE] connect error: " << std::strerror(errno) << std::endl;
      //exit(-1);
      error = true;
      return ;
    }

    //open the file for writing
    //fp = fdopen(fd, "w");
  }

  inline void disconnect() {
  }

  bool hasError() {
    return error;
  }


  inline void send_actuators(const Booster::ActuatorData& data) 
  {
    msgpack::sbuffer sbuf;
    msgpack::pack(sbuf, data);
    
    if(!send_msgpack(sbuf)) {
      std::cerr << "[BODY_BRIDGE] send msgpack failed: " << std::strerror(errno) << "\n";
      error = true;
    }
  }

public:

  inline size_t send_exact(const void* buffer, size_t size)
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

  inline bool send_msgpack(const msgpack::sbuffer& sbuf)
  {
    uint32_t len = static_cast<uint32_t>(sbuf.size());
    uint32_t len_be = htonl(len);

    if (send_exact(&len_be, sizeof(len_be)) != sizeof(len_be)) return false;
    if (send_exact(sbuf.data(), sbuf.size()) != sbuf.size()) return false;
    return true;
  }
};
#endif


#endif // SOCKET_CONNECTOR_H