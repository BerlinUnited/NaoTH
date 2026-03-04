/**
* @file LolaData.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Mellmann, Heinrich</a>
*
* 
* Example for UNIX soccet communication.
* https://github.com/troydhanson/network/blob/master/unixdomain/01.basic/cli.c
*
*/

#ifndef IMAGE_BRIDGE_H
#define IMAGE_BRIDGE_H

#include "Representations/Infrastructure/Image.h"

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

/** 
Client for the UNIX socket on the NAO robot
*/
#ifdef _WIN32

class ImageBridge 
{
public:
  inline void connectSocket() {}
  inline bool hasError() { return false; }
  inline void readImage(naoth::Image& image) {
    // for testing: simulate 30fps
    std::this_thread::sleep_for(std::chrono::milliseconds(33));
  }
};

#else
class ImageBridge 
{
  // POSIX file descriptor for the UNIX Socket on NAO
  int fd;
  // C-style file pointer to fd used to write by msgpack::fbuffer
  //FILE* fp;

  // indicated that the LOLA client is in the error state
  bool error = false;

private:

  static inline int recv_exact(int fd, void* buf, size_t n) 
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



public:

  ImageBridge() {}  

  void connectSocket() 
  {
    if ( (fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
      std::cerr << "[IMAGE_BRIDGE] socket error: " << std::strerror(errno) << std::endl;
      //exit(-1);
      error = true;
      return;
    }

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, "/tmp/naoth_image", sizeof(addr.sun_path)-1);

    std::cerr << "[IMAGE_BRIDGE] connect to socket: " << addr.sun_path << std::endl;
    if (connect(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
      std::cerr << "[IMAGE_BRIDGE] connect error: " << std::strerror(errno) << std::endl;
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

  /*
  void writeActuators(const ActuatorData& data) 
  {
  // DEBUG: check size
  //std::stringstream sbuf;
  //msgpack::pack(sbuf, data);
  //assert(sbuf.str().size() == PACKET_SIZE_ACTUATOR);

  msgpack::fbuffer fbuf(fp);
  msgpack::pack(fbuf, data);
  fflush(fp);
  }
  */

  void readImage(naoth::Image& image)
  {
    // read from the soccet (POSIX style)
    size_t bytes = recv_exact(fd, image.data(), image.data_size());

    if(bytes != image.data_size()) {
      std::cerr << "[IMAGE_BRIDGE] wrong message size: " << bytes << " expected " << image.data_size() << std::endl;
    }
  }
};
#endif


#endif // IMAGE_BRIDGE_H