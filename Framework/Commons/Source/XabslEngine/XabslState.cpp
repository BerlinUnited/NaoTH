/** 
* @file XabslState.cpp
*
* Implementation of class State and helper classes
*
* @author <a href="http://www.sim.informatik.tu-darmstadt.de/pers/card/risler.html">Max Risler</a>
*/

#include "XabslState.h"
#include "XabslOption.h"

namespace xabsl 
{

State* TransitionToState::getNextState()
{
  return nextState;
}

State::State
(
  const char* name, ErrorHandler& errorHandler,
  unsigned (*pTimeFunction)(),
  int optionIndex,
  int index
)
:
  NamedItem(name),
  subsequentOption(0),
  timeOfStateExecution(0),
  index(index),
  timeWhenStateWasActivated(0),
  optionIndex(optionIndex),
  targetState(false),
  errorHandler(errorHandler),
  decisionTree(0),
  pTimeFunction(pTimeFunction)
{
}

State::~State()
{
  delete decisionTree;
  for (int i=0; i<actions.getSize(); i++)
  {
    delete actions[i];
  }
}

void State::create(InputSource& input,
                         NamedArray<Option*>& options,
                         NamedArray<BasicBehavior*>& basicBehaviors,
                         Symbols& symbols)
{ 
  XABSL_DEBUG_INIT(errorHandler.message("creating state \"%s\"",n));

  char c[100]; 
  int i;
  
  // target state or not
  input.readString(c,1);
  if (*c == '1') 
  {
    targetState = true;
  }
  
  // subsequent option, basic behaviors and output symbol assignments
  int numberOfActions = (int)input.readValue();
  
  subsequentOption = 0;
  for(i = 0; i< numberOfActions; i++)
  {
    Action* action = 
      Action::create(input, 
                      options, 
                      basicBehaviors, 
                      symbols, 
                      *options[optionIndex], 
                      *this, 
                      errorHandler,
                      pTimeFunction);
    if (errorHandler.errorsOccurred)
    {
      errorHandler.error("XabslState::create(): could not create action for state \"%s\"",n);
      return;
    }
    if (subsequentOption == 0)
      if (ActionOption* actionOption = dynamic_cast<ActionOption*>(action)) 
        subsequentOption = actionOption->option;
    actions.append(action);
  }
  
  // transition to state or if / else block
  decisionTree = Statement::createStatement(input,errorHandler,symbols,*options[optionIndex],*this);
  if (errorHandler.errorsOccurred)
    errorHandler.error("XabslState::create(): could not create decision tree for state \"%s\"",n);
}

State* State::getNextState()
{
  timeOfStateExecution = pTimeFunction() - timeWhenStateWasActivated;
  
  State* nextState = decisionTree->getNextState();
  
  return nextState->coopCheck() ? nextState : this;
}

void State::reset()
{
  timeWhenStateWasActivated = pTimeFunction();
  timeOfStateExecution = 0;
}

bool State::isTargetState() const
{
  return targetState;
}

} // namespace

