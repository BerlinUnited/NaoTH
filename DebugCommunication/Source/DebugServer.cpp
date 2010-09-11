/* 
 * File:   DebugServer.cpp
 * Author: thomas
 * 
 * Created on 10. September 2010, 15:38
 */

#include <string>
#include <stdlib.h>

#include "DebugServer.h"

void incomingCallback(DebugServer* server)
{
  server->handleIncoming();
}

DebugServer::DebugServer()
: loop(NULL),socket(NULL), socketService(NULL)
{

  highLevelSocketTest();
  //lowlevelSocketTest();

}

void DebugServer::highLevelSocketTest()
{
  GError *err = NULL;
  g_message("Creating service on port 12345");
  socketService = g_socket_service_new();
  g_socket_listener_add_inet_port(G_SOCKET_LISTENER(socketService), 12345, NULL, &err);
  if(err) g_error("%s", err->message);

  g_signal_connect (socketService, "incoming",
			G_CALLBACK (incomingCallback), this);

  g_message("Starting the service");
  g_socket_service_start(socketService);

 GMainLoop *loop = NULL;
 loop = g_main_loop_new (NULL, FALSE);
 g_main_loop_run (loop);


}

void DebugServer::handleIncoming()
{
  g_message("Incoming Message!");
  exit(0);
}

void DebugServer::lowlevelSocketTest()
{
  GError *err = NULL;

  g_message("Creating new socket object");

  socket = g_socket_new(G_SOCKET_FAMILY_IPV4, G_SOCKET_TYPE_STREAM, G_SOCKET_PROTOCOL_TCP, &err);
  GInetAddress* inetAddress = g_inet_address_new_any(G_SOCKET_FAMILY_IPV4);

  GSocketAddress* socketAddress = g_inet_socket_address_new(inetAddress, 12345);

  g_message("Binding socket to port %d", 12345);
  g_socket_bind(socket, socketAddress, true, &err);
  if(err) g_error("%s", err->message);

  g_message("Listen");
  g_socket_listen(socket, &err);
  if(err) g_error("%s", err->message);

  g_message("Set to unblocking mode");
  g_socket_set_blocking(socket, false);

  GSocket* activeConnection = NULL;
  unsigned int counter = 0;
  while (activeConnection == NULL && counter < 30)
  {
    g_message("Checking for client");
    activeConnection = g_socket_accept(socket, NULL, NULL);

    counter++;
    sleep(1);
  }

  if (activeConnection != NULL)
  {
    g_message("Hey, someone is connected!");
    std::string msg = "Good morning, the DebugServer is not openend yet. Please be patient.\n";
    g_socket_send(activeConnection, msg.c_str(), msg.size(), NULL, NULL);

    g_socket_close(activeConnection, NULL);
    g_object_unref(activeConnection);
  }
}

DebugServer::~DebugServer()
{
  if(socket != NULL)
  {
    g_object_unref(socket);
  }
  if(socketService != NULL)
  {
    g_object_unref(socketService);
  }
  if(loop != NULL)
  {
    g_main_loop_quit (loop);
    g_object_unref(loop);
  }
}

