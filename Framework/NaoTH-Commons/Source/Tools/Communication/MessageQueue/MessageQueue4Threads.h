/**
 * @file MessageQueue4Threads.h
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 * 
 * A message queue for communicating between threads
 */
 
#ifndef _MESSAGE_QUEUE_4_THREADS_H_
#define _MESSAGE_QUEUE_4_THREADS_H_

#include "MessageQueue.h"

class MessageQueue4Threads: public MessageQueue
{
public:
  MessageQueue4Threads();
  virtual ~MessageQueue4Threads();
  
  /* write message to the channel */
  virtual void write(const std::string& msg);
  
  /* is there any message in the channel */
  virtual bool empty();
  
  /* read one message from the channel */
  virtual std::string read();
  
  /* clear messages in the channel */
  virtual void clear();
  
private:
  GMutex* theMutex;
};

#endif // _MESSAGE_QUEUE_4_THREADS_H_

