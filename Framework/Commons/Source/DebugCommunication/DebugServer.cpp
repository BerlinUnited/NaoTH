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
#include <Tools/NaoTime.h>

using namespace naoth;

DebugServer::DebugServer()
  : 
  lastSendTime(0),
  lastReceiveTime(0),
  connectionThread(NULL),
  abort(false)
{
  m_executing = g_mutex_new();
  m_abort = g_mutex_new();

  m_received_messages_cognition = g_mutex_new();
  m_received_messages_motion = g_mutex_new();

  commands = g_async_queue_new();
  answers = g_async_queue_new();

  g_async_queue_ref(commands);
  g_async_queue_ref(answers);
}

DebugServer::~DebugServer()
{
  // notify the connectionThread to stop
  g_mutex_lock(m_abort);
  abort = true;
  g_mutex_unlock(m_abort);

  // wait for connectionThread to stop
  if(connectionThread != NULL) {
    g_thread_join(connectionThread);
  }

  clearBothQueues();
  comm.disconnect();

  g_mutex_free(m_executing);
  g_mutex_free(m_abort);

  g_mutex_free(m_received_messages_cognition);
  g_mutex_free(m_received_messages_motion);

  g_async_queue_unref(commands);
  g_async_queue_unref(answers);
}


void DebugServer::start(unsigned short port)
{
  comm.init(port);

  if (!g_thread_supported()) {
    g_thread_init(NULL);
  }

  GError* err = NULL;
  g_debug("Starting debug server thread");
   
  connectionThread = g_thread_create(connection_thread_static, this, true, &err);
  if(err) {
    g_warning("Could not start debug server thread: %s", err->message);
  }
}//end start

void DebugServer::run()
{
  g_async_queue_ref(commands);
  g_async_queue_ref(answers);

  while(true)
  {
    // check if the stop is requested
    g_mutex_lock(m_abort);
    if(abort) {
      g_mutex_unlock(m_abort);
      break;
    }
    g_mutex_unlock(m_abort);

    
    if(comm.isConnected()) 
    {
      try {

        // 1. send out already answered messages
        send();

        // 2. get new commands (maximal 50)
        receive();

      } catch(const char* msg) {
        g_warning("debug server exception: %s", msg);
        disconnect();
      } catch(...) {
        g_warning("unexpected exception in debug server");
        disconnect();
      }
    }

    // connect again, wait max 1 second until connection is etablished
    if(!comm.isConnected()) {
      comm.connect(1);
    }

    // always give other thread the possibility to gain control before entering
    // the loop again (wait for 1 ms)
    
    g_usleep(1000);

    // TODO: do we really need this here?
    g_thread_yield();
  } // end while true

  g_async_queue_unref(commands);
  g_async_queue_unref(answers);
}//end run


void DebugServer::receive()
{
  GString* msg = NULL;
  gint32 id;
  unsigned int counter = 0;
  do {
    msg = comm.readMessage(id);
    if(msg != NULL)
    {
      // parse and sort the messages

      DebugMessageIn::Message message;
      parseCommand(msg, message);
      message.id = id;

      std::size_t p = message.command.find(':');
      std::string base(message.command.substr(0,p));
      message.command = message.command.substr(p+1);

      if(base == "Cognition") {
        g_mutex_lock(m_received_messages_cognition);
        received_messages_cognition.push(message);
        g_mutex_unlock(m_received_messages_cognition);
      } else if (base == "Motion") {
        g_mutex_lock(m_received_messages_motion);
        received_messages_motion.push(message);
        g_mutex_unlock(m_received_messages_motion);
      } else {
        received_messages_cognition.push(message);
      }
      g_string_free(msg, true);
      //g_async_queue_push(commands, msg);
    }
    counter++;
  } while(msg != NULL && counter < 50); // max of 50 messages
}//end receiveAll


void DebugServer::send()
{
  // NOTE: the size of the queue may change during this loop,
  //       so save it befor the execution
  int size = g_async_queue_length(answers);
  for(int i = 0; i < size && g_async_queue_length(answers) > 0; i++)
  {
    DebugMessageOut::Message* answer = (DebugMessageOut::Message*) g_async_queue_pop(answers);

    if(answer != NULL)
    {
      if(!comm.sendMessage(answer->id, answer->data.data(), answer->data.size()))
      {
        g_warning("could not send message");
        disconnect();
      }
      //g_string_free(answer, true);
      delete answer;
    }
  }//end for
}//end sendAll

void DebugServer::disconnect()
{
  // stop executing (so it's not messing up with our queues)
  g_mutex_lock(m_executing);
  clearBothQueues();
  g_mutex_unlock(m_executing);

  // all commands are "answered", disconnect
  comm.disconnect();
}


void DebugServer::getDebugMessageInCognition(DebugMessageIn& buffer)
{
  buffer.messages.clear();

  g_mutex_lock(m_received_messages_cognition);

  // parse messages
  while (!received_messages_cognition.empty())
  {
    buffer.messages.push_back(received_messages_cognition.front());
    received_messages_cognition.pop();
  }

  g_mutex_unlock(m_received_messages_cognition);
}//end getDebugMessageIn


void DebugServer::getDebugMessageInMotion(DebugMessageIn& buffer)
{
  buffer.messages.clear();

  g_mutex_lock(m_received_messages_motion);

  // parse messages
  while (!received_messages_motion.empty())
  {
    buffer.messages.push_back(received_messages_motion.front());
    received_messages_motion.pop();
  }

  g_mutex_unlock(m_received_messages_motion);
}//end getDebugMessageIn


void DebugServer::setDebugMessageOut(const DebugMessageOut& buffer)
{
  g_mutex_lock(m_executing);

  for(std::list<DebugMessageOut::Message*>::const_iterator iter = buffer.answers.begin(); iter != buffer.answers.end(); ++iter)
  {
    g_async_queue_push(answers, *iter);
  }

  g_mutex_unlock(m_executing);
}//end getDebugMessageOut


void DebugServer::parseCommand(GString* cmdRaw, DebugMessageIn::Message& command) const
{
  naothmessages::CMD cmd;
  command.command = "invalidcommand";

  if(cmd.ParseFromArray(cmdRaw->str, static_cast<int> (cmdRaw->len)))
  {
    command.command = cmd.name();

    for(int i=0; i < cmd.args().size(); i++)
    {
      const naothmessages::CMDArg& arg = cmd.args().Get(i);
      if(arg.has_bytes()) {
        command.arguments[arg.name()] = arg.bytes();
      } else {
        command.arguments[arg.name()] = ""; //arg.name();
      }
    }
  }
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
    delete (DebugMessageOut::Message*) g_async_queue_pop(answers);
    //g_string_free(tmp, true);
  }
}//end clearBothQueues

void* DebugServer::connection_thread_static(void* ref)
{
  static_cast<DebugServer*>(ref)->run();
  return NULL;
}

