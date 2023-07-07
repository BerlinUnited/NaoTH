#include "UDPSender.h"

#include <PlatformInterface/Platform.h>
#include "Tools/Communication/NetAddr.h"
#include <Tools/ThreadUtil.h>

#ifdef WIN32
#include <winsock.h>
#else // Linux/MACOS
#include <sys/socket.h>
#endif

using namespace naoth;
using namespace std;


UDPSender::UDPSender(std::string ip, unsigned int port, std::string name)
  : ip(ip), port(port), name(name)
{
  GError* err = bindAddress();
  if(err)
  {
    std::cout << "[WARN] " << name << " could not listen for " << ip << ":" << port << ": " << err->message << std::endl;
    socket = NULL;
    g_error_free(err);
  }
  else
  {
    std::cout << "[INFO] " << name << " start socket thread for " << ip << ":" << port << std::endl;
    cancelable = g_cancellable_new();
    socketThread = std::thread(&UDPSender::socketLoop, this);
    ThreadUtil::setPriority(socketThread, ThreadUtil::Priority::lowest);
    ThreadUtil::setName(socketThread, name);
  }
}

UDPSender::~UDPSender()
{
  std::cout << "[" << name << "] stop wait" << std::endl;
  // request the thread to stop
  exiting = true;
  dataOutCond.notify_all();

  // notify all waiting connections to cancel
  g_cancellable_cancel(cancelable);

  if(socketThread.joinable()) {
    socketThread.join();
  }

  if(socket != NULL) {
    g_object_unref(socket);
  }

  if(address != NULL) {
    g_object_unref(address);
  }

  g_object_unref(cancelable);
  std::cout << "[" << name << "] stop done" << std::endl;
}

GError* UDPSender::bindAddress()
{
  GError* err = NULL;
  socket = g_socket_new(G_SOCKET_FAMILY_IPV4, G_SOCKET_TYPE_DATAGRAM, G_SOCKET_PROTOCOL_UDP, &err);
  
  if(err) { 
    return err;
  }

  g_socket_set_blocking(socket, true);

  // NOTE:
  // Set the broadcast option directly. GLib spoorts it starting version 2.36.
  // Linux and Windows let you set a single-byte value from an int,
  // but most other platforms don't.
  // https://github.com/GNOME/glib/blob/main/gio/gsocket.c#L6340
  // TODO: the following might not work on MACOS

#ifdef WIN32
  // https://learn.microsoft.com/en-us/windows/win32/api/winsock/nf-winsock-setsockopt
  // https://learn.microsoft.com/en-us/windows/win32/winprog/windows-data-types
  BOOL broadcastFlag = TRUE;
  setsockopt(g_socket_get_fd(socket), SOL_SOCKET, SO_BROADCAST, (const char*)(&broadcastFlag), sizeof(broadcastFlag));
#else // Linux/MACOS
  // https://linux.die.net/man/3/setsockopt
  int broadcastFlag = 1;
  setsockopt(g_socket_get_fd(socket), SOL_SOCKET, SO_BROADCAST, (const char*)(&broadcastFlag), static_cast<socklen_t> (sizeof(int)));
#endif

  GInetAddress* inet_address = g_inet_address_new_from_string(ip.c_str());
  address = g_inet_socket_address_new(inet_address, port);

  g_object_unref(inet_address);

  return err;
}

void UDPSender::send(const std::string& data)
{
  if (data.empty()) {
    return;
  }

  std::unique_lock<std::mutex> lock(dataOutMutex, std::try_to_lock);
  if (lock.owns_lock())
  {
    dataOut = data;
    lock.unlock();
    dataOutCond.notify_all();
  }
}

void UDPSender::socketSend(const std::string& data)
{
  GError *error = NULL;
  if(address != NULL)
  {
    gssize result = g_socket_send_to(socket, address, data.c_str(), data.size(), cancelable, &error);
    if ( result != data.size() ) {
      std::cout << "[WARN] " << name << "::socketSend, sended size = " <<  result << std::endl;
    }
    if (error) {
      std::cout << "[WARN] " << name << " g_socket_send_to error: " << error->message << std::endl;
      g_error_free(error);
    }
  }
}

void UDPSender::socketLoop()
{
  while(!exiting && socket != NULL)
  {
    std::unique_lock<std::mutex> lock(dataOutMutex);
    // wait until it is necessary to send data
    while (dataOut.empty() && !exiting && socket != NULL)
    {
      dataOutCond.wait(lock);
    }

    if (!dataOut.empty())
    {
        socketSend(dataOut);
        dataOut.clear();
    }
  }
}
