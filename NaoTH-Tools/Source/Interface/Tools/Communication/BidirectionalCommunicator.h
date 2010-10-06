/* 
 * File:   BidirectionalCommunicator.h
 * Author: thomas
 *
 * Created on 4. März 2009, 22:49
 */

#ifndef _BIDIRECTIONALCOMMUNICATOR_H
#define	_BIDIRECTIONALCOMMUNICATOR_H

#include <string>

class BidirectionalCommunicator
{
public:
  BidirectionalCommunicator() {};
  virtual ~BidirectionalCommunicator() {}

  virtual void init() = 0;

  virtual void sendMessage(const std::string& data) = 0;
  
  virtual bool hasMessageInQueue() = 0;

  virtual std::string peekMessage() = 0;


private:

};

#endif	/* _BIDIRECTIONALCOMMUNICATOR_H */

