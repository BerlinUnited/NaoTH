/* 
 * File:   DebugServer.h
 * Author: thomas
 *
 * Created on 10. September 2010, 15:38
 */

#ifndef DEBUGSERVER_H
#define  DEBUGSERVER_H

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

  virtual void start(unsigned short port, bool threaded = false);
 
  static void* connection_thread_static(void* ref);

  void getDebugMessageIn(naoth::DebugMessageIn& buffer);
  void setDebugMessageOut(const naoth::DebugMessageOut& buffer);

private:

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
  void parseCommand(GString* cmdRaw, std::string& commandName, std::map<std::string, std::string>& arguments);

  void disconnect();
  void clearBothQueues();
};

#endif  /* DEBUGSERVER_H */

