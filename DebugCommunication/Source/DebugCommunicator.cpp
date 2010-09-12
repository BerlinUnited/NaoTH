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

#include "DebugCommunicator.h"

DebugCommunicator::DebugCommunicator(unsigned short port)
: serverSocket(NULL),
connection(NULL),
bufferValid(false),
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

bool DebugCommunicator::hasMessageInQueue()
{
  triggerRead();

  return (connection != NULL) && bufferValid;
}//end hasMessageInQueue

std::string DebugCommunicator::peekMessage()
{
  if (bufferValid)
  {
    bufferValid = false;
    return buffer;
  } else
  {
    bufferValid = false;
    return std::string("");
  }
}//end peekMessage

void DebugCommunicator::sendMessage(const std::string& data)
{
  GError* err = sendMessage(data, lastID);
  if (err)
  {
    std::cerr << "[DebugServer:port " << port << "] " << "ERROR: (SocketException in sendMessage) "
      << err->message << std::endl;

    disconnect();
  }
}//end sendMessage

GError* DebugCommunicator::sendMessage(const std::string& data, int id)
{
  GError* err = NULL;
  if (connection != NULL)
  {
    size_t dataSize = data.size();
    // send id
    g_socket_send(connection, (gchar*) & id, 4, NULL, &err);
    if (err) return err;
    // send size
    g_socket_send(connection, (gchar*) dataSize, 4, NULL, &err);
    if (err) return err;
    // send data
    g_socket_send(connection, data.c_str(), data.size(), NULL, &err);
    if (err) return err;
  }//end if

  return NULL;
}//end sendMessage

void DebugCommunicator::sendBeacon()
{
  if (connection != NULL)
  {
    // try to send 0 data in order to check wether we are still connected
    std::string dummyData;
    dummyData.resize(0, '\0');
    sendMessage(dummyData, INT_MAX);
  }
}

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
      bufferValid = false;

      g_socket_set_blocking(connection, true);

      if (connection != NULL)
      {
        GInetSocketAddress* remoteSocketAddr =
          G_INET_SOCKET_ADDRESS(g_socket_get_remote_address(connection, &err));

        if (err) return err;

        GInetAddress* addr = g_inet_socket_address_get_address(remoteSocketAddr);

        std::cout << "[DebugServer:port " << port << "] " << "connected to "
          << g_inet_address_to_string(addr) << std::endl;

        g_object_unref(remoteSocketAddr);


      }
    }
  }
  return NULL;
}

GError* DebugCommunicator::triggerReceive()
{
  GError* err = NULL;
  if (connection != NULL)
  {
    if (!bufferValid)
    {
      // try to read ID
      char buff4[4];

      int bytesRead = g_socket_receive(connection, (gchar*) &buff4, 4, NULL, &err);
      if (err) return err;
      
      if (bytesRead == 4)
      {
        lastID = *((int*) buff4);

        // get size
        int s = 0;
        bytesRead = g_socket_receive(connection, (gchar*) &buff4, 4, NULL, &err);
        if (err) return err;
        
        if (bytesRead == 4)
        {
          s = *((int*) buff4);
          char* tmp = (char*) malloc(s);
          // read data
          int sumOfBytes = 0;
          buffer.clear();
          while (sumOfBytes < s)
          {
            bytesRead = g_socket_receive(connection, tmp, s - sumOfBytes, NULL, &err);
            if (err) return err;
            sumOfBytes += bytesRead;
            buffer.append(tmp, bytesRead);
          }
          bufferValid = true;

          free(tmp);

        }
      }//end if
    }//end if
  }//end if
  return NULL;
}

void DebugCommunicator::triggerRead()
{
  if (fatalFail)
  {
    bufferValid = false;
    return;
  }

  GError* err = triggerConnect();
  if (err)
  {
    fatalFail = true;
    bufferValid = false;
    std::cerr << "[DebugServer:port " << port << "] " << "FATAL ERROR in triggerRead: "
      << err->message << std::endl;
    std::cerr << "will not try again to get connection" << std::endl;

    return;
  }

  err = triggerReceive();
  if(err)
  {

    std::cerr << "[DebugServer:port " << port << "] " << "ERROR: (SocketException in triggerRead) "
      << err->message << std::endl;
    disconnect();
  }
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

