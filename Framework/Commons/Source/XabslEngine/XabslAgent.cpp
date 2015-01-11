/** 
* @file XabslAgent.cpp
* 
* Implementation of class Agent
* 
* @author <a href="http://www.martin-loetzsch.de">Martin Loetzsch</a>
*/

#include "XabslAgent.h"

namespace xabsl 
{

  Agent::Agent(const char* name, Behavior* rootOption,
                         ErrorHandler& errorHandler, int index)
: NamedItem(name), rootOption(rootOption), errorHandler(errorHandler), index(index)
{
  XABSL_DEBUG_INIT(errorHandler.message("created agent \"%s\" with root option \"%s\"", n, rootOption->n));
}

Behavior* Agent::getRootOption() const
{
  return rootOption;
}

} // namespace

