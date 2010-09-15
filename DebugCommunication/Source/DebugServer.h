/* 
 * File:   DebugServer.h
 * Author: thomas
 *
 * Created on 10. September 2010, 15:38
 */

#ifndef DEBUGSERVER_H
#define	DEBUGSERVER_H

#include <glib.h>

#include "DebugCommandExecutor.h"
#include "DebugCommunicator.h"

class DebugServer
{
public:
  DebugServer(unsigned int port);
  virtual ~DebugServer();

  virtual void execute();

  static void* dispatcher_static(void* ref);
private:

  DebugCommunicator comm;
  GThread* dispatcherThread;

  GAsyncQueue* commands;
  GAsyncQueue* answers;

  void dispatcher();
};

#endif	/* DEBUGSERVER_H */

