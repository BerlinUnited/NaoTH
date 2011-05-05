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
  //g_debug("sending message with size %d:\n%s", (int) size, data);
  GError* err = internalSendMessage(data, size);
  if (err)
  {
    g_warning("(SocketException in sendMessage) %s", err->message);

    g_error_free(err);

    disconnect();
    return false;
  }
  return true;
}//end sendMessage

GError* DebugCommunicator::internalSendMessage(const char* data, size_t size)
{
  char zero = '\0';
  GError* err = NULL;
  if (connection != NULL)
  {
    gsize pos = 0;
    while(pos < size -1)
    {
      if(err == NULL && connection != NULL)
      {
        pos += g_socket_send(connection, data+pos, size-pos, NULL, &err);
      }
    }
    
    if(err == NULL)
    {
      g_socket_send(connection, &zero, sizeof(char), NULL, &err);
    }
    
    if (err) return err;
  }//end if
  return NULL;
}//end sendMessage


GError* DebugCommunicator::triggerConnect()
{
  GError* err = NULL;

  if (connection == NULL)
  {
    // try to accept an eventually pending connection request
    if (serverSocket != NULL)
    {
      connection = g_socket_accept(serverSocket, NULL, &err);
      if (err) return err;

      if (connection != NULL)
      {

        GInetSocketAddress* remoteSocketAddr =
          G_INET_SOCKET_ADDRESS(g_socket_get_remote_address(connection, &err));

        if (err) return err;

        GInetAddress* addr = g_inet_socket_address_get_address(remoteSocketAddr);

        char* addrString = g_inet_address_to_string(addr);
        g_message("DebugServer port %d connected to %s", port, addrString);
        g_free(addrString);
        g_object_unref(remoteSocketAddr);

      }
    }
  }
  return NULL;
}

char* DebugCommunicator::triggerReceive(GError** err)
{
  *err = NULL;
  if (connection != NULL)
  {


    // check if there is data available
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
        disconnect();
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
    else
    {
      disconnect();
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

  GError* err = triggerConnect();
  if (err)
  {
    fatalFail = true;
    std::cerr << "[DebugServer:port " << port << "] " << "FATAL ERROR in triggerRead: "
      << err->message << std::endl;
    std::cerr << "will not try again to get connection" << std::endl;

    g_error_free(err);

    return NULL;
  }

  char* result = triggerReceive(&err);
  if(err)
  {
    std::cerr << "[DebugServer:port " << port << "] " << "ERROR: (SocketException in triggerRead) "
      << err->message << std::endl;
    disconnect();
    g_free(result);
    g_error_free(err);
    return NULL;
  }
  return result;
}//end triggerRead

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

DebugCommunicator::~DebugCommunicator()
{
  disconnect();

  if (serverSocket != NULL)
  {
    g_socket_close(serverSocket, NULL);
    g_object_unref(serverSocket);
  }
}

