/* 
 * File:   DebugCommunicator.h
 * Author: thomas
 *
 * Created on 8. März 2009, 19:31
 */

#ifndef _DEBUGCOMMUNICATOR_H
#define	_DEBUGCOMMUNICATOR_H

#include <gio/gio.h>


class DebugCommunicator
{
public:
  DebugCommunicator(unsigned short port);
  virtual ~DebugCommunicator();

  void init();
  void sendMessage(const std::string& data);
  std::string* readMessage();

private:
  GSocket* serverSocket;
  GSocket* connection;

  unsigned short port;

  bool fatalFail;

  GError* internalSendMessage(const std::string& data);
  GError* internalInit();
  GError* triggerConnect();
  std::string* triggerReceive(GError** err);
  void disconnect();
};

#endif	/* _DEBUGCOMMUNICATOR_H */

