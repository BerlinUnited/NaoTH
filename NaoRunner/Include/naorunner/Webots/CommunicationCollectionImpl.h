/* 
 * File:   CommunicationCollectionImpl.h
 * Author: thomas
 *
 * Created on 4. März 2009, 23:42
 */

#ifndef _COMMUNICATIONCOLLECTIONIMPL_H
#define	_COMMUNICATIONCOLLECTIONIMPL_H

#include "naorunner/Tools/DataStructures/Singleton.h"
#include "naorunner/Tools/Communication/CommunicationCollection.h"
#include "naorunner/Tools/Communication/BidirectionalCommunicator.h"
#include "naorunner/Tools/Communication/DummyBidirectionCommunicator.h"

//#include "Debug/DebugCommunicator.h"

#include "WebotsCommunicator.h"
#include <list>

class WebotsCommunicationCollection : public CommunicationCollection
{

public:
  WebotsCommunicationCollection(unsigned short port);
  virtual ~WebotsCommunicationCollection();

  virtual BidirectionalCommunicator& getGameControllerComm();
  virtual BidirectionalCommunicator& getTeamComm();
  //virtual DebugCommunicator& getDebugComm();

  void receiveMessages();

private:
  WebotsCommunicator gameControllerComm;
  WebotsCommunicator teamComm;

  //DebugCommunicator debugComm;

  WbDeviceTag receiveTag;
};

#define GET_COMMUNICATION_COLLECTION_IMPL WebotsCommunicationCollection::getInstance()

#endif	/* _COMMUNICATIONCOLLECTIONIMPL_H */

