/**
 * @file MessageQueue4Threads.cpp
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 * 
 * A message queue for communicating between threads
 */
 
#include "MessageQueue4Threads.h"

MessageQueue4Threads::MessageQueue4Threads()
{
}

MessageQueue4Threads::~MessageQueue4Threads()
{
}

void MessageQueue4Threads::write(const std::string& msg)
{
  std::lock_guard<std::mutex> lock(theMutex);
  MessageQueue::write(msg);
}
  
bool MessageQueue4Threads::empty()
{
  std::lock_guard<std::mutex> lock(theMutex);
  bool v = MessageQueue::empty();
  return v;
}
  
std::string MessageQueue4Threads::read()
{
  std::lock_guard<std::mutex> lock(theMutex);
  std::string msg = MessageQueue::read();
  return msg;
}

void MessageQueue4Threads::clear()
{
  std::lock_guard<std::mutex> lock(theMutex);
  MessageQueue::clear();
}
