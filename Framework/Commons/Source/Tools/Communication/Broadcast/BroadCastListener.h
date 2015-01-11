/**
 * @file BroadCastListener.cpp
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 *
 */

#ifndef BROADCASTLISTENER_H
#define BROADCASTLISTENER_H

#include <string>
#include <vector>
#include <glib.h>
#include <gio/gio.h>

namespace naoth
{
class BroadCastListener
{

public:
  BroadCastListener(unsigned int port, unsigned int buffersize=4096);

  void receive(std::vector<std::string>& data);

  virtual ~BroadCastListener();

  void loop();

private:
  unsigned int bufferSize;
  bool exiting;
  GSocket* socket;
  char* buffer;
  GThread* socketThread;
  std::vector<std::string> messageIn;
  GMutex*  messageInMutex;

  GError* bindAndListen(unsigned int port);
};
} // namespace naoth


#endif // BROADCASTLISTENER_H
