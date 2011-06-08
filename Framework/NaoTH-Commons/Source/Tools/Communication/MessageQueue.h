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

class MessageReader;
class MessageWriter;

class MessageQueue
{
public:
  friend class MessageReader;
  friend class MessageWriter;

  MessageQueue();
  
  ~MessageQueue();
  
protected:
  
  /* write message to the channel */
  void write(const std::string& msg);
  
  /* is there any message in the channel */
  bool empty() const;
  
  /* read one message from the channel */
  std::string read();
  
  /* clear messages in the channel */
  void clear();
  
  void setReader(MessageReader* reader);
  
  void setWriter(MessageWriter* writer);
  
  void lock();
  
  void unlock();
  
private:
  std::queue<std::string> theMsg;
  GMutex* theMutex;
  
  MessageReader* theReader;
  MessageWriter* theWriter;
};

class MessageReader
{
public:
  MessageReader(MessageQueue* msgQueue);
  
  ~MessageReader();
  
  std::string read() { return theMsgQueue->read(); }
  
  bool empty() const { return theMsgQueue->empty(); }
  
private:
  MessageQueue* theMsgQueue;
};

class MessageWriter
{
public:
  MessageWriter(MessageQueue* msgQueue);
  
  ~MessageWriter();
  
  void write(const std::string& msg) { theMsgQueue->write(msg); }
  
private:
  MessageQueue* theMsgQueue;
};

#endif