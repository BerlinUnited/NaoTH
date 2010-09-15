/* 
 * File:   DebugServer.cpp
 * Author: thomas
 * 
 * Created on 10. September 2010, 15:38
 */

#include <string>
#include <stdlib.h>

#include <iostream>
#include <string.h>

#include "DebugServer.h"

DebugServer::DebugServer()
: comm(5401)
{

  commands = g_async_queue_new();
  answers = g_async_queue_new();

  if(g_thread_supported())
  {
    GError* err = NULL;
    g_message("Starting debug server as seperate thread");
    dispatcherThread = g_thread_create(dispatcher_static, this, true, &err);

  }
  else
  {
    g_warning("No threading support: DebugServer not available");
  }

}

void DebugServer::dispatcher()
{
  g_message("Dispatcher init");
  g_async_queue_ref(commands);
  g_async_queue_ref(answers);
  comm.init();

  g_message("Starting DebugServer dispatcher loop");
  while(true)
  {
    char* msg = comm.readMessage();
    if(msg != NULL)
    {
      g_async_queue_push(commands, msg);
    }

    while(g_async_queue_length(answers) > 0)
    {
      g_debug("there is something in the *answer* queue");
      char* answer = (char*) g_async_queue_pop(answers);

      g_debug("found %s in the *answer* queue", answer);

      comm.sendMessage(answer, strlen(answer)+1);

      delete answer;
    }

    g_thread_yield();
  }
  
}

void DebugServer::execute()
{
  g_debug("DebugServer::execute");
  if(g_async_queue_length(commands) > 0)
  {
    g_debug("there is something in the *commands* queue");
    char* cmdRaw = (char*) g_async_queue_pop(commands);

    g_debug("found %s in the *commands* queue", cmdRaw);

    GString* answer = g_string_new("");
    g_string_append(answer, "not implemented: ");
    g_string_append(answer, cmdRaw);
    g_string_append(answer, "\n\0");

    g_async_queue_push(answers, g_string_free(answer,false));
    g_debug("pushed new answer");

    delete cmdRaw;
  }
}

void* DebugServer::dispatcher_static(void* ref)
{
  ((DebugServer*) ref)->dispatcher();
  return NULL;
}


DebugServer::~DebugServer()
{
  g_free(dispatcherThread);
  g_async_queue_unref(commands);
  g_async_queue_unref(answers);
}

