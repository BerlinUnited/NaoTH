
#include "OptiTrackClient.h"

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
    socket(NULL),
    serverAddress(NULL)
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
  cancellable = g_cancellable_new();
  
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
  std::cout << "[OptiTrackClient] file descriptor " << fd << std::endl;
  if (setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0) {
    perror("setsockopt");
    exit(1);
  }

  g_object_unref(inetAddress);
  g_object_unref(socketAddress);

  return err;
}


void OptiTrackClient::get(OptiTrackData& data)
{ 
  data.trackables.clear();
  
  std::unique_lock<std::mutex> lock(dataMutex, std::try_to_lock);
  if ( lock.owns_lock() )
  {
    data.trackables = optiTrackParser.getTrackables();
  }
}

void OptiTrackClient::get(GPSData& data, const std::string& name) 
{ 
  std::unique_lock<std::mutex> lock(dataMutex, std::try_to_lock);
  if ( lock.owns_lock() )
  {
    std::map<std::string,Pose3D>::const_iterator it = optiTrackParser.getTrackables().find(name);

    if(it != optiTrackParser.getTrackables().end()) {
      const Pose3D& p = it->second;

      Pose2D pose(p.rotation.getZAngle(), p.translation.x, p.translation.y);
      data.data = Pose3D::embedXY(pose);
    }
  }
}


OptiTrackClient::~OptiTrackClient()
{
  std::cout << "[OptiTrackClient] stop wait" << std::endl;
  exiting = true;

  // notify the socket to cancel
  g_cancellable_cancel(cancellable);
  
  if(socketThread.joinable()) {
    socketThread.join();
  }

  if(socket != NULL) {
    g_object_unref(socket);
  }
  
  g_object_unref(cancellable);

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
  std::lock_guard<std::mutex> lock(dataMutex);
  
  optiTrackParser.reset();
  std::string msg(data, dataSize);
  std::string cmd;
  
  try {
    cmd = optiTrackParser.parseMessage(msg);
  } catch (const std::length_error& le) {
    std::cout << "[OptiTrackClient]: error in parseMessage " << le.what() << std::endl;
  }
  
  if(!cmd.empty()) {
    send(cmd.c_str(), cmd.size());
  }
}

void OptiTrackClient::socketLoop()
{
  if(socket == NULL) {
    return;
  }

  while(!exiting)
  {
    GSocketAddress* senderAddress = NULL;
    
    GError *error;
    gssize size = g_socket_receive_from(socket, &senderAddress,
                                     readBuffer, readBufferSize,
                                     cancellable, &error);
                                     
    if(error && g_error_matches (error, G_IO_ERROR, G_IO_ERROR_CANCELLED)) {
      std::cout << "[OptiTrackClient] calcel socket on request" << std::endl;
      g_error_free(error);
    } else if(error) {
      std::cerr << "[OptiTrackClient] " << error->message << std::endl;
      g_error_free(error);
    }
                             
    if(senderAddress != NULL)
    {
      // construct a proper return address from the receiver
      GInetAddress* rawAddress = g_inet_socket_address_get_address(G_INET_SOCKET_ADDRESS(senderAddress));
      if(serverAddress != NULL) {
        g_object_unref(serverAddress);
      }
      serverAddress = g_inet_socket_address_new(rawAddress, static_cast<guint16>(commandPort));
      g_object_unref(senderAddress);
    }

    // parse
    handleMessage(readBuffer, size);
  }
}

