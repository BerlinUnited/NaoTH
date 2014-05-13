/**
 * @file BroadCastListener.cpp
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 *
 */

#include "BroadCastListener.h"
#include "Tools/Debug/NaoTHAssert.h"

using namespace naoth;

void* broadcastlistener_static_loop(void* b)
{
  BroadCastListener* bl = static_cast<BroadCastListener*> (b);
  bl->loop();
  return NULL;
}

BroadCastListener::BroadCastListener(unsigned int port, unsigned int buffersize)
  : exiting(false), socket(NULL),
    socketThread(NULL)
{
  bufferSize = buffersize;
  buffer = new char[buffersize];

  if (!g_thread_supported()) {
    g_thread_init(NULL);
  }
  messageInMutex = g_mutex_new();

  GError* err = bindAndListen(port);

  if(err)
  {
    g_warning("could not initialize TeamCommSocket properly: %s", err->message);
    g_error_free(err);
    return;
  }

  g_message("BroadCastLister start thread (%d)", port);
  socketThread = g_thread_create(broadcastlistener_static_loop, this, true, NULL);
  ASSERT(socketThread != NULL);
  g_thread_set_priority(socketThread, G_THREAD_PRIORITY_LOW);
}

BroadCastListener::~BroadCastListener()
{
  exiting = true;

  if ( socketThread )
  {
    g_thread_join(socketThread);
  }
  g_mutex_free(messageInMutex);

  if(socket != NULL)
  {
    g_object_unref(socket);
  }

  delete [] buffer;
}

GError* BroadCastListener::bindAndListen(unsigned int port)
{
  GError* err = NULL;
  socket = g_socket_new(G_SOCKET_FAMILY_IPV4, G_SOCKET_TYPE_DATAGRAM, G_SOCKET_PROTOCOL_UDP, &err);
  if (err) return err;

  g_socket_set_blocking(socket, true);

  GInetAddress* inetAddress = g_inet_address_new_any(G_SOCKET_FAMILY_IPV4);
  GSocketAddress* socketAddress = g_inet_socket_address_new(inetAddress, static_cast<unsigned short>(port));

  g_socket_bind(socket, socketAddress, true, &err);

  g_object_unref(inetAddress);
  g_object_unref(socketAddress);

  return err;
}

void BroadCastListener::receive(std::vector<std::string>& data)
{
  data.clear();
  if ( g_mutex_trylock(messageInMutex) )
  {
    if ( !messageIn.empty() )
    {
      data = messageIn;
      messageIn.clear();
    }
    g_mutex_unlock(messageInMutex);
  }
}

void BroadCastListener::loop()
{
  if(socket == NULL) {
    return;
  }

  while(!exiting)
  {
    gssize result = g_socket_receive(socket, buffer,
                                     bufferSize, NULL, NULL);
    if(result > 0)
    {
      g_mutex_lock(messageInMutex);
      messageIn.push_back(std::string(buffer, result));
      g_mutex_unlock(messageInMutex);
    }
  }
}
