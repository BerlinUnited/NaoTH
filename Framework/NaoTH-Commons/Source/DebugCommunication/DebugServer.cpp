/* 
 * File:   DebugServer.cpp
 * Author: thomas
 * 
 * Created on 10. September 2010, 15:38
 */

#include <string.h>

#include <string>
#include <cstdlib>

#include <iostream>
#include <sstream>

#include <Messages/Messages.pb.h>
#include "DebugServer.h"

DebugServer::DebugServer()
  : connectionThread(NULL),
    abort(false)
{
  m_executing = g_mutex_new();
  m_abort = g_mutex_new();

  commands = g_async_queue_new();
  answers = g_async_queue_new();

  g_async_queue_ref(commands);
  g_async_queue_ref(answers);
}//end DebugServer


void DebugServer::start(unsigned short port, bool threaded)
{
  comm.init(port);

  if(threaded)
  {
    if (!g_thread_supported())
      g_thread_init(NULL);

    GError* err = NULL;
    g_debug("Starting debug server thread");
   
    connectionThread = g_thread_create(connection_thread_static, this, true, &err);
    if(err)
    {
      g_warning("Could not start debug server thread: %s", err->message);
    }
  }
}//end start

void DebugServer::mainConnection()
{
  g_async_queue_ref(commands);
  g_async_queue_ref(answers);

  while(true)
  {
    // check if the stop is requested
    g_mutex_lock(m_abort);
    if(abort)
    {
      g_mutex_unlock(m_abort);
      break; // end loop;
    }
    g_mutex_unlock(m_abort);

    
    if(comm.isConnected())
    {
      // 1. send out already answered messages
      try
      {
        sendAll();
      }
      catch(...)
      {
        disconnect();
      }
    }

    if(comm.isConnected())
    {
      // 2. get new commands (maximal 50)
      try
      {
        receiveAll();
      }
      catch(...)
      {
        disconnect();
      }
    }


    if(!comm.isConnected())
    {
      // connect again, wait max 1 second until connection is etablished
      comm.connect(1);
    } // end if connected

    // always give other thread the possibility to gain control before entering
    // the loop again
    g_usleep(1000);
    g_thread_yield();
  } // end while true

  g_async_queue_unref(commands);
  g_async_queue_unref(answers);
}//end mainConnection


void DebugServer::receiveAll()
{
  GString* msg = NULL;
  unsigned int counter = 0;
  do
  {
    msg = comm.readMessage();
    if(msg != NULL)
    {
      g_async_queue_push(commands, msg);
    }
    counter++;
  } while(msg != NULL && counter < 50);
}//end receiveAll

void DebugServer::sendAll()
{
  while(g_async_queue_length(answers) > 0)
  {
    GString* answer = (GString*) g_async_queue_pop(answers);

    if(answer != NULL)
    {
      if(!comm.sendMessage(answer->str, answer->len))
      {
        g_warning("could not send message");
        disconnect();
      }

      g_string_free(answer, true);
    } // end if answer != NULL
  }//end whille
}//end sendAll

void DebugServer::disconnect()
{
  // stop executing (so it's not messing up with our queues)
  g_mutex_lock(m_executing);
  clearBothQueues();
  g_mutex_unlock(m_executing);

  // all commands are "answered", disconnect
  comm.disconnect();
}//end disconnect


void DebugServer::getDebugMessageIn(naoth::DebugMessageIn& buffer)
{
  buffer.messages.clear();

  g_mutex_lock(m_executing);

  // if running in the single threaded mode
  // try to receive messages
  if(connectionThread == NULL)
  {
    // try to connect
    if(!comm.isConnected())
      comm.connect(-1);
  
    if(comm.isConnected())
    {
      try
      {
        receiveAll();
      }
      catch(...)
      {
        disconnect();
      }
    }
  }// end if single threaded

  // copy messages
  while (g_async_queue_length(commands) > 0)
  {
    GString* cmdRaw = (GString*) g_async_queue_pop(commands);

    naoth::DebugMessageIn::Message message;
    parseCommand(cmdRaw, message.command, message.arguments);
    buffer.messages.push_back(message);

    g_string_free(cmdRaw, true);
  }//end while

  g_mutex_unlock(m_executing);
}//end getDebugMessageIn


void DebugServer::setDebugMessageOut(const naoth::DebugMessageOut& buffer)
{
  g_mutex_lock(m_executing);

  for(std::list<std::string>::const_iterator iter=buffer.answers.begin(); iter!=buffer.answers.end(); ++iter)
  {
    GString* answer = g_string_new("");
    g_string_append_len(answer, iter->c_str(), iter->size());
    g_async_queue_push(answers, answer);
  }//end while

  // if running in the single threaded mode
  // try to send messages
  if(connectionThread == NULL)
  {
    // try to connect
    if(!comm.isConnected())
      comm.connect(-1);

    if(comm.isConnected())
    {
      try
      {
        sendAll();
      }
      catch(...)
      {
        disconnect();
      }
    }
  }//end if single threaded

  g_mutex_unlock(m_executing);
}//end getDebugMessageOut


void DebugServer::parseCommand(
  GString* cmdRaw, 
  std::string& commandName, 
  std::map<std::string, std::string>& arguments)
{
  naothmessages::CMD cmd;
  commandName = "invalidcommand";

  if(cmd.ParseFromArray(cmdRaw->str, cmdRaw->len))
  {
    commandName = cmd.name();

    for(int i=0; i < cmd.args().size(); i++)
    {
      const naothmessages::CMDArg& arg = cmd.args().Get(i);
      if(arg.has_bytes())
      {
        arguments[arg.name()] = arg.bytes();
      }
      else
      {
        arguments[arg.name()] = arg.name();
      }
    }
  }//end if
}//end parseCommand


void DebugServer::clearBothQueues()
{
  while (g_async_queue_length(commands) > 0)
  {
    GString* tmp = (GString*) g_async_queue_pop(commands);
    g_string_free(tmp, true);
  }

  while (g_async_queue_length(answers) > 0)
  {
    GString* tmp = (GString*) g_async_queue_pop(answers);
    g_string_free(tmp, true);
  }
}//end clearBothQueues

void* DebugServer::connection_thread_static(void* ref)
{
  static_cast<DebugServer*>(ref)->mainConnection();
  return NULL;
}

DebugServer::~DebugServer()
{
  // notify the connectionThread to stop
  g_mutex_lock(m_abort);
  abort = true;
  g_mutex_unlock(m_abort);

  // wait for connectionThread to stop
  if(connectionThread != NULL)
  {
    g_thread_join(connectionThread);
  }

  clearBothQueues();
  comm.disconnect();

  g_mutex_free(m_executing);
  g_mutex_free(m_abort);

  g_async_queue_unref(commands);
  g_async_queue_unref(answers);
}

