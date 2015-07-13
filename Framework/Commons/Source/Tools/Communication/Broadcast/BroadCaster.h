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

namespace naoth
{

class BroadCaster
{
public:
  BroadCaster(const std::string& interfaceName, unsigned int port);

  ~BroadCaster();

  void send(const std::string& data);

  void send(std::list<std::string>& msgs);

  void loop();

private:
  void socketSend(const std::string& data);

  bool queryBroadcastAddress();

private:
  bool exiting;
  GSocket* socket;
  GSocketAddress* broadcastAddress;
  GThread* socketThread;
  GMutex*  messageMutex;
  GCond* messageCond;
  std::string message;
  std::list<std::string> messages;
  const std::string& interfaceName;
  const unsigned int port;
  /** Number of frame where no broadcast information was available since the interface was down */
  int framesWithoutInterface;
  /** The number of frames we should wait before re-attempting to query the broadcast address */
  const int queryAddressPause;
};


} // namespace naoth

#endif // BROAD_CASTER_H
