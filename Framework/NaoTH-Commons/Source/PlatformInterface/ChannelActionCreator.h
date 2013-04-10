/**
 * @file ChannelActionCreator.h
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Mellmann, Heinrich</a>
 */

#ifndef _ChannelActionCreator_H
#define _ChannelActionCreator_H

#include "Tools/Communication/MessageQueue/MessageQueue.h"
#include "Tools/DataStructures/Serializer.h"
#include "MessageQueueHandler.h"

#include <sstream>

namespace naoth
{

/**
 * @class OutputChanelAction
 * Writes the object data of type T to the given message queue
 * using the serializer ST
 */
template<class T, class ST = Serializer<T> >
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

  ~OutputChanelAction()
  {
    std::cout << "destruct OutputChanelAction " << typeid(T).name() << std::endl;
  }
      
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
template<class T, class ST = Serializer<T> >
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

  ~InputChanelAction()
  {
    std::cout << "destruct OutputChanelAction " << typeid(T).name() << std::endl;
  }
      
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

  template<class T>
  AbstractAction* createOutputChanelAction(const T& data)
  {
    if(messageQueueHandler == NULL) return NULL;
    
    MessageQueue* messageQueue = messageQueueHandler->getMessageQueue(typeid(T).name());
    return new OutputChanelAction<T>(messageQueue, data);
  }//end createOutputChanelAction


  template<class T>
  AbstractAction* createInputChanelAction(T& data)
  {
    if(messageQueueHandler == NULL) return NULL;

    MessageQueue* messageQueue = messageQueueHandler->getMessageQueue(typeid(T).name());
    return new InputChanelAction<T>(messageQueue, data);
  }//end createInputChanelAction

};//end ChannelActionCreator

}// end namespace naoth

#endif // _ChannelActionCreator_H