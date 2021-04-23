/* 
 * File:   DebugServer.cpp
 * Author: thomas
 * 
 * Created on 10. September 2010, 15:38
 */

#include <iostream>
#include <sstream>

#include <Messages/Messages.pb.h>

#include "DebugServer.h"

#include <Tools/NaoTime.h>
#include <Tools/ThreadUtil.h>

using namespace naoth;

DebugServer::DebugServer()
  : 
  abort(false)
{
  answers = g_async_queue_new();
  g_async_queue_ref(answers);
}

DebugServer::~DebugServer()
{
  // notify the connectionThread to stop
  abort = true;

  // wait for connectionThread to stop
  if(connectionThread.joinable()) {
    connectionThread.join();
  }

  clearQueues();
  comm.disconnect();

  g_async_queue_unref(answers);
}


void DebugServer::start(unsigned short port)
{
  comm.init(port);

  std::cout << "[INFO] Starting debug server thread" << std::endl;
   
  connectionThread = std::thread(&DebugServer::run, this);
  ThreadUtil::setName(connectionThread, "DebugServer");
}

void DebugServer::run()
{
  while(!abort)
  {
    if(comm.isConnected()) 
    {
      try {

        // 1. send out already answered messages
        send();

        // 2. get new commands (maximal 50)
        receive();

      } catch(const char* msg) {
        std::cout << "[WARN] debug server exception: " << msg << std::endl;
        disconnect();
      } catch(...) {
        std::cout << "[WARN] unexpected exception in debug server" << std::endl;
        disconnect();
      }
    }

    // connect again, wait max 1 second until connection is etablished
    // watch connections
    if(!comm.isConnected()) 
    {
      // clear the backlog of old messages before accepting new connections
      if(id_backlog.empty()) {
        comm.connect(1);
      } else {
        clearQueues();
      }
    }

    // always give other thread the possibility to gain control before entering
    // the loop again (wait for 1 ms)
    ThreadUtil::sleep(1); 

    // TODO: do we really need this here?
    std::this_thread::yield();
  } // end while true
}//end run


void DebugServer::receive()
{
  GString* msg = NULL;
  gint32 id;
  unsigned int counter = 0;
  do 
  {
    msg = comm.readMessage(id);
    if(msg != NULL)
    {
      // parse and sort the messages

      DebugMessageIn::Message* message = new DebugMessageIn::Message();
      parseCommand(msg, *message);
      message->id = id;

      id_backlog.insert(id);

      std::size_t p = message->command.find(':');
      std::string base(message->command.substr(0,p));
      std::string subcmd(message->command.substr(p+1));

      if(base == "Cognition") 
      {
        message->command = subcmd;
        received_messages_cognition.push(message);
      } 
      else if (base == "Motion") 
      {
        message->command = subcmd;
        received_messages_motion.push(message);
      } 
      else 
      {
        received_messages_cognition.push(message);
      }
      g_string_free(msg, true);
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
    id_backlog.erase(answer->id);

    if(answer != NULL)
    {
      if(!comm.sendMessage(answer->id, answer->data.data(), answer->data.size()))
      {
        std::cout << "[WARN] could not send message" << std::endl;
        disconnect();
      }
      delete answer;
    }
  }//end for
}//end sendAll

void DebugServer::disconnect()
{
  // stop executing (so it's not messing up with our queues)
  {
    std::lock_guard<std::mutex> lock(m_executing);
    clearQueues();
  }
  // all commands are "answered", disconnect
  comm.disconnect();
}


void DebugServer::getDebugMessageInCognition(DebugMessageIn& buffer)
{
  buffer.messages.clear();
  received_messages_cognition.copy(buffer);
}


void DebugServer::getDebugMessageInMotion(DebugMessageIn& buffer)
{
  buffer.messages.clear();
  received_messages_motion.copy(buffer);
}


void DebugServer::setDebugMessageOut(const DebugMessageOut& buffer)
{
    std::lock_guard<std::mutex> lock(m_executing);

    for(std::list<DebugMessageOut::Message*>::const_iterator iter = buffer.answers.begin(); iter != buffer.answers.end(); ++iter)
    {
      g_async_queue_push(answers, *iter);
    }
}

// TODO: serializer?
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


void DebugServer::clearQueues()
{
  while (g_async_queue_length(answers) > 0) {
    DebugMessageOut::Message* msg = (DebugMessageOut::Message*) g_async_queue_pop(answers);
    id_backlog.erase(msg->id);
    delete msg;
  }

  received_messages_cognition.clear(id_backlog);
  received_messages_motion.clear(id_backlog);
}

