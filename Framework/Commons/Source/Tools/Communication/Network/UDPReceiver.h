/**
 * @file UDPReceiver.cpp
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 *
 */

#ifndef UDPReceiver_H
#define UDPReceiver_H

#include <string>
#include <vector>
#include <glib.h>
#include <gio/gio.h>

#include <mutex>
#include <thread>

namespace naoth
{
class UDPReceiver
{

public:
  UDPReceiver(unsigned int port, unsigned int buffersize=4096);

  void receive(std::vector<std::string>& data);

  virtual ~UDPReceiver();

  void loop();

private:
  unsigned int bufferSize;
  bool exiting;

  GSocket* socket;
  GCancellable* socket_cancelable;

  char* buffer;
  std::thread socketThread;
  std::vector<std::string> messageIn;
  std::mutex messageInMutex;

  GError* bindAndListen(unsigned int port);
};
} // namespace naoth


#endif // UDPReceiver_H
