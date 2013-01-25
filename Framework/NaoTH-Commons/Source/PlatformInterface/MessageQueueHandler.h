/**
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Mellmann, Heinrich</a>
 */

#ifndef _MessageQueueHandler_H
#define _MessageQueueHandler_H

#include "Tools/Communication/MessageQueue/MessageQueue.h"

#include <map>
#include <string>

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
    std::cout << "destruct MessageQueueHandler" << std::endl;

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