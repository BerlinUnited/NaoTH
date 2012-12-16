/**
 * @file ChannelActionCreator.h
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Mellmann, Heinrich</a>
 */

#ifndef _ChannelActionCreator_H
#define _ChannelActionCreator_H

#include "Tools/Communication/MessageQueue/MessageQueue.h"
#include "MessageQueueHandler.h"

#include <sstream>

namespace naoth
{

/**
 * @class OutputChanelAction
 * Writes the object data of type T to the given message queue
 * using the serializer ST
 */
template<class T, class ST>
class OutputChanelAction: public AbstractAction
{
private:
  MessageWriter writer;
  const T& data;
      
public:
  OutputChanelAction(MessageQueue* messageQueue, const T& data)
    : writer(messageQueue),
      data(data)
  {}
      
  void execute()
  {
    std::stringstream hmmsg;
    ST::serialize(data, hmmsg);
    writer.write(hmmsg.str());
  }//end execute
}; // end class OutputChanelAction


/**
 * @class InputChanelAction
 * Reads the object data of type T from the given message queue
 * using the serializer ST
 */
template<class T, class ST>
class InputChanelAction: public AbstractAction
{
private:
  MessageReader reader;
  T& data;

public:
  InputChanelAction(MessageQueue* messageQueue, T& data) 
    : reader(messageQueue),
      data(data)
  {}
      
  void execute()
  {
    if ( !reader.empty() )
    {
      std::string msg = reader.read();
      // drop old message
      while ( !reader.empty() )
      {
        msg = reader.read();
      }
      std::stringstream ss(msg);
      ST::deserialize(ss, data);
    }//end if
  }//end execute
};//end class InputChanelAction


/**
 * @class ChannelActionCreator
 */
class ChannelActionCreator
{
private:
  MessageQueueHandler* messageQueueHandler;

public:
  ChannelActionCreator()
    : messageQueueHandler(NULL)
  {
  }

  void setMessageQueueHandler(MessageQueueHandler* handler)
  {
    messageQueueHandler = handler;
  }

  template<class T, class ST>
  AbstractAction* createOutputChanelAction(const T& data)
  {
    if(messageQueueHandler == NULL) return NULL;
    
    MessageQueue* messageQueue = messageQueueHandler->getMessageQueue(typeid(T).name());
    return new OutputChanelAction<T, ST>(messageQueue, data);
  }//end createOutputChanelAction


  template<class T, class ST>
  AbstractAction* createInputChanelAction(T& data)
  {
    if(messageQueueHandler == NULL) return NULL;

    MessageQueue* messageQueue = messageQueueHandler->getMessageQueue(typeid(T).name());
    return new InputChanelAction<T,ST>(messageQueue, data);
  }//end createInputChanelAction

};//end ChannelActionCreator

}// end namespace naoth

#endif // _ChannelActionCreator_H