/**
 * @file UDPReceiver.cpp
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 *
 */

#ifndef UDPReceiver_H
#define UDPReceiver_H

#include <gio/gio.h>

#include <string>
#include <vector>
#include <mutex>
#include <thread>

namespace naoth
{
class UDPReceiver
{

public:
  UDPReceiver(unsigned int port, unsigned int buffersize=4096);
  virtual ~UDPReceiver();

  void receive(std::vector<std::string>& data);

private:
  unsigned int bufferSize;
  bool exiting;

  GSocket* socket;
  GCancellable* cancelable;

  char* buffer;
  std::thread socketThread;
  std::vector<std::string> messageIn;
  std::mutex messageInMutex;

  GError* bindAndListen(unsigned int port);
  //void socketReceive();
  void socketLoop();
};
} // namespace naoth


#endif // UDPReceiver_H
