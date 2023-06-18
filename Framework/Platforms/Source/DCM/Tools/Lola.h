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

#include "LolaData.h"

// this is needed for communication with the UNIX socket on NAO
#ifdef NAO
  #include <ext/stdio_filebuf.h>
  #include <sys/socket.h>
  #include <sys/un.h>
  #include <unistd.h>
#endif

#include <sstream>
#include <iostream>

/** 
  Client for the UNIX socket on the NAO robot
*/
#ifdef NAO
class Lola 
{
  // POSIX file descriptor for the UNIX Socket on NAO
  int fd;
  // C-style file pointer to fd used to write by msgpack::fbuffer
  FILE* fp;
  
  static const int PACKET_ZIZE = 896;
  msgpack::unpacker m_pac;

  // indicated that the LOLA client is in the error state
  bool error = false;
  
  public:
    Lola() {}  

    void connectSocket() 
    {
      if ( (fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        std::cerr << "[LOLA] socket error" << std::endl;
        //exit(-1);
        error = true;
        return;
      }

      struct sockaddr_un addr;
      memset(&addr, 0, sizeof(addr));
      addr.sun_family = AF_UNIX;
      strncpy(addr.sun_path, "/tmp/robocup", sizeof(addr.sun_path)-1);

      if (connect(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        std::cerr << "[LOLA] connect error" << std::endl;
        //exit(-1);
        error = true;
        return ;
      }

      //open the file for writing
      fp = fdopen(fd, "w");
    }

    bool hasError() {
      return error;
    }
  
    void writeActuators(const ActuatorData& data) 
    {
      msgpack::fbuffer fbuf(fp);
      msgpack::pack(fbuf, data);
      fflush(fp);
    }
    
    void readSensors(SensorData& data) 
    {
      // make sure we have enough space
      m_pac.reserve_buffer(PACKET_ZIZE);

      // read from the soccet (POSIX style)
      size_t bytes = read(fd, m_pac.buffer(), m_pac.buffer_capacity());
      
      if(bytes != PACKET_ZIZE) {
        std::cerr << "[LOLA] wrong message size: " << bytes << " expected " << PACKET_ZIZE << std::endl;
      }
      m_pac.buffer_consumed(bytes);
      
      msgpack::object_handle oh;
      m_pac.next(oh);

      // deserialized object is valid during the msgpack::object_handle instance is alive.
      msgpack::object deserialized = oh.get();
      
      // print the decerialized content
      if(bytes != PACKET_ZIZE) {
        std::cout << deserialized << std::endl;
        //assert(false);
      }

      // TODO: is this faster?
      // get() - deserialized object is valid during the msgpack::object_handle instance is alive.
      // convert() - convert msgpack::object instance into the original type.
      //oh.get().convert(data);

      // convert msgpack::object instance into the original type.
      deserialized.convert(data);

      // DEBUG:
      //std::cout << oh.get() << std::endl;
    }
};
#endif


// TODO: separate file
#ifndef NAO

#include <glib.h>
#include <gio/gio.h>

/** 
  TCP based client using GLIB, emulating LOLA (experimental).
*/
class GLola 
{
  GSocketClient * client;
  GSocketConnection * connection;

  static const int PACKET_ZIZE = 896;
  msgpack::unpacker m_pac;

public:

  void connect(const std::string& host, int port) 
  {
    GError * error = NULL;

    /* create a new connection */
    connection = NULL;
    client = g_socket_client_new();

    /* connect to the host */
    connection = g_socket_client_connect_to_host (client, (gchar*)host.c_str(), port, NULL, &error);
    //fd = g_socket_get_fd( g_socket_connection_get_socket (connection) );
  }

  void writeActuators(const ActuatorData& data) 
  {
    std::stringstream sbuf;
    msgpack::pack(sbuf, data);

    std::string s = sbuf.str();

    GError * error = NULL;
    GOutputStream * ostream = g_io_stream_get_output_stream (G_IO_STREAM (connection));
    g_output_stream_write (ostream, s.c_str(), (int)s.size(), NULL, &error);
  }

  void readSensors(SensorData& data) 
  {
    m_pac.reserve_buffer(PACKET_ZIZE);

    GError * error = NULL;
    GInputStream * istream = g_io_stream_get_input_stream (G_IO_STREAM (connection));
    int bytes = g_input_stream_read (istream, m_pac.buffer(), m_pac.buffer_capacity(), NULL, &error);

    if(bytes != PACKET_ZIZE) {
      std::cout << "[LOLA] wrong message size: " << bytes << " expected " << PACKET_ZIZE << std::endl;
    }
    m_pac.buffer_consumed(bytes);

    msgpack::object_handle oh;
    m_pac.next(oh);

    // deserialized object is valid during the msgpack::object_handle instance is alive.
    msgpack::object deserialized = oh.get();

    // print the decerialized content
    if(bytes != PACKET_ZIZE) {
      std::cout << deserialized << std::endl;
      //assert(false);
    }

    // convert msgpack::object instance into the original type.
    deserialized.convert(data);

    // DEBUG:
    //std::cout << oh.get() << std::endl;
  }
};

#endif