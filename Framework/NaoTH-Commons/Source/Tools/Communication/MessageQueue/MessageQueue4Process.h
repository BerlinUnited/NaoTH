/**
 * @file MessageQueue4Process.h
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 * 
 * A message queue for communicating between processes
 */
 
#ifndef _MESSAGE_QUEUE_4_PROCESS_H_
#define _MESSAGE_QUEUE_4_PROCESS_H_

#include "MessageQueue.h"
#include <glib.h>
#include <gio/gio.h>
#include "Tools/Communication/SocketStream/SocketStream.h"

class MessageQueue4Process: public MessageQueue
{
public:
  MessageQueue4Process(const std::string& name);
  virtual ~MessageQueue4Process();
  
  /* write message to the channel */
  virtual void write(const std::string& msg);
  
  /* is there any message in the channel */
  virtual bool empty();
  
  /* clear messages in the channel */
  virtual void clear();

  virtual void setReader(MessageReader* reader);

  virtual void setWriter(MessageWriter* writer);

protected:
  void connect();

  bool isFixedLengthDataAvailable(unsigned int len);
  
private:
  std::string theName;
  GSocketAddress* addr;

  GSocket* serverSocket;
  PrefixedSocketStream theReadStream;
  PrefixedSocketStream theWriteStream;
  GSocket* readSocket;
  GSocket* writeSocket;
};

#endif // _MESSAGE_QUEUE_4_PROCESS_H_
