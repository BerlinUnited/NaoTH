/** 
* @file XabslStatement.cpp
*
* Implementation of class Statement and helper classes
*
* @author <a href="http://www.sim.informatik.tu-darmstadt.de/pers/card/risler.html">Max Risler</a>
*/

#include "XabslStatement.h"
#include "XabslState.h"
#include "XabslOption.h"

namespace xabsl 
{

Statement* Statement::createStatement(InputSource& input,    
                                                  ErrorHandler& errorHandler,
                                                  Symbols& symbols,    
                                                  Option& option,
                                                  State& state)
{
  char c[100]; 
  
  input.readString(c,1);
  
  switch (*c)
  {
  case 't':
    return new TransitionToState(input, errorHandler, option);
  case 'i':
    return new IfElseBlock(input, errorHandler, symbols, option, state);
  default:
    errorHandler.error("XabslStatement::create(): unknown type \"%c\"",*c);
    return 0;
  }
}

Statement::~Statement()
{
}

IfElseBlock::IfElseBlock(InputSource& input,    
                                     ErrorHandler& errorHandler,
                                     Symbols& symbols,    
                                     Option& option,
                                     State& state) :
ifCondition(0), ifStatement(0), elseStatement(0)
{
  
  // if case
  XABSL_DEBUG_INIT(errorHandler.message("creating if statement"));
  
  ifCondition = BooleanExpression::create(input, errorHandler, symbols, option, state);
  if (errorHandler.errorsOccurred)
  {
    errorHandler.error("XabslIfElseBlock::IfElseBlock(): could not create if condition");
    return;
  }
  
  ifStatement = Statement::createStatement(input, errorHandler, symbols, option, state);
  if (errorHandler.errorsOccurred)
  {
    errorHandler.error("XabslIfElseBlock::IfElseBlock(): could not create if statement");
    return;
  }
    
  // else case
  XABSL_DEBUG_INIT(errorHandler.message("creating else statement"));
  
  elseStatement = Statement::createStatement(input, errorHandler, symbols, option, state);
  if (errorHandler.errorsOccurred)
    errorHandler.error("XabslIfElseBlock::IfElseBlock(): could not create else statement");
  
}

IfElseBlock::~IfElseBlock()
{
  delete ifCondition;
  delete ifStatement;
  
  int i;
  for (i=0; i<elseIfStatements.getSize(); i++)
  {
    delete elseIfConditions[i];
    delete elseIfStatements[i];
  }
  
  delete elseStatement;
}

State* IfElseBlock::getNextState()
{
  if (ifCondition->getValue()) return ifStatement->getNextState();
  
  for (int i=0; i<elseIfConditions.getSize(); i++)
  {
    if (elseIfConditions[i]->getValue()) return elseIfStatements[i]->getNextState();
  }
  
  return elseStatement->getNextState();
}

TransitionToState::TransitionToState(InputSource& input,    
                                                 ErrorHandler& XABSL_DEBUG_INIT(errorHandler),
                                                 Option& option)
{
  char buf[100];
  input.readString(buf,99);
  
  nextState = option.states[buf];
  
  XABSL_DEBUG_INIT(errorHandler.message("creating a transition to state \"%s\"",nextState->n));
}

} // namespace

