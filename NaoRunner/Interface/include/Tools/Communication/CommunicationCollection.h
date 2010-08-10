/* 
 * File:   CommunicationCollection.h
 * Author: thomas
 *
 * Created on 4. März 2009, 23:39
 */

#ifndef _COMMUNICATIONCOLLECTION_H
#define	_COMMUNICATIONCOLLECTION_H

#include "BidirectionalCommunicator.h"


class CommunicationCollection
{
public:
  CommunicationCollection() {};

  virtual ~CommunicationCollection()
  {
  };

  virtual BidirectionalCommunicator& getGameControllerComm() = 0;
  virtual BidirectionalCommunicator& getTeamComm() = 0;
  //virtual DebugCommunicator& getDebugComm() = 0;
};

#endif	/* _COMMUNICATIONCOLLECTION_H */

