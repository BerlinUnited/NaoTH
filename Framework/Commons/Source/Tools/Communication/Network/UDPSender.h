#ifndef UDPSENDER_H
#define UDPSENDER_H

#include <gio/gio.h>

#include <string>
#include <mutex>
#include <thread>
#include <condition_variable>

class UDPSender
{
public:
  UDPSender(std::string ip, unsigned int port) : UDPSender(ip, port, "UDPSender") {};
  UDPSender(std::string ip, unsigned int port, std::string name);
  ~UDPSender();

  void send(const std::string& data);

  std::string getIp() const { return ip; };
  unsigned int getPort() const { return port; };
  std::string getName() const { return name; };

private:
  std::string ip;
  unsigned int port;
  std::string name;

  std::string dataOut = "";
  std::mutex  dataOutMutex;
  std::condition_variable dataOutCond;

  bool exiting = false;
  GCancellable* cancelable;
  std::thread socketThread;

  GSocketAddress* address = NULL;
  GSocket* socket = NULL;

  GError* bindAddress();
  void socketSend(const std::string& data);
  void socketLoop();
};

#endif // UDPSENDER_H
