/**
 * @file MessageQueue4Threads.cpp
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 * 
 * A message queue for communicating between threads
 */
 
#include "MessageQueue4Threads.h"

MessageQueue4Threads::MessageQueue4Threads()
{
  theMutex = g_mutex_new();
}

MessageQueue4Threads::~MessageQueue4Threads()
{
  g_mutex_free(theMutex);
}

void MessageQueue4Threads::write(const std::string& msg)
{
  g_mutex_lock(theMutex);
  MessageQueue::write(msg);
  g_mutex_unlock(theMutex);
}
  
bool MessageQueue4Threads::empty()
{
  g_mutex_lock(theMutex);
  bool v = MessageQueue::empty();
  g_mutex_unlock(theMutex);
  return v;
}
  
std::string MessageQueue4Threads::read()
{
  g_mutex_lock(theMutex);
  std::string msg = MessageQueue::read();
  g_mutex_unlock(theMutex);
  return msg;
}

void MessageQueue4Threads::clear()
{
  g_mutex_lock(theMutex);
  MessageQueue::clear();
  g_mutex_unlock(theMutex);
}
