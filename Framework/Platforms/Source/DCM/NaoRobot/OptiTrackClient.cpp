
#include "OptiTrackClient.h"

#include <PlatformInterface/Platform.h>
#include "Tools/Communication/NetAddr.h"
#include <Tools/ThreadUtil.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace naoth;
using namespace std;

OptiTrackClient::OptiTrackClient()
  :
    exiting(false), 
    commandPort(1510),
    dataPort(1511),
    socket(NULL)
{
  GError* err = bindAndListenMulticast(dataPort);
  if(err)
  {
    std::cout << "[WARN] could not listen for OptiTrackClient: " << err->message << std::endl;
    socket = NULL;
    g_error_free(err);
  }
  else
  {
    std::cout << "[INFO] OptiTrackClient start socket thread" << std::endl;
    socketThread = std::thread([this] {this->socketLoop();});
    ThreadUtil::setPriority(socketThread, ThreadUtil::Priority::lowest);
  }
}

GError* OptiTrackClient::bindAndListenMulticast(unsigned int port)
{
  GError* err = NULL;
  socket = g_socket_new(G_SOCKET_FAMILY_IPV4, G_SOCKET_TYPE_DATAGRAM, G_SOCKET_PROTOCOL_UDP, &err);
  if (err) return err;

  g_socket_set_blocking(socket, true);

  GInetAddress* inetAddress = g_inet_address_new_any(G_SOCKET_FAMILY_IPV4);
  GSocketAddress* socketAddress = g_inet_socket_address_new(inetAddress, static_cast<unsigned short>(port));

  g_socket_bind(socket, socketAddress, true, &err);

  struct ip_mreq mreq;
  mreq.imr_multiaddr.s_addr = inet_addr("239.255.42.99");
  mreq.imr_interface.s_addr = htonl(INADDR_ANY);

  int fd = g_socket_get_fd(socket);
  std::cout << "file descriptor " << fd << std::endl;
  if (setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0) {
    perror("setsockopt");
    exit(1);
  }

  g_object_unref(inetAddress);
  g_object_unref(socketAddress);

  return err;
}

/*
void OptiTrackClient::get(GameData& gameData)
{
  std::unique_lock<std::mutex> lock(dataMutex, std::try_to_lock);
  if ( lock.owns_lock() )
  {
    if(data.valid) {
      gameData = data;
      data.valid = false; // invalidate after copy
    } else {
      // no new message received
      gameData.valid = false;
    }
  }
}
*/

OptiTrackClient::~OptiTrackClient()
{
  exiting = true;

  if(socketThread.joinable())
  {
    socketThread.join();
  }

  if(socket != NULL) {
    g_object_unref(socket);
  }

  if(serverAddress != NULL) {
    g_object_unref(serverAddress);
  }
}

void OptiTrackClient::send(const char* data, size_t dataSize)
{
  GError *error = NULL;
  if(serverAddress != NULL)
  {
    gssize result = g_socket_send_to(socket, serverAddress, data, dataSize, NULL, &error);
    if ( result != (gssize)dataSize ) {
      std::cout << "[WARN] OptiTrackClient::send, sent size = " <<  result << std::endl;
    }
    if (error) {
      std::cout << "[WARN] g_socket_send_to error: " << error->message << std::endl;
      g_error_free(error);
    }
  }
}

void OptiTrackClient::handleMessage(const char* data, size_t dataSize) 
{
  optiTrackParser.reset();
  std::string msg(data, dataSize);
  optiTrackParser.parseMessage(msg);
  
  std::string cmd = optiTrackParser.requestDefinitions();
  send(cmd.c_str(), cmd.size());
}

void OptiTrackClient::socketLoop()
{
  if(socket == NULL) {
    return;
  }

  while(!exiting)
  {
    GSocketAddress* senderAddress = NULL;
    int size = g_socket_receive_from(socket, &senderAddress,
                                     readBuffer, readBufferSize,
                                     NULL, NULL);

    if(senderAddress != NULL)
    {
      // construct a proper return address from the receiver
      GInetAddress* rawAddress = g_inet_socket_address_get_address(G_INET_SOCKET_ADDRESS(senderAddress));
      if(serverAddress != NULL)
      {
        g_object_unref(serverAddress);
      }
      serverAddress = g_inet_socket_address_new(rawAddress, static_cast<guint16>(commandPort));
      g_object_unref(senderAddress);
    }

    // parse
    handleMessage(readBuffer, size);
  }
}

