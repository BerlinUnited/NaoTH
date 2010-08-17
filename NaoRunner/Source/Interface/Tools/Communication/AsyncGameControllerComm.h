/* 
 * File:   AsyncGameControllerComm.h
 * Author: thomas
 *
 * Created on 8. März 2009, 14:02
 */

#ifndef _ASYNCGAMECONTROLLERCOMM_H
#define	_ASYNCGAMECONTROLLERCOMM_H

#include "BidirectionalCommunicator.h"
#include "RoboCupGameControlData.h"
#include "Tools/Communication/PracticalSocket.h"
#include "PracticalSocket.h"

#include <stdlib.h>

class AsyncGameControllerComm : public BidirectionalCommunicator
{
public:
  AsyncGameControllerComm();
  virtual ~AsyncGameControllerComm();

  virtual void init();
  virtual bool hasMessageInQueue();
  virtual void sendMessage(const std::string& data);
  virtual std::string peekMessage();

private:

  void triggerReceive();

  UDPSocket udpSocket;
  std::string lastData;
  bool lastDataValid;

};

#endif	/* _ASYNCGAMECONTROLLERCOMM_H */

