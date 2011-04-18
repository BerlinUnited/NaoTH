
#include "Module.h" 
#include "Representation.h"

void Module::registerUsing(const RepresentationMap& list)
{
  RepresentationMap::const_iterator iter = list.begin();
  for(;iter != list.end(); iter++)
  {
    // init the actual dependency to te black board
    Representation& representation = (*iter).second->registerAtBlackBoard(getBlackBoard());
    used.push_back(&representation);
    representation.registerUsingModule(*this);
  }
}//end registerUsing

void Module::registerRequiring(const RepresentationMap& list)
{
  RepresentationMap::const_iterator iter = list.begin();
  for(;iter != list.end(); iter++)
  {
    // init the actual dependency to te black board
    Representation& representation = (*iter).second->registerAtBlackBoard(getBlackBoard());
    required.push_back(&representation);
    representation.registerRequiringModule(*this);
  }
}//end registerRequired

void Module::registerProviding(const RepresentationMap& list)
{
  RepresentationMap::const_iterator iter = list.begin();
  for(;iter != list.end(); iter++)
  {
    // init the actual dependency to te black board
    Representation& representation = (*iter).second->registerAtBlackBoard(getBlackBoard());
    provided.push_back(&representation);
    representation.registerProvidingModule(*this);
  }
}//end registerProviding


void Module::unregisterUsing(const RepresentationMap& list)
{
  RepresentationMap::const_iterator iter = list.begin();
  for(;iter != list.end(); iter++)
  {
    // init the actual dependency to te black board
    Representation& representation = (*iter).second->registerAtBlackBoard(getBlackBoard());
    
    used.remove(&representation);
    representation.unregisterUsingModule(*this);
  }
}//end unregisterUsing

void Module::unregisterRequiring(const RepresentationMap& list)
{
  RepresentationMap::const_iterator iter = list.begin();
  for(;iter != list.end(); iter++)
  {
    // init the actual dependency to te black board
    Representation& representation = (*iter).second->registerAtBlackBoard(getBlackBoard());
    
    required.remove(&representation);
    representation.unregisterRequiringModule(*this);
  }
}//end unregisterRequiring

void Module::unregisterProviding(const RepresentationMap& list)
{
  RepresentationMap::const_iterator iter = list.begin();
  for(;iter != list.end(); iter++)
  {
    // init the actual dependency to te black board
    Representation& representation = (*iter).second->registerAtBlackBoard(getBlackBoard());
    provided.remove(&representation);
    representation.unregisterProvidingModule(*this);
  }
}//end unregisterProviding
