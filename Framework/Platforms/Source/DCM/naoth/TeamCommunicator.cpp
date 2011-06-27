#include "TeamCommunicator.h"
#include "PlatformInterface/Platform.h"

#include <sys/socket.h>

#define TEAMCOMM_MAX_MSG_SIZE 4096

using namespace naoth;

TeamCommunicator::TeamCommunicator()
  : initialized(false), socket(NULL),
    lanBroadcastAddress(NULL), wlanBroadcastAddress(NULL)
{
  buffer = new char[TEAMCOMM_MAX_MSG_SIZE];

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
  }

}

GError* TeamCommunicator::bindAndListen(unsigned int port)
{
  GError* err = NULL;
  socket = g_socket_new(G_SOCKET_FAMILY_IPV4, G_SOCKET_TYPE_DATAGRAM, G_SOCKET_PROTOCOL_UDP, &err);
  if(err) return err;

  g_socket_set_blocking(socket, false);

  bool broadcast = true;
  setsockopt(g_socket_get_fd(socket), SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(bool));

  GInetAddress* inetAddress = g_inet_address_new_any(G_SOCKET_FAMILY_IPV4);
  GSocketAddress* socketAddress = g_inet_socket_address_new(inetAddress, port);

  g_socket_bind(socket, socketAddress, true, &err);

  g_object_unref(inetAddress);
  g_object_unref(socketAddress);

  if (err) return err;
}

void TeamCommunicator::send(const std::string& data)
{
  if(data.size() > TEAMCOMM_MAX_MSG_SIZE)
  {
    g_warning("tried to send too big TeamComm message, please adjust TEAMCOMM_MAX_MSG_SIZE");
  }
  else
  {
    if(lanBroadcastAddress)
    {
      g_socket_send_to(socket, lanBroadcastAddress, data.c_str(), data.size(), NULL, NULL);
    }
    if(wlanBroadcastAddress)
    {
      g_socket_send_to(socket, wlanBroadcastAddress, data.c_str(), data.size(), NULL, NULL);
    }
  }
}

void TeamCommunicator::receive(vector<string>& data)
{
  if(socket == NULL)
    return;

  gssize result = -1;
  do
  {
    result = g_socket_receive(socket, buffer,
                              TEAMCOMM_MAX_MSG_SIZE, NULL, NULL);
    if(result > 0)
    {
      data.push_back(string(buffer, result));
    }
  } while(result > 0);
}

TeamCommunicator::~TeamCommunicator()
{
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
