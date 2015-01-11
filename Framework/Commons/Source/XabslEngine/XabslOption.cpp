/**
* @file XabslOption.cpp
*
* Implementation of class Option and helper classes
*
* @author <a href="http://www.martin-loetzsch.de">Martin Loetzsch</a>
* @author <a href="http://www.sim.informatik.tu-darmstadt.de/pers/card/risler.html">Max Risler</a>
*/

#include "XabslOption.h"
#include "XabslCoopState.h"

namespace xabsl
{
  
OptionParameters::OptionParameters(InputSource& input,
                                             ErrorHandler& errorHandler,
                                             Symbols& symbols) : Parameters(errorHandler)
{
  int numberOfOptionParameters = (int)input.readValue();
  char buf[100];
  char c[2];
  for (int i=0; i< numberOfOptionParameters; i++)
  {
    input.readString(c,1);
    switch (c[0])
    {
      case 'd':
        input.readString(buf,99);
        XABSL_DEBUG_INIT(errorHandler.message("registering decimal option parameter \"%s\"",buf));
        decimal.append(buf, new double(0));
        break;
      case 'b':
        input.readString(buf,99);
        XABSL_DEBUG_INIT(errorHandler.message("registering boolean option parameter \"%s\"",buf));
        boolean.append(buf, new bool(false));
        break;
      case 'e':
        input.readString(buf,99);
        if (!symbols.enumerations.exists(buf))
        {
          errorHandler.error("XabslOptionParameters::OptionParameters(): enumeration \"%s\" was not registered",buf);
          return;
        }
        const Enumeration* enumeration = symbols.enumerations[buf];
        if (enumeration->enumElements.getSize() == 0)
        {
          errorHandler.error("XabslOptionParameters::OptionParameters(): no enumeration elements for \"%s\" were registered",buf);
          return;
        }
        input.readString(buf,99);
        XABSL_DEBUG_INIT(errorHandler.message("registering enumerated option parameter \"%s\"",buf));
        enumerated.append(buf, new int(enumeration->enumElements[0]->v));
        enumerations.append(buf,enumeration);
        break;
    }
  }
}

OptionParameters::~OptionParameters()
{
  int i;
  for (i = 0; i < decimal.getSize(); i++)
    delete decimal[i];
  for (i = 0; i < boolean.getSize(); i++)
    delete boolean[i];
  for (i = 0; i < enumerated.getSize(); i++)
    delete enumerated[i];
}

Option::Option(const char* name,
                           InputSource& input,
                           ErrorHandler& errorHandler,
                           Symbols& symbols,
                           unsigned (*pTimeFunction)(),
                           int index)
                           : Behavior(name), activeState(0),
                           index(index),
                           initialState(0), errorHandler(errorHandler),
                           pTimeFunction(pTimeFunction)
                          
{
  parameters = new OptionParameters(input, errorHandler, symbols);
}

Option::~Option()
{
  for (int i=0;i<states.getSize();i++)
    if (states[i]!=0)
      delete states[i];
  delete parameters;
}

void Option::create(InputSource& input,   
                          NamedArray<Option*>& options,
                          NamedArray<BasicBehavior*>& basicBehaviors,
                          Symbols& symbols,
                          const int& agentPriority,
                          const int& synchronizationTicks
                          )
{
  int i;
 
  // the number of states of the option
  int numberOfStates = (int)input.readValue();
 
  char stateName[100];
  char c[2];
 
  // register all states to the array
  for (i=0; i< numberOfStates; i++)
  {
    input.readString(stateName, 99);
    // cooperative state or not
    input.readString(c,1);
    switch (*c)
    {
      case 'n':
        states.append(stateName, new State(stateName,errorHandler,pTimeFunction,index,i));
        break;
      case 's':
        states.append(stateName, new SynchronizedState(stateName,errorHandler,pTimeFunction,index,i,agentPriority,synchronizationTicks,(int)input.readValue()));
        break;
      case 'c':
        states.append(stateName, new CapacityState(stateName,errorHandler,pTimeFunction,index,i,agentPriority,synchronizationTicks,(int)input.readValue()));
        break;
    }
  }
  XABSL_DEBUG_INIT(errorHandler.message("XabslOption::create(): registered %i states",numberOfStates));
 
  // set the initial state
  input.readString(stateName,99);
  initialState = states[stateName];
  activeState = initialState;
 
  XABSL_DEBUG_INIT(errorHandler.message("XabslOption::create(): initial state: \"%s\"",initialState->n));
 
  // create the states and their subelements
  for (i=0; i< numberOfStates; i++)
  {
    states[i]->create(input, options, basicBehaviors, symbols);
    if (errorHandler.errorsOccurred)
    {
      errorHandler.error("XabslOption::create(): could not create state \"%s\"",states[i]->n);
      return;
    }
  }
 
}

void Option::execute()
{
  if (isActive)
  {
    errorHandler.error("XabslOption::execute(): option \"%s\" is executed multiple times. This is unsupported. Resulting behavior might be unexpected.",n);
  }
  if (!wasActive)
  {
    activeState = initialState;
    activeState->reset();
  }

  State* newState = activeState->getNextState();
  if (newState != activeState)
  {
    activeState = newState;
    activeState->reset();
  }

  for (int j=0; j < activeState->actions.getSize(); j++)
  {
    activeState->actions[j]->execute();
  }
}

bool Option::getOptionReachedATargetState() const
{
  if (wasActive && activeState->isTargetState())
  {
    return true;
  }
  else
  {
    return false;
  }
}

} // namespace

