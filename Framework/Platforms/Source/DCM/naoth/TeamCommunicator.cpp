#include "TeamCommunicator.h"
#include "PlatformInterface/Platform.h"

#include <sys/socket.h>

#define TEAMCOMM_MAX_MSG_SIZE 4096

using namespace naoth;

void* sendLoopWrap(void* c)
{
  TeamCommunicator* comm = static_cast<TeamCommunicator*> (c);
  comm->sendLoop();
  return NULL;
}//end motionLoopWrap

void* receiveLoopWrap(void* c)
{
  TeamCommunicator* comm = static_cast<TeamCommunicator*> (c);
  comm->receiveLoop();
  return NULL;
}//end motionLoopWrap

TeamCommunicator::TeamCommunicator()
  : exiting(false), socket(NULL),
    lanBroadcastAddress(NULL), wlanBroadcastAddress(NULL)
{
  buffer = new char[TEAMCOMM_MAX_MSG_SIZE];
  if (!g_thread_supported())
    g_thread_init(NULL);
  messageInMutex = g_mutex_new();
  messageOutMutex = g_mutex_new();
  messageOutCond = g_cond_new();

  naoth::Configuration& config = naoth::Platform::getInstance().theConfiguration;
  unsigned int port = 10700;
  if(config.hasKey("teamcomm", "port"))
  {
    port = config.getInt("teamcomm", "port");
  }

  GError* err = bindAndListen(port);
  if(err)
  {
    g_warning("could not initialize teamcomm properly: %s", err->message);
    g_error_free(err);
  }
  else
  {
    g_message("Team Communication started on port %d", port);

    if(config.hasKey("teamcomm", "wlan"))
    {
      GInetAddress* address = g_inet_address_new_from_string(config.getString("teamcomm", "wlan").c_str());
      wlanBroadcastAddress = g_inet_socket_address_new(address, port);
      g_object_unref(address);
    }
    if(config.hasKey("teamcomm", "lan"))
    {
      GInetAddress* address = g_inet_address_new_from_string(config.getString("teamcomm", "lan").c_str());
      lanBroadcastAddress = g_inet_socket_address_new(address, port);
      g_object_unref(address);
    }

    g_message("TeamCommunicator start sending thread");
    sendThread = g_thread_create(sendLoopWrap, this, true, NULL);
    ASSERT(sendThread != NULL);
    g_thread_set_priority(sendThread, G_THREAD_PRIORITY_LOW);

    g_message("TeamCommunicator start receiving thread");
    receiveThread = g_thread_create(receiveLoopWrap, this, true, NULL);
    ASSERT(receiveThread != NULL);
    g_thread_set_priority(receiveThread, G_THREAD_PRIORITY_LOW);
  }
}

TeamCommunicator::~TeamCommunicator()
{
  exiting = true;

  g_thread_join(sendThread);
  g_thread_join(receiveThread);
  g_mutex_free(messageInMutex);
  g_mutex_free(messageOutMutex);
  g_cond_free(messageOutCond);

  if(socket != NULL)
  {
    g_object_unref(socket);
  }
  if(lanBroadcastAddress != NULL)
  {
    g_object_unref(lanBroadcastAddress);
  }
  if(wlanBroadcastAddress != NULL)
  {
    g_object_unref(wlanBroadcastAddress);
  }
  delete [] buffer;
}

GError* TeamCommunicator::bindAndListen(unsigned int port)
{
  GError* err = NULL;
  socket = g_socket_new(G_SOCKET_FAMILY_IPV4, G_SOCKET_TYPE_DATAGRAM, G_SOCKET_PROTOCOL_UDP, &err);
  if(err) return err;

  g_socket_set_blocking(socket, true);

  int broadcast = 1;
  setsockopt(g_socket_get_fd(socket), SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(int));

  GInetAddress* inetAddress = g_inet_address_new_any(G_SOCKET_FAMILY_IPV4);
  GSocketAddress* socketAddress = g_inet_socket_address_new(inetAddress, port);

  g_socket_bind(socket, socketAddress, true, &err);

  g_object_unref(inetAddress);
  g_object_unref(socketAddress);

  return err;
}

void TeamCommunicator::send(const std::string& data)
{
  if ( data.empty() )
    return;

  if ( g_mutex_trylock(messageOutMutex) )
  {
    messageOut = data;
    g_cond_signal(messageOutCond); // tell send thread to send
    g_mutex_unlock(messageOutMutex);
  }
}

void TeamCommunicator::send()
{
  if ( messageOut.empty() )
    return;

  if(messageOut.size() > TEAMCOMM_MAX_MSG_SIZE)
  {
    g_warning("tried to send too big TeamComm message, please adjust TEAMCOMM_MAX_MSG_SIZE");
  }
  else
  {
    if(lanBroadcastAddress)
    {
      GError *error = NULL;
      gssize result = g_socket_send_to(socket, lanBroadcastAddress, messageOut.c_str(), messageOut.size(), NULL, &error);
      if ( result != messageOut.size() )
      {
        g_warning("lan broadcast error, sended size = %d", result);
      }
      if (error)
      {
        g_warning("g_socket_send_to error: %s", error->message);
        g_error_free(error);
      }
    }
    if(wlanBroadcastAddress)
    {
      GError *error = NULL;
      gssize result = g_socket_send_to(socket, wlanBroadcastAddress, messageOut.c_str(), messageOut.size(), NULL, &error);
      if ( result != messageOut.size() )
      {
        g_warning("wlan broadcast error, sended size = %d", result);
      }
      if (error)
      {
        g_warning("g_socket_send_to error: %s", error->message);
        g_error_free(error);
      }
    }
  }

  messageOut.clear();
}

void TeamCommunicator::receive(vector<string>& data)
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

void TeamCommunicator::sendLoop()
{
  while(!exiting)
  {
    g_mutex_lock(messageOutMutex);
    // wait until it is necessary to send data
    while ( messageOut.empty() )
    {
      g_cond_wait(messageOutCond, messageOutMutex);
    }
    send();
    g_mutex_unlock(messageOutMutex);
  }
}

void TeamCommunicator::receiveLoop()
{
  if(socket == NULL)
    return;

  while(!exiting)
  {
    gssize result = g_socket_receive(socket, buffer,
                                     TEAMCOMM_MAX_MSG_SIZE, NULL, NULL);
    if(result > 0)
    {
      g_mutex_lock(messageInMutex);
      messageIn.push_back(string(buffer, result));
      g_mutex_unlock(messageInMutex);
    }
  }
}
