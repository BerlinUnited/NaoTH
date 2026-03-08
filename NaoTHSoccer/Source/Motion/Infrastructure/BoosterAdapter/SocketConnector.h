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

  inline void send_actuators(const Booster::ActuatorData& data) {}
  inline void receive_sensors(const Booster::SensorData& data) {}
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

  msgpack::unpacker m_pac;

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

  inline bool send_msgpack(const msgpack::sbuffer& sbuf)
  {
    uint32_t len = static_cast<uint32_t>(sbuf.size());
    uint32_t len_be = htonl(len);

    if (send_exact(&len_be, sizeof(len_be)) != sizeof(len_be)) { 
      return false;
    }
    if (send_exact(sbuf.data(), sbuf.size()) != sbuf.size()) { 
      return false;
    }

    return true;
  }

  inline bool receive_sensors(Booster::SensorData& data)
  {
    uint32_t len_be;
    if (recv_exact(&len_be, sizeof(len_be)) != sizeof(len_be)) { 
      return false; 
    }

    uint32_t len = ntohl(len_be);

    // make sure we have enough space
    m_pac.reserve_buffer(len);

    size_t bytes = recv_exact(m_pac.buffer(), m_pac.buffer_capacity());
    if (bytes != sizeof(m_pac.buffer_capacity())) {
      return false;
    }

    m_pac.buffer_consumed(bytes);

    msgpack::object_handle oh;
    m_pac.next(oh);

    // deserialized object is valid during the msgpack::object_handle instance is alive.
    msgpack::object deserialized = oh.get();

    // debug
    //std::cout << deserialized << std::endl;

    // convert msgpack::object instance into the original type.
    deserialized.convert(data);

    return true;
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

  inline int recv_exact(void* buf, size_t n) 
  {
    auto* p = static_cast<char*>(buf);
    size_t got = 0;
    while (got < n) 
    {
      ssize_t r = ::recv(fd, p + got, n - got, 0);

      // peer closed
      if (r == 0) { 
        return r; 
      }    

      // interrupted -> retry
      if (r < 0) {
        if (errno == EINTR) { 
          continue; 
        }
        std::cerr << "[IMAGE_BRIDGE] recv_exact error: " << std::strerror(errno) << std::endl;
        return r;
      }
      got += static_cast<size_t>(r);
    }
    return got;
  }

};
#endif


#endif // SOCKET_CONNECTOR_H