/**
 * @file BroadCaster.cpp
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 *
 */

#include "BroadCaster.h"

#include <Tools/ThreadUtil.h>

#ifndef WIN32
#include <sys/socket.h>
#else
#include <winsock.h>
#if !defined(socklen_t)
typedef int socklen_t;
#endif
#endif

#include "Tools/Communication/NetAddr.h"
#include "Tools/Debug/NaoTHAssert.h"

using namespace std;
using namespace naoth;

#include <sstream>

BroadCaster::BroadCaster(const std::string& interfaceName, unsigned int port)
 :exiting(false), socket(NULL), broadcastAddress(NULL),
    interfaceName(interfaceName), port(port),
    messagesWithoutInterface(0),
    // try to query broadcast address in every frame
    queryAddressPause(1)
{
  GError* err = NULL;
  socket = g_socket_new(G_SOCKET_FAMILY_IPV4, G_SOCKET_TYPE_DATAGRAM, G_SOCKET_PROTOCOL_UDP, &err);

  if(err)
  {
    std::cout << "[WARN] could not initialize BroadCaster properly: " << err->message << std::endl;
    g_error_free(err);
    return;
  }

  g_socket_set_blocking(socket, true);
  int broadcastFlag = 1;
  
  #ifndef WIN32
    setsockopt(g_socket_get_fd(socket), SOL_SOCKET, SO_BROADCAST, (const char*)(&broadcastFlag), static_cast<socklen_t> (sizeof(int)));
  #else
    setsockopt(g_socket_get_fd(socket), SOL_SOCKET, SO_BROADCAST, (const char*)(&broadcastFlag), (sizeof(int)));
  #endif

  queryBroadcastAddress();

  socketThread = std::thread(&BroadCaster::loop, this);
  ThreadUtil::setPriority(socketThread, ThreadUtil::Priority::lowest);

  stringstream s;
  s << "BC " << interfaceName << ":" << port;
  ThreadUtil::setName(socketThread, s.str());
}

bool BroadCaster::queryBroadcastAddress()
{
  string broadcast = NetAddr::getBroadcastAddr(interfaceName);
  if("unknown" != broadcast && "" != broadcast)
  {
    GInetAddress* address = g_inet_address_new_from_string(broadcast.c_str());
    if(broadcastAddress != NULL) {
      g_object_unref(broadcastAddress);
    }
    broadcastAddress = g_inet_socket_address_new(address, static_cast<unsigned short>(port));
    g_object_unref(address);
    std::cout << "[INFO] BroadCaster configured (" << interfaceName << ", " << broadcast << ", " << port << ")" << std::endl;
    return true;
  } else {
    std::cerr << "[BroadCaster] WARNING: unable to get broadcast address (" << interfaceName << ", " << broadcast << ", " << port << ")" << std::endl;
  }
  return false;
}

BroadCaster::~BroadCaster()
{
  std::cout << "[BroadCaster] stop wait" << std::endl;
  exiting = true;
  messageCond.notify_all(); // tell socket thread to exit

  if(socketThread.joinable()) {
    socketThread.join();
  }

  if(broadcastAddress != NULL) {
    g_object_unref(broadcastAddress);
  }
  std::cout << "[BroadCaster] stop done" << std::endl;
}

void BroadCaster::send(const std::string& data)
{
  if ( data.empty() ) {
    return;
  }

  std::unique_lock<std::mutex> lock(messageMutex, std::try_to_lock);
  if ( lock.owns_lock() ) {
    message = data;
    lock.unlock();
    messageCond.notify_all();
  }
}

void BroadCaster::send(std::list<std::string>& msgs)
{
  if ( msgs.empty() ) {
    return;
  }

  std::unique_lock<std::mutex> lock(messageMutex, std::try_to_lock);
  if ( lock.owns_lock() ) {
    messages = msgs;
    lock.unlock();
    messageCond.notify_all(); // tell socket thread to sends
  }
}

void BroadCaster::loop()
{
  while(!exiting)
  {
    std::unique_lock<std::mutex> lock(messageMutex);
    // wait until it is necessary to send data
    while ( message.empty() && messages.empty() && !exiting )
    {
      messageCond.wait(lock);
    }

    // send data via socket
    if ( !message.empty() )
    {
      socketSend(message);
      message.clear();
    }

    for(list<string>::const_iterator iter=messages.begin(); iter!=messages.end(); ++iter)
    {
      socketSend(*iter);
    }
    messages.clear();

  }
}

void BroadCaster::socketSend(const std::string& data)
{

  if(broadcastAddress == NULL) {
    messagesWithoutInterface++;
    if(messagesWithoutInterface % queryAddressPause == 0)
    {
      // attempt to get a the broadcast address from the interface again
      queryBroadcastAddress();
    }
  }

  if(broadcastAddress == NULL)
  {
    // if still not valid do nothing
    return;
  }

  GError *error = NULL;
  int result = static_cast<int> (g_socket_send_to(socket, broadcastAddress, data.c_str(), data.size(), NULL, &error));
  if (error)
  {
    std::cout << "[WARN] g_socket_send_to error: " << error->message << std::endl;
    g_error_free(error);
  }
  else if ( result != static_cast<int>(data.size()) )
  {
    std::cout << "[WARN] broadcast error, sended size = " << result << std::endl;
  }
}

