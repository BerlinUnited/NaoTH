/* 
 * File:   DebugServer.h
 * Author: thomas
 *
 * Created on 10. September 2010, 15:38
 */

#ifndef DEBUGSERVER_H
#define DEBUGSERVER_H

#include <glib.h>
//#include <libb64/decode.h>
//#include <libb64/encode.h>
#include <Tools/DataStructures/DestructureSentinel.h>
#include <Representations/Infrastructure/DebugMessage.h>

#include "DebugCommunicator.h"

#include <map>
#include <queue>
#include <set>

#include <thread>
#include <mutex>
#include <atomic>

class DebugServer
{
public:
  DebugServer();
  virtual ~DebugServer();

  // TODO: why is this virtual?
  virtual void start(unsigned short port);
  
  /** 
  * Set the time after which the connection should be closed in case of inactivity.
  * 0 - means the connection never times out.
  */
  void setTimeOut(unsigned int t) {
    comm.setTimeOut(t);
  }
  
  void getDebugMessageInCognition(naoth::DebugMessageIn& buffer);
  void getDebugMessageInMotion(naoth::DebugMessageIn& buffer);

  void setDebugMessageOut(const naoth::DebugMessageOut& buffer);

private:

  class Channel
  {
  private:
    GAsyncQueue* message_queue;

  public:
    Channel()
    {
      message_queue = g_async_queue_new();
      g_async_queue_ref(message_queue);
    }

    ~Channel()
    {
      g_async_queue_unref(message_queue);
    }

    void push(naoth::DebugMessageIn::Message* message) {
      g_async_queue_push(message_queue, message);
    }

    void copy(naoth::DebugMessageIn& buffer)
    {
      // NOTE: during the loop the received_messages_cognition may change
      //       so there is a chance we would not stop
      int size = g_async_queue_length(message_queue);
      for(int i = 0; i < size && g_async_queue_length(message_queue) > 0; i++)
      {
        naoth::DebugMessageIn::Message* message = (naoth::DebugMessageIn::Message*) g_async_queue_pop(message_queue);

        if(message != NULL)
        {
          buffer.messages.push_back(*message);
          delete message;
        }
      }//end for
    }//end getDebugMessageIn

    void clear(std::set<int>& id_backlog)
    {
      while (g_async_queue_length(message_queue) > 0) {
        naoth::DebugMessageIn::Message* msg = (naoth::DebugMessageIn::Message*) g_async_queue_pop(message_queue);
        id_backlog.erase(msg->id);
        delete msg;
      }
    }
  };

 
  /** Communication interface */
  DebugCommunicator comm;

  std::thread connectionThread;

  GAsyncQueue* answers; // outgoing messages
  Channel received_messages_cognition;
  Channel received_messages_motion;
  std::set<int> id_backlog; // list of unanswered id's

  std::mutex m_executing;

  std::atomic<bool> abort;

  void run();
  void receive();
  void send();

  void parseCommand(GString* cmdRaw, naoth::DebugMessageIn::Message& command) const;

  void disconnect();
  void clearQueues();
};

#endif  /* DEBUGSERVER_H */

