/* 
 * File:   CommunicationCollectionImpl.h
 * Author: thomas
 *
 * Created on 7. März 2009, 22:10
 */

#ifndef _COMMUNICATIONCOLLECTIONIMPL_H
#define	_COMMUNICATIONCOLLECTIONIMPL_H

#include "Tools/Communication/CommunicationCollection.h"
#include "Tools/Communication/DummyBidirectionCommunicator.h"
#include "Tools/Communication/AsyncGameControllerComm.h"
#include "Debug/DebugCommunicator.h"

class CommunicationCollectionImpl : public CommunicationCollection
{
public:
  CommunicationCollectionImpl();
  virtual ~CommunicationCollectionImpl();

  virtual DebugCommunicator& getDebugComm();
  virtual BidirectionalCommunicator& getGameControllerComm();
  virtual BidirectionalCommunicator& getTeamComm();

private:

  AsyncGameControllerComm gameControllerComm;
  DummyBidirectionalCommunicator teamComm;
  DebugCommunicator debugComm;

};

#endif	/* _COMMUNICATIONCOLLECTIONIMPL_H */

