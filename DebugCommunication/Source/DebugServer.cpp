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
  socket = g_socket_new(G_SOCKET_FAMILY_IPV4, G_SOCKET_TYPE_STREAM, G_SOCKET_PROTOCOL_TCP, NULL);
  GInetAddress* inetAddress = g_inet_address_new_from_string("localhost");
  GSocketAddress* socketAddress = g_inet_socket_address_new(inetAddress, 12345);
  g_socket_bind(socket, socketAddress, true, NULL);
  g_socket_listen(socket, NULL);
}


DebugServer::~DebugServer()
{
  g_object_unref(socket);
}

