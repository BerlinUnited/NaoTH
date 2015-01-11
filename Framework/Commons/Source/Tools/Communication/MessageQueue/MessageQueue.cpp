/**
 * @file MessageQueue.cpp
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 *
 * A message queue for communicating
 */

#include "MessageQueue.h"
#include "Tools/Debug/NaoTHAssert.h"

MessageQueue::MessageQueue():
theReader(NULL),
theWriter(NULL)
{
}

MessageQueue::~MessageQueue()
{
  ASSERT(theReader==NULL);
  ASSERT(theWriter==NULL);
}

void MessageQueue::write(const std::string& msg)
{
  theMsg.push(msg);
}
  
bool MessageQueue::empty()
{
  return theMsg.empty();
}
  
std::string MessageQueue::read()
{
  std::string msg = theMsg.front();
  theMsg.pop();
  return msg;
}

void MessageQueue::clear()
{
  theMsg = std::queue<std::string>();
}

void MessageQueue::setReader(MessageReader* reader)
{
  ASSERT(theReader==NULL || reader == NULL);
  theReader = reader;
}

void MessageQueue::setWriter(MessageWriter* writer)
{
  ASSERT(theWriter==NULL || writer == NULL);
  theWriter = writer;
}

MessageReader::MessageReader(MessageQueue* msgQueue)
:theMsgQueue(msgQueue)
{
  theMsgQueue->setReader(this);
}

MessageReader::~MessageReader()
{
  if(theMsgQueue != NULL)
    theMsgQueue->setReader(NULL);
}

MessageWriter::MessageWriter(MessageQueue* msgQueue)
:theMsgQueue(msgQueue)
{
  theMsgQueue->setWriter(this);
}

MessageWriter::~MessageWriter()
{
  if(theMsgQueue != NULL)
    theMsgQueue->setWriter(NULL);
}
