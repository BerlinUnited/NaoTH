/* 
 * File:   DebugCommunicator.cpp
 * Author: thomas
 * 
 * Created on 8. march 2009, 19:31
 */

#include <stdlib.h>
#include <limits.h>

#include <assert.h>
#include <vector>
#include <string>

#include <iostream>
#include <sstream>

#include <Tools/NaoTime.h>

#include "DebugCommunicator.h"


DebugCommunicator::DebugCommunicator()
  : 
    serverSocket(NULL),
    connection(NULL),
    port(-1),
    fatalFail(false),
    time_of_last_message(0),
    time_out_delta(5) // seconds
{
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

void DebugCommunicator::init(unsigned short portNum)
{
  port = portNum;
  
  GError* err = internalInit();
  if (err)
  {
    fatalFail = true;
    std::cerr << "[DebugServer:port " << port << "] " << "No communication available. Error message:\n" << err->message << std::endl;
  } else {
    std::cout << "[DebugServer:port " << port << "] " << "is ready for incoming connections." << std::endl;
  }
}//end init

GError* DebugCommunicator::internalInit()
{
  GError* err = NULL;

  serverSocket = g_socket_new(G_SOCKET_FAMILY_IPV4, G_SOCKET_TYPE_STREAM, G_SOCKET_PROTOCOL_TCP, &err);
  if (err) { return err; }

  g_socket_set_blocking(serverSocket, true);

  GInetAddress* inetAddress = g_inet_address_new_any(G_SOCKET_FAMILY_IPV4);
  GSocketAddress* socketAddress = g_inet_socket_address_new(inetAddress, port);

  g_socket_bind(serverSocket, socketAddress, true, &err);
  if (err) { return err; }

  g_socket_listen(serverSocket, &err);
  if (err) { return err; }

  return NULL;
}

bool DebugCommunicator::sendMessage(gint32 id, const char* data, size_t size)
{
  if (fatalFail) {
    throw "fatalFail";
  }

  GError* err = internalSendMessage(id, data, size);
  if (err)
  {
    std::cerr << "[DebugServer:port " << port << "] " << "SocketException in sendMessage: " << err->message << std::endl;
    g_error_free(err);
    return false;
  }
  return true;
}//end sendMessage


GError* DebugCommunicator::internalSendMessage(gint32 id, const char* data, size_t size)
{
  GError* err = NULL;
  if (connection != NULL)
  {
    g_socket_set_blocking(connection, true);

    // send the message id
    g_socket_send(connection, (char*) &id, 4, NULL, &err);

    // send message size in 4 bytes
    guint32 sizeFixed = static_cast<unsigned int> (size);
    g_socket_send(connection, (char*) &sizeFixed, 4, NULL, &err);

    gsize pos = 0;
    while(err == NULL && connection != NULL && pos < size)
    {
      gsize length = size-pos;

      gsize sent = g_socket_send(connection, data+pos, length, NULL, &err);
      pos += sent;
    }
  }

  return err;
}//end sendMessage


GString* DebugCommunicator::readMessage(gint32& id)
{
  if (fatalFail) {
    throw "fatalFail";
  }

  GError* err = NULL;
  GString* result = NULL;
  id = internalReadMessage(&result, &err);

  if(err)
  {
    if(err->code != G_IO_ERROR_WOULD_BLOCK)
    {
      std::cerr << "[DebugServer:port " << port << "] " << "ERROR: (SocketException in readMessage) "
                << err->message << std::endl;
      g_error_free(err);

      if(result != NULL) {
        g_string_free(result,true);
      }
      
      throw "Socket Exception";
    }
    else
    {
      g_error_free(err);
    }
  }
  return result;
}//end readMessage

gint32 DebugCommunicator::internalReadMessage(GString** result, GError** err)
{
  *err = NULL;
  if (connection == NULL) {
    return -1;
  }

  // read the message id
  gint32 messageId = -1;
  gssize initialBytes = g_socket_receive_with_blocking(connection, (char*) &messageId, 4, false, NULL, err);

  // nothing to read
  if ( initialBytes == -1 ) {
    // no data was received for a while, time out
    unsigned long long current_time = naoth::NaoTime::getSystemTimeInMilliSeconds();
    if( time_of_last_message > 0 && 
        time_out_delta > 0 && 
        current_time > time_of_last_message + time_out_delta*1000 ) 
    {
      throw "connection timed out";
    }

    return -1;
  }

  if(initialBytes == 0) {
    throw "connection closed by peer";
  }

  if(initialBytes != 4) {
    throw "error reading message id";
  }

  // some data was received, so keep the connection alive
  time_of_last_message = naoth::NaoTime::getSystemTimeInMilliSeconds();

  // heartbeat message, ignore
  if(messageId == -1) { 
    // std::cout << "heartbeat received" << std::endl;
    return -1;
  }

  gint32 sizeOfMessage = 0;
  if(g_socket_receive_with_blocking(connection, (char*) &sizeOfMessage, 4, true, NULL, err) != 4) {
    throw "error reading message size";
  }

  if(sizeOfMessage > 0)
  {
    GString* buffer = g_string_new("");
    g_string_set_size(buffer, sizeOfMessage);
    g_assert(buffer->len == (guint32)sizeOfMessage);

    // read message completly
    guint32 pos = 0;
    while(*err == NULL && pos < (guint32)sizeOfMessage)
    {
      gssize read_bytes =
          g_socket_receive_with_blocking(connection, buffer->str + pos, sizeOfMessage - pos, true ,NULL, err);

      if(read_bytes == 0)
      {
        g_string_free(buffer, true);
        buffer = NULL;
        // we got 0 bytes, this indicates
        // an error in the connection (-1 is set if no data was available
        // in non-blocking mode)
        throw "Connection Error";
      }
      pos += static_cast<unsigned int> (read_bytes);
    }

    if(*err) {
      g_string_free(buffer, true);
      return -1;
    }

    *result = buffer;
    return messageId;
  }

  return -1;
}


bool DebugCommunicator::connect(int timeout)
{
  GError* err = NULL;

  // TODO: use isConnected()?
  // already connected
  if (connection != NULL) {
    return true;
  }

  // TODO: throw?
  // not initialized
  if (serverSocket == NULL) {
    return false;
  }

  // prepare the server socket
  if(timeout < 0) {
    g_socket_set_blocking(serverSocket, false);
  } else {
    g_socket_set_blocking(serverSocket, true);
    g_socket_set_timeout(serverSocket, timeout);
  }

  // try to accept an eventually pending connection request
  connection = g_socket_accept(serverSocket, NULL, &err);

  // TODO: throw?
  if (err) {
    return false;
  }

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

    // print the address
    GInetAddress* addr = g_inet_socket_address_get_address(remoteSocketAddr);
    char* addrString = g_inet_address_to_string(addr);
    std::cout << "[DebugServer:port " << port << "] " << "connected to " << addrString << std::endl;
    g_free(addrString);
    g_object_unref(remoteSocketAddr);

    // init the time stamp for the time out
    time_of_last_message = naoth::NaoTime::getSystemTimeInMilliSeconds();
    g_socket_set_timeout(connection, time_out_delta);

    return true;
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
  }
}

bool DebugCommunicator::isConnected()
{
  return connection != NULL && g_socket_is_connected(connection);
}


