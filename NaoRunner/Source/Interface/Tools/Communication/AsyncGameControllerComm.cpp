/* 
 * File:   AsyncGameControllerComm.cpp
 * Author: thomas
 * 
 * Created on 8. März 2009, 14:02
 */

#include "AsyncGameControllerComm.h"

AsyncGameControllerComm::AsyncGameControllerComm()
  : udpSocket(GAMECONTROLLER_PORT),
  lastDataValid(false)
{
}

void AsyncGameControllerComm::init()
{
  lastData.resize(sizeof(RoboCupGameControlData), '\0');
}

void AsyncGameControllerComm::triggerReceive()
{
  try
  {
    udpSocket.configureBlocking(false);
    char* tmp = (char*) malloc(sizeof(RoboCupGameControlData));
    int size = udpSocket.recv(tmp, sizeof(RoboCupGameControlData));
    if(size == sizeof(RoboCupGameControlData))
    {
      lastData.assign(tmp, sizeof(RoboCupGameControlData));
      lastDataValid = true;
    }
    free(tmp);
  }
  catch(SocketException ex)
  {
    // ignore
  }
}

bool AsyncGameControllerComm::hasMessageInQueue()
{
  if(!lastDataValid)
  {
    triggerReceive();
  }
  return lastDataValid;
}

void AsyncGameControllerComm::sendMessage(const std::string& /*data*/)
{
  // not implemented yet
}

std::string AsyncGameControllerComm::peekMessage()
{
  if(!lastDataValid)
  {
    triggerReceive();
  }
  
  if(lastDataValid)
  {
    lastDataValid = false;
    return lastData;
  }
  else
  {
    return std::string();
  }
}

AsyncGameControllerComm::~AsyncGameControllerComm()
{
}

