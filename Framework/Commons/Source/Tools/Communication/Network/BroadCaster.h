/**
 * @file BroadCaster.h
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 *
 */

#ifndef BROAD_CASTER_H
#define BROAD_CASTER_H

#include <string>
#include <list>
#include <glib.h>
#include <gio/gio.h>

#include <thread>
#include <mutex>
#include <condition_variable>

namespace naoth
{

class BroadCaster
{
public:
  BroadCaster(const std::string& interfaceName, unsigned int port);
  ~BroadCaster();

  // send a single message
  void send(const std::string& data);

  // send a list of messages
  void send(std::list<std::string>& msgs);

private:
  void loop();

  void socketSend(const std::string& data);

  bool queryBroadcastAddress();

private:
  // threading
  bool exiting;
  std::thread socketThread;
  std::mutex  messageMutex;
  std::condition_variable messageCond;

  // socket
  GSocket* socket;
  GSocketAddress* broadcastAddress;
  const std::string interfaceName;
  const unsigned int port;
  
  // internal message buffers
  std::string message;
  std::list<std::string> messages;

  // monior the state of the broadcastAddress
  /** Number of message where no broadcast information was available since the interface was down */
  int messagesWithoutInterface;
  /** The number of message we should wait before re-attempting to query the broadcast address */
  const int queryAddressPause;
};

} // namespace naoth

#endif // BROAD_CASTER_H
