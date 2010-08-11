/* 
 * File:   WebotsCommunicator.cpp
 * Author: thomas
 * 
 * Created on 5. März 2009, 09:19
 */

#include "naorunner/Webots/WebotsCommunicator.h"
#include <iostream>
#include <sstream>
#include <list>

WebotsCommunicator::WebotsCommunicator()
{
}

void WebotsCommunicator::init()
{
  emitterTag = wb_robot_get_device("emitter");
  wb_emitter_set_channel(emitterTag, WB_CHANNEL_BROADCAST);
}

bool WebotsCommunicator::hasMessageInQueue()
{
  return messageList.size() > 0;
}

std::string WebotsCommunicator::peekMessage()
{
  if(hasMessageInQueue())
  {
    std::string tmp = messageList.front();
    messageList.pop_front();
    return tmp;
  }
  return std::string("");
}//end peekMessage

void WebotsCommunicator::sendMessage(const std::string& data)
{
  wb_emitter_send(emitterTag, data.c_str(), data.size());
}

void WebotsCommunicator::pushMessage(const std::string& message)
{
  messageList.push_back(message);
}

WebotsCommunicator::~WebotsCommunicator()
{

}


