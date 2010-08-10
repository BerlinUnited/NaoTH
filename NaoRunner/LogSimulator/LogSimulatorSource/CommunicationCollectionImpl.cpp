/* 
 * File:   CommunicationCollectionImpl.cpp
 * Author: thomas
 * 
 * Created on 7. März 2009, 22:10
 */

#include "CommunicationCollectionImpl.h"

CommunicationCollectionImpl::CommunicationCollectionImpl()
  : debugComm(5401) // always the same for simulator (assuming only one is active)
{
}

DebugCommunicator& CommunicationCollectionImpl::getDebugComm()
{
  return debugComm;
}

BidirectionalCommunicator& CommunicationCollectionImpl::getTeamComm()
{
  return teamComm;
}

BidirectionalCommunicator& CommunicationCollectionImpl::getGameControllerComm()
{
  return gameControllerComm;
}

CommunicationCollectionImpl::~CommunicationCollectionImpl()
{
}

