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

    void clear()
    {
      while (g_async_queue_length(message_queue) > 0) {
        delete (naoth::DebugMessageIn::Message*) g_async_queue_pop(message_queue);
      }
    }
  };


  long long lastSendTime;
  long long lastReceiveTime;

  /** Communication interface */
  DebugCommunicator comm;

  GThread* connectionThread;

  GAsyncQueue* answers; // outgoing messages
  Channel received_messages_cognition;
  Channel received_messages_motion;

  GMutex* m_executing;
  GMutex* m_abort;

  bool abort;

  void run();
  void receive();
  void send();

  void parseCommand(GString* cmdRaw, naoth::DebugMessageIn::Message& command) const;

  static void* connection_thread_static(void* ref);

  void disconnect();
  void clearQueues();
};

#endif  /* DEBUGSERVER_H */

