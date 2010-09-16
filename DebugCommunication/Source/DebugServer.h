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

  /**
   * Register a command and a handler for this command.
   * @param commmand The name of the command
   * @param description A description displayed to the user if he uses the help function.
   *                    You might want to describe possible arguments here, too.
   * @param executor The callback handler.
   * @return
   */
  bool registerCommand(std::string command, std::string description,
    DebugCommandExecutor* executor);
  
  virtual void execute();

  static void* dispatcher_static(void* ref);
private:

  DebugCommunicator comm;
  GThread* dispatcherThread;

  GAsyncQueue* commands;
  GAsyncQueue* answers;

  /** hash map with all registered callback function  */
  std::map<std::string, DebugCommandExecutor*> executorMap;
  std::map<std::string, std::string> descriptionMap;

  void dispatcher();
  void handleCommand(char* cmdRaw, GString* answer);
  void handleCommand(std::string command, std::map<std::string,std::string> arguments,
    GString* answer, bool encodeBase64);
  
};

#endif	/* DEBUGSERVER_H */

