
#include "Module.h" 
#include "Representation.h"

void Module::registerUsing(const RepresentationMap& list)
{
  RepresentationMap::const_iterator iter = list.begin();
  for(;iter != list.end(); iter++)
  {
    // init the actual dependency to te black board
    Representation& representation = (*iter).second->registerAtBlackBoard(getBlackBoard());
    node.registerFrom(representation);
    representation.registerUsingModule(*this);
  }
}//end registerUsing


void Module::registerProviding(const RepresentationMap& list)
{
  RepresentationMap::const_iterator iter = list.begin();
  for(;iter != list.end(); iter++)
  {
    // init the actual dependency to te black board
    Representation& representation = (*iter).second->registerAtBlackBoard(getBlackBoard());
    node.registerTo(representation);
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
    node.unregisterFrom(representation);
    representation.unregisterUsingModule(*this);
  }
}//end unregisterUsing


void Module::unregisterProviding(const RepresentationMap& list)
{
  RepresentationMap::const_iterator iter = list.begin();
  for(;iter != list.end(); iter++)
  {
    // init the actual dependency to te black board
    Representation& representation = (*iter).second->registerAtBlackBoard(getBlackBoard());
    node.unregisterTo(representation);
    representation.unregisterProvidingModule(*this);
  }
}//end unregisterProviding
