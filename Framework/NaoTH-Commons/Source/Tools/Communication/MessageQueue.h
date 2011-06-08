/**
 * @file MessageQueue.h
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 * 
 * A message queue for communicating
 */

#ifndef _MESSAGE_QUEUE_H_
#define _MESSAGE_QUEUE_H_

#include <string>
#include <queue>
#include <glib.h>

class MessageQueue
{
public:
  MessageQueue();
  
  ~MessageQueue();
  
  /* write message to the channel */
  void write(const std::string& msg);
  
  /* is there any message in the channel */
  bool empty() const;
  
  /* read one message from the channel */
  std::string read();
  
  /* clear messages in the channel */
  void clear();
  
private:
  std::queue<std::string> theMsg;
  GMutex* theMutex;
};

#endif