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

DebugCommunicator::DebugCommunicator(unsigned short port)
: serverSocket(NULL),
connection(NULL),
port(port),
fatalFail(false)
{
}

void DebugCommunicator::init()
{
  GError* err = internalInit();
  if (err)
  {
    fatalFail = true;
    g_warning("(DebugServer) No communication available. Error message:\n%s", err->message);
  } else
  {
    std::cout << "(DebugServer:port " << port << ") " << "is ready for incoming connections "
      << std::endl;
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

  g_socket_set_blocking(serverSocket, true);

  return NULL;
}

void DebugCommunicator::sendMessage(const std::string& data)
{
  GError* err = internalSendMessage(data);
  if (err)
  {
    std::cerr << "[DebugServer:port " << port << "] " << "ERROR: (SocketException in sendMessage) "
      << err->message << std::endl;

    disconnect();
  }
}//end sendMessage

GError* DebugCommunicator::internalSendMessage(const std::string& data)
{
  GError* err = NULL;
  if (connection != NULL)
  {
    g_socket_send(connection, data.c_str(), data.size(), NULL, &err);
    
    if (err) return err;
  }//end if

  return NULL;
}//end sendMessage


GError* DebugCommunicator::triggerConnect()
{
  GError* err = NULL;

  if (connection == NULL)
  {
    // try to accept a eventually pending connection request
    if (serverSocket != NULL)
    {
      connection = g_socket_accept(serverSocket, NULL, &err);
      if (err) return err;

      g_socket_set_blocking(connection, true);

      if (connection != NULL)
      {
        GInetSocketAddress* remoteSocketAddr =
          G_INET_SOCKET_ADDRESS(g_socket_get_remote_address(connection, &err));

        if (err) return err;

        GInetAddress* addr = g_inet_socket_address_get_address(remoteSocketAddr);

        char* addrString = g_inet_address_to_string(addr);
        std::cout << "[DebugServer:port " << port << "] " << "connected to "
          << addrString << std::endl;
        g_free(addrString);
        g_object_unref(remoteSocketAddr);

      }
    }
  }
  return NULL;
}

std::string* DebugCommunicator::triggerReceive(GError** err)
{
  *err = NULL;
  if (connection != NULL)
  {

    // read until \0 or \n character found
    std::stringstream buf;
    char c;
    g_socket_set_blocking(connection, false); // only check if something is there
    int bytesReceived = g_socket_receive(connection, &c, 1, NULL, NULL);
    g_socket_set_blocking(connection,true); // read complete answer from now

    if(bytesReceived < 1)
    {
      return NULL;
    }

    while(*err == NULL && c != '\n')
    {
      if(c != '\r')
      {
        buf << c;
      }
      g_socket_receive(connection, &c, 1, NULL, err);
    }

    if(*err) return NULL;

    std::string* result = new std::string(buf.str());
    return result;

  }//end if
  return NULL;
}

std::string* DebugCommunicator::readMessage()
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

    return NULL;
  }

  std::string* result = triggerReceive(&err);
  if(err)
  {
    std::cerr << "[DebugServer:port " << port << "] " << "ERROR: (SocketException in triggerRead) "
      << err->message << std::endl;
    disconnect();
    delete result;
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

