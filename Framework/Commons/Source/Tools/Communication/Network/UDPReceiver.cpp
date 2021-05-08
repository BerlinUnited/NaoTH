/**
 * @file UDPReceiver.cpp
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 *
 */

#include "UDPReceiver.h"
#include "Tools/Debug/NaoTHAssert.h"

#include <Tools/ThreadUtil.h>

using namespace naoth;

#include <sstream>
using namespace std;

UDPReceiver::UDPReceiver(unsigned int port, unsigned int buffersize)
  : exiting(false), socket(NULL)
{
  bufferSize = buffersize;
  buffer = new char[buffersize];

  GError* err = bindAndListen(port);

  if(err)
  {
    std::cout << "[WARN] could not initialize TeamCommSocket properly: " << err->message << std::endl;;
    g_error_free(err);
    return;
  }

  socket_cancelable = g_cancellable_new();

  std::cout << "[INFO] BroadCastLister start thread (" << port << ")" << std::endl;

  socketThread = std::thread(&UDPReceiver::loop, this);
  ThreadUtil::setPriority(socketThread, ThreadUtil::Priority::lowest);

  stringstream s;
  s << "Listen " << port;
  ThreadUtil::setName(socketThread, s.str());
}

UDPReceiver::~UDPReceiver()
{
  std::cout << "[UDPReceiver] stop wait" << std::endl;
  exiting = true;

  // notify all waiting connections to cancel
  g_cancellable_cancel(socket_cancelable);

  if(socketThread.joinable()) {
    socketThread.join();
  }

  if(socket != NULL) {
    g_object_unref(socket);
  }

  g_object_unref(socket_cancelable);
  delete [] buffer;

  std::cout << "[UDPReceiver] stop done" << std::endl;
}

GError* UDPReceiver::bindAndListen(unsigned int port)
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

void UDPReceiver::receive(std::vector<std::string>& data)
{
  data.clear();
  std::unique_lock<std::mutex> lock(messageInMutex, std::try_to_lock);
  if ( lock.owns_lock())
  {
    if ( !messageIn.empty() )
    {
      data = messageIn;
      messageIn.clear();
    }
  }
}

void UDPReceiver::loop()
{
  if(socket == NULL) {
    return;
  }

  while(!exiting)
  {
    gssize result = g_socket_receive(socket, buffer, bufferSize, socket_cancelable, NULL);
    if(result > 0)
    {
      std::lock_guard<std::mutex> lock(messageInMutex);
      messageIn.push_back(std::string(buffer, static_cast<size_t>(result)));
    }
  }
}
