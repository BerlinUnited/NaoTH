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

    while(pos + 1 < size)
    {
      if(err == NULL && connection != NULL)
      {
        gsize length = size-pos;

        gsize sent = g_socket_send(connection, data+pos, length, NULL, &err);
        pos += sent;
      }
    }//end while

    if (err) return err;
  }//end if
  return NULL;
}//end sendMessage


char* DebugCommunicator::internalReadMessage(GError** err)
{
  *err = NULL;
  if (connection != NULL)
  {
    // wait until if there is data available
    g_socket_condition_wait(connection, G_IO_IN, NULL, err);
    if(*err == NULL)
    {
      // read until \0 or \n character found
      GString* buffer = g_string_new("");
      char c;

      int bytesReceived = g_socket_receive(connection, &c, 1, NULL, NULL);

      if(bytesReceived < 1)
      {
        g_string_free(buffer, true);
        // if G_IO_IN was signalled but there was no data available this is a sign that
        // the client was disconnected
        return NULL;
      }

      while(*err == NULL && c != '\n')
      {
        if(c != '\r')
        {
          g_string_append_c(buffer,c);
        }
        g_socket_receive(connection, &c, 1, NULL, err);
      }

      if(*err)
      {
        g_string_free(buffer,true);
        return NULL;
      }

      g_string_append_c(buffer,'\0');
      return g_string_free(buffer, false);
    }
  }//end if
  return NULL;
}
char* DebugCommunicator::readMessage()
{
  if (fatalFail)
  {
    return NULL;
  }

  GError* err = NULL;
  char* result = internalReadMessage(&err);
  if(err)
  {
    std::cerr << "[DebugServer:port " << port << "] " << "ERROR: (SocketException in triggerRead) "
      << err->message << std::endl;
    g_free(result);
    g_error_free(err);
    return NULL;
  }
  return result;
}//end triggerRead

bool DebugCommunicator::isDataAvailable()
{
  if(fatalFail)
  {
    return false;
  }

  if(connection != NULL)
  {
    GIOCondition ret = g_socket_condition_check(connection, G_IO_IN);
    if(ret & G_IO_IN)
    {
      return true;
    }
  }

  return false;
}

bool DebugCommunicator::connect(bool blocking)
{
  GError* err = NULL;

  if (connection == NULL)
  {
    // try to accept an eventually pending connection request
    if (serverSocket != NULL)
    {
      g_socket_set_blocking(serverSocket, blocking);
      connection = g_socket_accept(serverSocket, NULL, &err);
      g_socket_set_blocking(serverSocket, true);
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

