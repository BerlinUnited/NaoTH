/* 
 * File:   DebugServer.h
 * Author: thomas
 *
 * Created on 10. September 2010, 15:38
 */

#ifndef DEBUGSERVER_H
#define	DEBUGSERVER_H

#include <glib.h>

#include "DebugCommunicator.h"

class DebugServer
{
public:
  DebugServer();
  virtual ~DebugServer();

  void handleIncoming();

  static void* thread_init_static(void* ref);
private:

  DebugCommunicator comm;
  GThread* thread;

  void threadInit();
};

#endif	/* DEBUGSERVER_H */

