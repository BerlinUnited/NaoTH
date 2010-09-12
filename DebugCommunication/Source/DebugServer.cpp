/* 
 * File:   DebugServer.cpp
 * Author: thomas
 * 
 * Created on 10. September 2010, 15:38
 */

#include <string>
#include <stdlib.h>

#include "DebugServer.h"

DebugServer::DebugServer()
: comm(5401)
{

  if(g_thread_supported())
  {
    GError* err = NULL;
    g_message("Starting debug server as seperate thread");
    thread = g_thread_create(thread_init_static, this, true, &err);

    // just for testing..
    g_thread_join(thread);

  }
  else
  {
    g_warning("No threading support: DebugServer not available");
  }



}

void DebugServer::threadInit()
{
  g_message("Welcome to our funny new thread");
  comm.init();
  comm.hasMessageInQueue();
}

void* DebugServer::thread_init_static(void* ref)
{
  ((DebugServer*) ref)->threadInit();
  return NULL;
}


DebugServer::~DebugServer()
{

}

