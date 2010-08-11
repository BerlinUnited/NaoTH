/* 
 * File:   WebotsCommunicator.h
 * Author: thomas
 *
 * Created on 5. März 2009, 09:19
 */

#ifndef _WebotsCommunicator_H
#define	_WebotsCommunicator_H

#include <webots/receiver.h>
#include <webots/emitter.h>
#include <webots/robot.h>
#include "Tools/Communication/BidirectionalCommunicator.h"
#include "Tools/Communication/RoboCupGameControlData.h"
#include "Tools/Communication/WoboCupGameControlData.h"
#include <list>

class WebotsCommunicator : public BidirectionalCommunicator
{
public:

  WebotsCommunicator();
  virtual ~WebotsCommunicator();

  virtual void init();
  virtual bool hasMessageInQueue();
  virtual std::string peekMessage();
  virtual void sendMessage(const std::string& data);

  void pushMessage(const std::string& message);
private:
  WbDeviceTag emitterTag;

  std::list<std::string> messageList;
};

#endif	/* _WebotsCommunicator_H */

