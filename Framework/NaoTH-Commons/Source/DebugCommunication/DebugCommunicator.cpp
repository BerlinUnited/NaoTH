/* 
 * File:   DebugCommunicator.cpp
 * Author: thomas
 * 
 * Created on 8. März 2009, 19:31
 */

#include <stdlib.h>
#include <limits.h>

#include <assert.h>
#include <vector>
#include <string>

#include <iostream>
#include <sstream>

#include "DebugCommunicator.h"


DebugCommunicator::DebugCommunicator()
: serverSocket(NULL),
connection(NULL),
port(-1),
fatalFail(false)
{
}

void DebugCommunicator::init(unsigned short portNum)
{
  port = portNum;
  
  GError* err = internalInit();
  if (err)
  {
    fatalFail = true;
    g_warning("(DebugServer) No communication available. Error message:\n%s", err->message);
  } else
  {
    g_debug("Port %d is ready for incoming connections ", port);
  }

}//end init

GError* DebugCommunicator::internalInit()
{
  GError *err = NULL;
  serverSocket = g_socket_new(G_SOCKET_FAMILY_IPV4, G_SOCKET_TYPE_STREAM, G_SOCKET_PROTOCOL_TCP, &err);

  if (err) return err;

  g_socket_set_blocking(serverSocket, true);

  GInetAddress* inetAddress = g_inet_address_new_any(G_SOCKET_FAMILY_IPV4);
  GSocketAddress* socketAddress = g_inet_socket_address_new(inetAddress, port);

  g_socket_bind(serverSocket, socketAddress, true, &err);
  if (err) return err;

  g_socket_listen(serverSocket, &err);
  if (err) return err;

  return NULL;
}

bool DebugCommunicator::sendMessage(const char* data, size_t size)
{
  GError* err = internalSendMessage(data, size);
  if (err)
  {
    g_warning("(SocketException in sendMessage) %s", err->message);
    g_error_free(err);
    return false;
  }
  return true;
}//end sendMessage


GError* DebugCommunicator::internalSendMessage(const char* data, size_t size)
{
  GError* err = NULL;
  if (connection != NULL)
  {
    gsize pos = 0;

    while(err == NULL && connection != NULL && pos < size)
    {
      gsize length = size-pos;

      gsize sent = g_socket_send(connection, data+pos, length, NULL, &err);
      pos += sent;
    }//end while
  }//end if

  return err;
}//end sendMessage


char* DebugCommunicator::internalReadMessage(GError** err)
{
  *err = NULL;
  if (connection != NULL)
  {

    GString* buffer = g_string_new("");
    char c = 0;

    // read until \n character found
    while(!fatalFail && *err == NULL && c != '\n')
    {
      if(err != NULL)
      {
        gssize read_bytes =
            g_socket_receive_with_blocking(connection, &c, 1, buffer->len == 0 ? false : true ,NULL, err);

        if(read_bytes < 1)
        {
          g_string_free(buffer, true);

          if(read_bytes == 0)
          {
            // we got 0 bytes, this indicates
            // an error in the connection (-1 is set if no data was available
            // in non-blocking mode)
            throw "Connection Error";
          }
          else
          {
            // just return nothing
            return NULL;
          }
        }
      }

      if(c != '\r' && c != '\n')
      {
        g_string_append_c(buffer,c);
      }
    } // end while no \n found

    if(*err)
    {
      g_string_free(buffer,true);
    }

    g_string_append_c(buffer,'\0');
    return g_string_free(buffer, false);
  }//end if connection not null

  return NULL;
}
char* DebugCommunicator::readMessage()
{
  if (fatalFail)
  {
    throw "fatalFail";
  }

  GError* err = NULL;
  char* result = internalReadMessage(&err);
  if(err)
  {
    if(err->code != G_IO_ERROR_WOULD_BLOCK)
    {
      std::cerr << "[DebugServer:port " << port << "] " << "ERROR: (SocketException in readMessage) "
      << err->message << std::endl;

      if(result != NULL)
      {
        g_free(result);
      }
      throw "Socket Exception";
    }
    else
    {
      g_error_free(err);
    }
  }
  return result;
}//end triggerRead

bool DebugCommunicator::connect(unsigned int timeout)
{
  GError* err = NULL;

  if (connection == NULL)
  {
    // try to accept an eventually pending connection request
    if (serverSocket != NULL)
    {
      g_socket_set_timeout(serverSocket, timeout);
      connection = g_socket_accept(serverSocket, NULL, &err);
      g_socket_set_timeout(serverSocket, 0);
      if (err) return false;

      if (connection != NULL)
      {

        GInetSocketAddress* remoteSocketAddr =
          G_INET_SOCKET_ADDRESS(g_socket_get_remote_address(connection, &err));

        if (err)
        {
          // cleanup
          g_socket_close(connection, NULL);
          g_object_unref(connection);
          connection = NULL;
          return false;
        }
        GInetAddress* addr = g_inet_socket_address_get_address(remoteSocketAddr);

        char* addrString = g_inet_address_to_string(addr);
        g_message("DebugServer port %d connected to %s", port, addrString);
        g_free(addrString);
        g_object_unref(remoteSocketAddr);

        return true;
      }
    }
  }
  return false;
}

void DebugCommunicator::disconnect()
{
  if (connection != NULL)
  {
    g_socket_close(connection, NULL);
    g_object_unref(connection);
    connection = NULL;
    std::cerr << "[DebugServer:port " << port << "] " << "disconnected" << std::endl;
  }//end if
}

bool DebugCommunicator::isConnected()
{
  return connection != NULL;
}

DebugCommunicator::~DebugCommunicator()
{
  disconnect();

  if (serverSocket != NULL)
  {
    g_socket_close(serverSocket, NULL);
    g_object_unref(serverSocket);
  }
}

