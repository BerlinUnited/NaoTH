/* 
 * File:   DebugCommunicator.h
 * Author: thomas
 *
 * Created on 8. März 2009, 19:31
 */

#ifndef _DEBUGCOMMUNICATOR_H
#define	_DEBUGCOMMUNICATOR_H

#include "BidirectionalCommunicator.h"

#include <gio/gio.h>

class DebugCommunicator : public BidirectionalCommunicator
{
public:
  DebugCommunicator(unsigned short port);
  virtual ~DebugCommunicator();

  virtual void init();
  virtual void sendMessage(const std::string& data);
  virtual bool hasMessageInQueue();
  virtual std::string peekMessage();

  /**
   *  Will send a message with invalid id and no payload in order to check if
   *  the connection is still alive.
   */
  void sendBeacon();

private:
  GSocket* serverSocket;
  GSocket* connection;

  std::string buffer;
  bool bufferValid;
  int lastID;

  unsigned short port;

  bool fatalFail;

  void triggerRead();
  GError* sendMessage(const std::string& data, int id);
  GError* internalInit();
  GError* triggerConnect();
  GError* triggerReceive();
  void disconnect();

};

#endif	/* _DEBUGCOMMUNICATOR_H */

