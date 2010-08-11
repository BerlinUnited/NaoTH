/* 
 * File:   CommunicationCollectionImpl.cpp
 * Author: thomas
 * 
 * Created on 4. März 2009, 23:42
 */

//#include "Debug/DebugCommunicator.h"
#include "CommunicationCollectionImpl.h"
#include <string.h>

#include "Tools/Communication/RoboCupGameControlData.h"
#include "Tools/Communication/WoboCupGameControlData.h"

WebotsCommunicationCollection::WebotsCommunicationCollection(unsigned short port)
 /* : debugComm(port) */
{
  receiveTag = wb_robot_get_device("receiver");
  wb_receiver_enable(receiveTag, 500);
  wb_receiver_set_channel(receiveTag, WB_CHANNEL_BROADCAST);
}

BidirectionalCommunicator& WebotsCommunicationCollection::getGameControllerComm()
{
  return gameControllerComm;
}

BidirectionalCommunicator& WebotsCommunicationCollection::getTeamComm()
{
  return teamComm;
}

//DebugCommunicator& WebotsCommunicationCollection::getDebugComm()
//{
//  return debugComm;
//}

WebotsCommunicationCollection::~WebotsCommunicationCollection()
{
}

void WebotsCommunicationCollection::receiveMessages()
{
  while(wb_receiver_get_queue_length(receiveTag))
  {

    const char* data = (char*) wb_receiver_get_data(receiveTag);
    int dataSize = wb_receiver_get_data_size(receiveTag);
    std::string tmp(data, dataSize);

    if (dataSize == sizeof(struct WoboCupGameControlData) && 
        memcmp(data, GAMECONTROLLER_STRUCT_HEADER, 4) == 0)
    {
      gameControllerComm.pushMessage(tmp);
    }else
    {
      teamComm.pushMessage(tmp);
    }

    wb_receiver_next_packet(receiveTag);
  }//end while
}//end receiveMessages


