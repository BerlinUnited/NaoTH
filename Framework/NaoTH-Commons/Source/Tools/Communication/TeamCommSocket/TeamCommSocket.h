#ifndef NAOTH_TEAMCOMMUNICATOR_H
#define NAOTH_TEAMCOMMUNICATOR_H

#include <string>
#include <vector>
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

class BroadCastLister
{

public:
  BroadCastLister(unsigned int port, unsigned int buffersize=4096);

  void receive(std::vector<std::string>& data);

  virtual ~BroadCastLister();

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

#endif // NAOTH_TEAMCOMMUNICATOR_H
