/**
 * @file BroadCaster.cpp
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 *
 */

#include "BroadCaster.h"
#include <sys/socket.h>
#include "Tools/Communication/MacAddr.h"
#include "Tools/Debug/NaoTHAssert.h"

using namespace std;
using namespace naoth;

void* broadcaster_static_loop(void* b)
{
  BroadCaster* bc = static_cast<BroadCaster*> (b);
  bc->loop();
  return NULL;
}

BroadCaster::BroadCaster(const std::string& interface, unsigned int port)
  :exiting(false), socket(NULL), broadcastAddress(NULL),
    socketThread(NULL), messageMutex(NULL), messageCond(NULL)
{
  GError* err = NULL;
  socket = g_socket_new(G_SOCKET_FAMILY_IPV4, G_SOCKET_TYPE_DATAGRAM, G_SOCKET_PROTOCOL_UDP, &err);

  if(err)
  {
    g_warning("could not initialize BroadCaster properly: %s", err->message);
    g_error_free(err);
    return;
  }

  if (!g_thread_supported())
    g_thread_init(NULL);

  messageMutex = g_mutex_new();
  messageCond = g_cond_new();

  g_socket_set_blocking(socket, true);
  int broadcastFlag = 1;
  setsockopt(g_socket_get_fd(socket), SOL_SOCKET, SO_BROADCAST, &broadcastFlag, sizeof(int));

  string broadcast = getBroadcastAddr(interface);
  GInetAddress* address = g_inet_address_new_from_string(broadcast.c_str());
  broadcastAddress = g_inet_socket_address_new(address, port);
  g_object_unref(address);

  g_message("BroadCaster start thread (%s, %d)", broadcast.c_str(), port);
  socketThread = g_thread_create(broadcaster_static_loop, this, true, NULL);
  ASSERT(socketThread != NULL);
  g_thread_set_priority(socketThread, G_THREAD_PRIORITY_LOW);
}

BroadCaster::~BroadCaster()
{
  exiting = true;
  g_cond_signal(messageCond); // tell socket thread to exit

  if ( socketThread )
    g_thread_join(socketThread);
  g_mutex_free(messageMutex);
  g_cond_free(messageCond);
  if(broadcastAddress != NULL)
  {
    g_object_unref(broadcastAddress);
  }
}

void BroadCaster::send(const std::string& data)
{
  if ( data.empty() )
    return;

  if ( g_mutex_trylock(messageMutex) )
  {
    message = data;
    g_cond_signal(messageCond); // tell socket thread to send
    g_mutex_unlock(messageMutex);
  }
}

void BroadCaster::loop()
{
  while(!exiting)
  {
    g_mutex_lock(messageMutex);
    // wait until it is necessary to send data
    while ( message.empty() )
    {
      g_cond_wait(messageCond, messageMutex);
    }

    // send data via socket
    if(broadcastAddress)
    {
      GError *error = NULL;
      gssize result = g_socket_send_to(socket, broadcastAddress, message.c_str(), message.size(), NULL, &error);
      if ( result != message.size() )
      {
        g_warning("broadcast error, sended size = %ld", result);
      }
      if (error)
      {
        g_warning("g_socket_send_to error: %s", error->message);
        g_error_free(error);
      }
    }

    message.clear();
    g_mutex_unlock(messageMutex);
  }
}
