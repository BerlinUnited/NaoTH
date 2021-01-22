#ifndef _OptiTrackClient_H_
#define _OptiTrackClient_H_

#include <gio/gio.h>

#include <mutex>
#include <thread>

#include "OptiTrackParser.h"
#include <Representations/Infrastructure/OptiTrackData.h>
#include <Representations/Infrastructure/GPSData.h>

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
  GCancellable* cancellable; // this is used to close cancel the listening on stop
  GSocketAddress* serverAddress;
  std::thread socketThread;
  
  std::mutex  dataMutex;
  OptiTrackParser optiTrackParser;
  
public:
  void get(naoth::OptiTrackData& data);
  void get(naoth::GPSData& data, const std::string& name);
};

#endif // _OptiTrackClient_H_
