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
    // send message size in 4 bytes
    guint32 sizeFixed = size;
    g_socket_send(connection, (char*) &sizeFixed, 4, NULL, &err);

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


GString* DebugCommunicator::internalReadMessage(GError** err)
{
  *err = NULL;
  if (connection != NULL)
  {

    guint32 sizeOfMessage = 0;
    if(fatalFail) return NULL;

    gssize initialBytes = g_socket_receive_with_blocking(connection, (char*) &sizeOfMessage, 4, false, NULL, err);

    if(initialBytes == 4)
    {
      if(sizeOfMessage > 0)
      {
        GString* buffer = g_string_new("");
        g_string_set_size(buffer, sizeOfMessage);
        g_assert(buffer->len == sizeOfMessage);

        guint32 pos = 0;
        // read message completly
        while(!fatalFail && *err == NULL && pos < sizeOfMessage)
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
            pos += read_bytes;
        } // end while read bytes

        if(*err)
        {
          g_string_free(buffer, true);
          return NULL;
        }

        return buffer;
      }
      else
      {
        //g_debug("keepalive received");
        // keepalive message, ignore
        return NULL;
      } // end if sizeOfMessage > 0
    } // end if header read
  }//end if connection not null

  return NULL;
}
GString* DebugCommunicator::readMessage()
{
  if (fatalFail)
  {
    throw "fatalFail";
  }

  GError* err = NULL;
  GString* result = internalReadMessage(&err);
  if(err)
  {
    if(err->code != G_IO_ERROR_WOULD_BLOCK)
    {
      std::cerr << "[DebugServer:port " << port << "] " << "ERROR: (SocketException in readMessage) "
      << err->message << std::endl;

      if(result != NULL)
      {
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

