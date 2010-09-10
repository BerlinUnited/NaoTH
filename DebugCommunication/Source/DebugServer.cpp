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
  socket = g_socket_new(G_SOCKET_FAMILY_IPV4, G_SOCKET_TYPE_STREAM, G_SOCKET_PROTOCOL_TCP, &err);
  GInetAddress* inetAddress = g_inet_address_new_any(G_SOCKET_FAMILY_IPV4);
  g_assert(err == NULL);
  GSocketAddress* socketAddress = g_inet_socket_address_new(inetAddress, 12345);
  g_socket_bind(socket, socketAddress, true, &err);
  g_assert(err == NULL);
  g_socket_listen(socket, &err);
  g_assert(err == NULL);
}


DebugServer::~DebugServer()
{
  g_object_unref(socket);
}

