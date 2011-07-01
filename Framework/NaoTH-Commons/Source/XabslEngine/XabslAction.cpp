/** 
* @file XabslAction.cpp
*
* Implementation of class Action and helper classes
*
* @author <a href="http://www.sim.informatik.tu-darmstadt.de/pers/card/risler.html">Max Risler</a>
*/


#include "XabslAction.h"
#include "XabslOption.h"

namespace xabsl 
{

Action* Action::create(
    InputSource& input,    
    NamedArray<Option*>& options,
    NamedArray<BasicBehavior*>& basicBehaviors,
    Symbols& symbols,
    Option& option,
    State& state,
    ErrorHandler& errorHandler,
    unsigned (*pTimeFunction)())
{

  char c[2];
  char buf[100];

  input.readString(c,1);
  input.readString(buf,99);

  switch (*c)
  {
    case 'o':
      XABSL_DEBUG_INIT(errorHandler.message("creating reference to option \"%s\"",buf));

      if (!options.exists(buf))
      {
        errorHandler.error("XabslAction::create(): unknown option \"%s\" was encountered",buf);
        return 0;
      }
      else
      {
        ActionOption* action = new ActionOption(pTimeFunction);
        action->option = options[buf];
        action->parameters = new ParameterAssignment(action->option->parameters, errorHandler);
        action->parameters->create(input, 
                          symbols, 
                          option, 
                          state);
        return action;
      }
      break;
    case 'a':
      XABSL_DEBUG_INIT(errorHandler.message("creating reference to basic behavior \"%s\"",buf));

      if (!basicBehaviors.exists(buf))
      {
        errorHandler.error("XabslAction::create(): the subsequent basic behavior \"%s\" was not registered",buf);
        return 0;
      }
      else
      {
        ActionBasicBehavior* action = new ActionBasicBehavior(pTimeFunction);
        action->basicBehavior = basicBehaviors[buf];
        action->parameters = new ParameterAssignment(action->basicBehavior->parameters, errorHandler);
        action->parameters->create(input, 
                          symbols, 
                          option,
                          state);
        return action;
      }
      break;
    case 'd':
      XABSL_DEBUG_INIT(errorHandler.message("creating reference to decimal output symbol \"%s\"",buf));
      
      if (!symbols.decimalOutputSymbols.exists(buf))
      {
        errorHandler.error("XabslAction::create(): decimal output symbol \"%s\" does not exist",buf);
        return 0;
      }
      else
      {
        ActionDecimalOutputSymbol* action = new ActionDecimalOutputSymbol(pTimeFunction);
        action->decimalOutputSymbol = symbols.decimalOutputSymbols[buf];
        action->decimalOutputSymbolExpression = DecimalExpression::create(input,errorHandler,symbols,option,state);
        if (errorHandler.errorsOccurred)
        {
          errorHandler.error("XabslAction::create(): could not create decimal expression for output symbol \"%s\"",buf);
          delete action;
          return 0;
        }
        return action;
      }
    case 'b':
      XABSL_DEBUG_INIT(errorHandler.message("creating reference to boolean output symbol \"%s\"",buf));
      
      if (!symbols.booleanOutputSymbols.exists(buf))
      {
        errorHandler.error("XabslAction::create(): boolean output symbol \"%s\" does not exist",buf);
        return 0;
      }
      else
      {
        ActionBooleanOutputSymbol* action = new ActionBooleanOutputSymbol(pTimeFunction);
        action->booleanOutputSymbol = symbols.booleanOutputSymbols[buf];
        action->booleanOutputSymbolExpression = BooleanExpression::create(input,errorHandler,symbols,option,state);
        if (errorHandler.errorsOccurred)
        {
          errorHandler.error("XabslAction::create(): could not create boolean expression for output symbol \"%s\"",buf);
          delete action;
          return 0;
        }
        return action;
      }
    case 'e':
      XABSL_DEBUG_INIT(errorHandler.message("creating reference to enumerated output symbol \"%s\"",buf));
      
      if (!symbols.enumeratedOutputSymbols.exists(buf))
      {
        errorHandler.error("XabslAction::create(): enumerated output symbol \"%s\" does not exist",buf);
        return 0;
      }
      else
      {
        ActionEnumeratedOutputSymbol* action = new ActionEnumeratedOutputSymbol(pTimeFunction);
        action->enumeratedOutputSymbol = symbols.enumeratedOutputSymbols[buf];
        action->enumeratedOutputSymbolExpression = EnumeratedExpression::create(action->enumeratedOutputSymbol->enumeration,input,errorHandler,symbols,option,state);
        if (errorHandler.errorsOccurred)
        {
          errorHandler.error("XabslAction::create(): could not create enumerated expression for output symbol \"%s\"",buf);
          delete action;
          return 0;
        }
        return action;
      }
  }
  return 0;
}

Action* Action::create(
    Behavior* behavior,
    ErrorHandler& errorHandler,
    unsigned (*pTimeFunction)())
{
  ActionBehavior* action = 0;
  if (Option* option = dynamic_cast<Option*>(behavior))
  {
    action = new ActionOption(pTimeFunction);
    static_cast<ActionOption*>(action)->option = option;
  }
  else if (BasicBehavior* basicBehavior = dynamic_cast<BasicBehavior*>(behavior))
  {
    action = new ActionBasicBehavior(pTimeFunction);
    static_cast<ActionBasicBehavior*>(action)->basicBehavior = basicBehavior;
  }
  if (action != 0)
    action->parameters = new ParameterAssignment(behavior->parameters, errorHandler);
  return action;
}

ActionBehavior::~ActionBehavior()
{
  delete parameters;
}

ActionDecimalOutputSymbol::~ActionDecimalOutputSymbol()
{
  delete decimalOutputSymbolExpression;
}

ActionBooleanOutputSymbol::~ActionBooleanOutputSymbol() 
{
  delete booleanOutputSymbolExpression;
}

ActionEnumeratedOutputSymbol::~ActionEnumeratedOutputSymbol()
{
  delete enumeratedOutputSymbolExpression;
}

void ActionBehavior::execute()
{
  // execute subsequent option or basic behavior
  if (parameters->set())
    getBehavior()->parametersChanged();
  if (!getBehavior()->wasActive) getBehavior()->timeWhenActivated = pTimeFunction();
  getBehavior()->timeOfExecution = pTimeFunction()- getBehavior()->timeWhenActivated;
  getBehavior()->execute();
  getBehavior()->isActive = true;
}

void ActionDecimalOutputSymbol::execute()
{
  decimalOutputSymbolValue = decimalOutputSymbolExpression->getValue();
  decimalOutputSymbol->setValue(decimalOutputSymbolValue);
}

void ActionBooleanOutputSymbol::execute()
{
  booleanOutputSymbolValue = booleanOutputSymbolExpression->getValue();
  booleanOutputSymbol->setValue(booleanOutputSymbolValue);
}

void ActionEnumeratedOutputSymbol::execute()
{
  enumeratedOutputSymbolValue = enumeratedOutputSymbolExpression->getValue();
  enumeratedOutputSymbol->setValue(enumeratedOutputSymbolValue);
}

const Behavior* ActionOption::getBehavior() const
{
  return option;
}

const Behavior* ActionBasicBehavior::getBehavior() const
{
  return basicBehavior;
}

Behavior* ActionOption::getBehavior()
{
  return option;
}

Behavior* ActionBasicBehavior::getBehavior()
{
  return basicBehavior;
}

Behavior* Action::getBehavior()
{
  if (ActionBehavior* action = dynamic_cast<ActionBehavior*>(this))
    return action->getBehavior();
  else
    return 0;
}

Option* Action::getOption()
{
  if (ActionOption* action = dynamic_cast<ActionOption*>(this))
    return action->option;
  else
    return 0;
}

BasicBehavior* Action::getBasicBehavior()
{
  if (ActionBasicBehavior* action = dynamic_cast<ActionBasicBehavior*>(this))
    return action->basicBehavior;
  else
    return 0;
}

const Behavior* Action::getBehavior() const
{
  if (const ActionBehavior* action = dynamic_cast<const ActionBehavior*>(this))
    return action->getBehavior();
  else
    return 0;
}

const Option* Action::getOption() const
{
  if (const ActionOption* action = dynamic_cast<const ActionOption*>(this))
    return action->option;
  else
    return 0;
}

const BasicBehavior* Action::getBasicBehavior() const
{
  if (const ActionBasicBehavior* action = dynamic_cast<const ActionBasicBehavior*>(this))
    return action->basicBehavior;
  else
    return 0;
}

ParameterAssignment* Action::getParameters()
{
  if (ActionBehavior* action = dynamic_cast<ActionBehavior*>(this))
    return action->parameters;
  else
    return 0;
}

const ParameterAssignment* Action::getParameters() const
{
  if (const ActionBehavior* action = dynamic_cast<const ActionBehavior*>(this))
    return action->parameters;
  else
    return 0;
}

const DecimalOutputSymbol* Action::getDecimalOutputSymbol() const
{
  if (const ActionDecimalOutputSymbol* action = dynamic_cast<const ActionDecimalOutputSymbol*>(this))
    return action->decimalOutputSymbol;
  else
    return 0;
}

const BooleanOutputSymbol* Action::getBooleanOutputSymbol() const
{
  if (const ActionBooleanOutputSymbol* action = dynamic_cast<const ActionBooleanOutputSymbol*>(this))
    return action->booleanOutputSymbol;
  else
    return 0;
}

const EnumeratedOutputSymbol* Action::getEnumeratedOutputSymbol() const
{
  if (const ActionEnumeratedOutputSymbol* action = dynamic_cast<const ActionEnumeratedOutputSymbol*>(this))
    return action->enumeratedOutputSymbol;
  else
    return 0;
}

double Action::getDecimalOutputSymbolValue() const
{
  if (const ActionDecimalOutputSymbol* action = dynamic_cast<const ActionDecimalOutputSymbol*>(this))
    return action->decimalOutputSymbolValue;
  else
    return 0;
}

bool Action::getBooleanOutputSymbolValue() const
{
  if (const ActionBooleanOutputSymbol* action = dynamic_cast<const ActionBooleanOutputSymbol*>(this))
    return action->booleanOutputSymbolValue;
  else
    return false;
}

int Action::getEnumeratedOutputSymbolValue() const
{
  if (const ActionEnumeratedOutputSymbol* action = dynamic_cast<const ActionEnumeratedOutputSymbol*>(this))
    return action->enumeratedOutputSymbolValue;
  else
    return 0;
}

} // namespace
