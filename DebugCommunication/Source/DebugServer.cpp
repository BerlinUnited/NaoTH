/* 
 * File:   DebugServer.cpp
 * Author: thomas
 * 
 * Created on 10. September 2010, 15:38
 */

#include "DebugServer.h"

DebugServer::DebugServer()
: socket(NULL)
{
  GError *err = NULL;
 
  
  g_message("Creating new socket object");

  socket = g_socket_new(G_SOCKET_FAMILY_IPV4, G_SOCKET_TYPE_STREAM, G_SOCKET_PROTOCOL_TCP, &err);
  GInetAddress* inetAddress = g_inet_address_new_any(G_SOCKET_FAMILY_IPV4);
 
  GSocketAddress* socketAddress = g_inet_socket_address_new(inetAddress, 12345);

  g_message("Binding socket to port %d", 12345);
  g_socket_bind(socket, socketAddress, true, &err);
  g_assert(err == NULL);

  g_message("Listen");
  g_socket_listen(socket, &err);
  g_assert(err == NULL);

  g_message("Try to accept");
  GSocket* activeConnection = g_socket_accept(socket, NULL, &err);
  g_assert(err == NULL);

  g_message("Hey, someone is connected!");
  

}


DebugServer::~DebugServer()
{
  g_object_unref(socket);
}

