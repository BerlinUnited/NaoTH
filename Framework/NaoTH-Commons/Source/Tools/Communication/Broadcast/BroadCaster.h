/**
 * @file BroadCaster.h
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 *
 */

#ifndef BROAD_CASTER_H
#define BROAD_CASTER_H

#include <string>
#include <glib.h>
#include <gio/gio.h>

namespace naoth
{

class BroadCaster
{
public:
  BroadCaster(const std::string& interface, unsigned int port);

  ~BroadCaster();

  void send(const std::string& data);

  void loop();

private:
  bool exiting;
  GSocket* socket;
  GSocketAddress* broadcastAddress;
  GThread* socketThread;
  GMutex*  messageMutex;
  GCond* messageCond;
  std::string message;
};


} // namespace naoth

#endif // BROAD_CASTER_H
