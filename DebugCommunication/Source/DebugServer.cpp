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
    g_message("Starting debug server as seperate thread");

    comm.init();
  }
  else
  {
    g_warning("No threading support: DebugServer not available");
  }

}



DebugServer::~DebugServer()
{

}

