/**
 * @file MessageQueue.cpp
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 *
 * A message queue for communicating
 */

#include "MessageQueue.h"

MessageQueue::MessageQueue()
{
  theMutex = g_mutex_new();
}

MessageQueue::~MessageQueue()
{
  g_mutex_free(theMutex);
}

void MessageQueue::write(const std::string& msg)
{
  g_mutex_lock(theMutex);
  theMsg.push(msg);
  g_mutex_unlock(theMutex);
}
  
bool MessageQueue::empty() const
{
  g_mutex_lock(theMutex);
  bool v = theMsg.empty();
  g_mutex_unlock(theMutex);
  return v;
}
  
std::string MessageQueue::read()
{
  g_mutex_lock(theMutex);
  std::string msg = theMsg.front();
  theMsg.pop();
  g_mutex_unlock(theMutex);
  return msg;
}

void MessageQueue::clear()
{
  g_mutex_lock(theMutex);
  theMsg = std::queue<std::string>();
  g_mutex_unlock(theMutex);
}
