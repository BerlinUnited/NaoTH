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
#include <Tools/DataStructures/SharedQueue.h>
#include <Representations/Infrastructure/DebugMessage.h>

#include "DebugCommunicator.h"

#include <map>
#include <queue>

#include <thread>
#include <mutex>

#include <memory>

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

  void setDebugMessageOut(const naoth::DebugMessageOut &buffer);

private:

  class Channel
  {
  private:
    naoth::SharedQueue<std::shared_ptr<naoth::DebugMessageIn::Message>> message_queue;

  public:
    Channel()
    {
    }

    ~Channel()
    {
    }

    void push(std::shared_ptr<naoth::DebugMessageIn::Message> message) {
      message_queue.push(message);
    }

    void copy(naoth::DebugMessageIn& buffer)
    {
      // NOTE: during the loop the received_messages_cognition may change
      //       so there is a chance we would not stop
      size_t size = message_queue.size();
      for(size_t i = 0; i < size && !message_queue.empty(); i++)
      {
        std::shared_ptr<naoth::DebugMessageIn::Message> message;
        if(message_queue.try_pop(message))
        {
          buffer.messages.push_back(*message);
        }
      }//end for
    }//end getDebugMessageIn

    void clear()
    {
      message_queue.clear();
    }
  };


  long long lastSendTime;
  long long lastReceiveTime;

  /** Communication interface */
  DebugCommunicator comm;

  std::thread connectionThread;

  naoth::SharedQueue<std::shared_ptr<naoth::DebugMessageOut::Message>> answers; // outgoing messages
  Channel received_messages_cognition;
  Channel received_messages_motion;

  std::mutex m_executing;
  std::mutex m_abort;

  bool abort;

  void run();
  void receive();
  void send();

  void parseCommand(GString* cmdRaw, naoth::DebugMessageIn::Message& command) const;

  void disconnect();
  void clearQueues();
};

#endif  /* DEBUGSERVER_H */

