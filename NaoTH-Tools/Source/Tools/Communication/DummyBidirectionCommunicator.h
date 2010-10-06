/* 
 * File:   DummyBidirectionalCommunicator.h
 * Author: thomas
 *
 * Created on 5. März 2009, 09:07
 */

#ifndef _DUMMYBIDIRECTIONALCOMMUNICATOR_H
#define	_DUMMYBIDIRECTIONALCOMMUNICATOR_H

#include "BidirectionalCommunicator.h"

class DummyBidirectionalCommunicator : public BidirectionalCommunicator
{
public:
  DummyBidirectionalCommunicator() {};
  virtual ~DummyBidirectionalCommunicator() {};

  virtual void init() {};
  virtual bool hasMessageInQueue() {return false;};
  virtual void sendMessage(const std::string& /*data*/) {};
  virtual std::string peekMessage() {return std::string();};
  
private:

};

#endif	/* _DUMMYBIDIRECTIONALCOMMUNICATOR_H */


