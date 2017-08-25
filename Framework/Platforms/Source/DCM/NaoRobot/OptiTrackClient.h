#ifndef _OptiTrackClient_H_
#define _OptiTrackClient_H_

#include <gio/gio.h>

#include <mutex>
#include <thread>

#include "OptiTrackParser.h"

class OptiTrackClient
{
public:
  OptiTrackClient();

  ~OptiTrackClient();

private:
  GError* bindAndListenMulticast(unsigned int port);
  void send(const char* data, size_t dataSize);
  void socketLoop();
  void handleMessage(const char* data, size_t dataSize);
  
private:
  bool exiting;
  int commandPort;
  int dataPort;
  
  static const unsigned int readBufferSize = 32768; // 32KB
  char readBuffer[readBufferSize];
  
  GSocket* socket;
  GSocketAddress* serverAddress;
  std::thread socketThread;

  // results
  //naoth::GameData data;
  std::mutex  dataMutex;

public:
  OptiTrackParser optiTrackParser;
};

#endif // _OptiTrackClient_H_
