/* 
 * File:   DebugServer.h
 * Author: thomas
 *
 * Created on 10. September 2010, 15:38
 */

#ifndef DEBUGSERVER_H
#define DEBUGSERVER_H

#include <glib.h>
#include <libb64/decode.h>
#include <libb64/encode.h>
#include <Tools/DataStructures/DestructureSentinel.h>
#include <Representations/Infrastructure/DebugMessage.h>

#include "DebugCommunicator.h"

class DebugServer
{
public:
  DebugServer();
  virtual ~DebugServer();

  // TODO: why is this virtual?
  virtual void start(unsigned short port, bool threaded = false);
  
  /** 
  * Set the time after which the connection should be closed in case of inactivity.
  * 0 - means the connection never times out.
  */
  void setTimeOut(unsigned int t) {
    comm.setTimeOut(t);
  }
  
  void getDebugMessageIn(naoth::DebugMessageIn& buffer);
  void setDebugMessageOut(const naoth::DebugMessageOut& buffer);

private:

  long long lastSendTime;
  long long lastReceiveTime;

  /** Communication interface */
  DebugCommunicator comm;

  GThread* connectionThread;

  GAsyncQueue* commands;
  GAsyncQueue* answers;

  GMutex* m_executing;
  GMutex* m_abort;

  bool abort;

  void mainConnection();
  void receiveAll();
  void sendAll();

  void parseCommand(GString* cmdRaw, naoth::DebugMessageIn::Message& command) const;

  static void* connection_thread_static(void* ref);

  void disconnect();
  void clearBothQueues();
};

#endif  /* DEBUGSERVER_H */

