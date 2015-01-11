/**
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Mellmann, Heinrich</a>
 */

#ifndef _MessageQueueHandler_H
#define _MessageQueueHandler_H

#include "Tools/Communication/MessageQueue/MessageQueue.h"

#include <map>
#include <string>

#undef PRINT_DEBUG
#ifdef DEBUG_PLATFORM
#  define PRINT_DEBUG(m) std::err << m << std::endl
#else
#  define PRINT_DEBUG(m) ((void)0)
#endif

namespace naoth
{

/**
  *
  */
class MessageQueueHandler
{
public:
  typedef std::map<std::string, MessageQueue*> MessageQueueMapT;

  virtual ~MessageQueueHandler()
  {
    PRINT_DEBUG("destruct MessageQueueHandler");

    for(MessageQueueMapT::iterator iter = theMessageQueueMap.begin(); iter!=theMessageQueueMap.end(); ++iter)
    {
      delete iter->second;
    }
  }

  /**
  *
  */
  MessageQueue* getMessageQueue(const std::string& name)
  {
    // create a new one if necessary
    if (theMessageQueueMap.find(name) == theMessageQueueMap.end())
    {
      theMessageQueueMap[name] = this->createMessageQueue(name);
    }
  
    return theMessageQueueMap[name];
  }//end getMessageQueue

protected:
  virtual MessageQueue* createMessageQueue(const std::string& name) = 0;


private:
  MessageQueueMapT theMessageQueueMap;

};//end class MessageQueueHandler
  
}// end namespace naoth

#endif // _MessageQueueHandler_H