/**
* @file BroadCaster.cpp
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
*
*/

#include "BroadCaster.h"
#include "NetUtils.h"

#include "Tools/ThreadUtil.h"
//#include "Tools/Debug/NaoTHAssert.h"

#ifdef WIN32
  #include <winsock.h>
#else // Linux/MACOS
  #include <sys/socket.h>
#endif

#include <iostream>
#include <sstream>

using namespace naoth;


BroadCaster::BroadCaster(const std::string& interfaceName, unsigned int port)
 :
  exiting(false), 
  socket(NULL), 
  broadcastAddress(NULL),
  interfaceName(interfaceName), 
  port(port),
  messagesWithoutInterface(0),
  // try to query broadcast address in every frame
  queryAddressPause(1)
{
  GError* err = bindAddress();
  if(err)
  {
    std::cout << "[WARN] could not initialize BroadCaster on inferface: " << interfaceName << ", port: " << port << ": " << err->message << std::endl;
    socket = NULL;
    g_error_free(err);
  }
  else
  {
    std::cout << "[INFO] start BroadCaster thread on inferface: " << interfaceName << ", port: " << port << std::endl;
    cancelable = g_cancellable_new();
    
    // configute and start the thread
    socketThread = std::thread(&BroadCaster::loop, this);
    ThreadUtil::setPriority(socketThread, ThreadUtil::Priority::lowest);

    std::stringstream s;
    s << "BC " << interfaceName << ":" << port;
    ThreadUtil::setName(socketThread, s.str());
  }
}

BroadCaster::~BroadCaster()
{
  std::cout << "[BroadCaster] stop wait" << std::endl;
  // request the thread to stop
  exiting = true;
  messageCond.notify_all();

  // notify all waiting connections to cancel
  g_cancellable_cancel(cancelable);

  if(socketThread.joinable()) {
    socketThread.join();
  }

  if(socket != NULL) {
    g_object_unref(socket);
  }

  if(broadcastAddress != NULL) {
    g_object_unref(broadcastAddress);
  }

  g_object_unref(cancelable);
  std::cout << "[BroadCaster] stop done" << std::endl;
}


GError* BroadCaster::bindAddress()
{
  GError* err = NULL;
  socket = g_socket_new(G_SOCKET_FAMILY_IPV4, G_SOCKET_TYPE_DATAGRAM, G_SOCKET_PROTOCOL_UDP, &err);

  if(err) { 
    return err;
  }

  g_socket_set_blocking(socket, true);

  // Enable socket to be allowed to send  packets to a broadcast address.
  // NOTE: needs newer glib 2.36
  //  g_socket_set_broadcast(socket, true);
  NetUtils::my_g_socket_set_broadcast(socket, true);


  queryBroadcastAddress();

  return err;
}

bool BroadCaster::queryBroadcastAddress()
{
  std::string broadcast = NetUtils::getBroadcastAddr(interfaceName);
  if("unknown" != broadcast && "" != broadcast)
  {
    GInetAddress* inet_address = g_inet_address_new_from_string(broadcast.c_str());

    // TODO: queryBroadcastAddress() is only called when broadcastAddress == NULL
    // delete old broadcastAddress if it was already set
    if(broadcastAddress != NULL) {
      g_object_unref(broadcastAddress);
    }
    broadcastAddress = g_inet_socket_address_new(inet_address, static_cast<guint16>(port));
    g_object_unref(inet_address);
    std::cout << "[INFO] BroadCaster configured (" << interfaceName << ", " << broadcast << ", " << port << ")" << std::endl;
    return true;
  } else {
    std::cerr << "[BroadCaster] WARNING: unable to get broadcast address (" << interfaceName << ", " << broadcast << ", " << port << ")" << std::endl;
  }
  return false;
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
    // wait until there is data to be sent
    while ( message.empty() && messages.empty() && !exiting )
    {
      messageCond.wait(lock);
    }

    // send a single message
    if ( !message.empty() )
    {
      socketSend(message);
      message.clear();
    }

    // send a list of messages
    for(const std::string& msg: messages)
    {
      socketSend(msg);
    }
    messages.clear();

  }
}

void BroadCaster::socketSend(const std::string& data)
{
  if(broadcastAddress == NULL) 
  {
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
  gssize result = g_socket_send_to(socket, broadcastAddress, data.c_str(), data.size(), cancelable, &error);
  if (error)
  {
    std::cout << "[WARN] g_socket_send_to error: " << error->message << std::endl;
    g_error_free(error);
  }
  else if ( result != static_cast<int>(data.size()) )
  {
    std::cout << "[WARN] broadcast error wrong size sent: data size = " << data.size() << ", sent size = " << result << std::endl;
  }
}

