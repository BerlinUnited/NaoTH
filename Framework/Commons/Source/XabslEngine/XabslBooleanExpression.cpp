/** 
* @file XabslBooleanExpression.cpp
*
* Implementation of BooleanExpression and derivates
* 
* @author <a href="http://www.martin-loetzsch.de">Martin Loetzsch</a>
* @author <a href="http://www.sim.informatik.tu-darmstadt.de/pers/card/risler.html">Max Risler</a>
*/

#include "XabslBooleanExpression.h"
#include "XabslOption.h"

namespace xabsl 
{
  
BooleanExpression::~BooleanExpression()
{
}

BooleanExpression* BooleanExpression::create(InputSource& input, 
                                                         ErrorHandler& errorHandler,
                                                         Symbols& symbols,
                                                         Option& option,
                                                         State& state)
{
  char c[100];
  BooleanExpression* booleanOperand = 0;
  DecimalExpression* decimalOperand1 = 0;
  DecimalExpression* decimalOperand2 = 0;
  RelationalAndEqualityOperator* relationalAndEqualityOperator = 0;
  input.readString(c,1);
  
  switch (*c)
  {
  case 'v':
    return new BooleanValue(input,errorHandler);
  case 'p':
    return new BooleanOptionParameterRef(input,errorHandler,option);
  case 'i':
    return new BooleanInputSymbolRef(input,errorHandler,symbols,option,state);
  case 'o':
    return new BooleanOutputSymbolRef(input,errorHandler,symbols);
  case 't':
    return new SubsequentOptionReachedTargetStateCondition(errorHandler, state);
  case 'c':
    return new EnumeratedInputSymbolComparison(input, errorHandler, symbols, option, state);
  case '&':
    {
      XABSL_DEBUG_INIT(errorHandler.message("creating and operator"));
      
      int numberOfOperands = (int)input.readValue();
      
      AndOperator* andOperator = new AndOperator();
      
      for (int i=0; i<numberOfOperands; i++)
      {
        if (!BooleanExpression::createOperand(booleanOperand, input, errorHandler, symbols, option, state))
        {
          delete andOperator;
          return 0;
        }
        
        andOperator->addOperand(booleanOperand);
      }
      
      return andOperator;
    }
  case '|':
    {
      XABSL_DEBUG_INIT(errorHandler.message("creating or operator"));
      
      int numberOfOperands = (int)input.readValue();
      
      OrOperator* orOperator = new OrOperator();
      
      for (int i=0; i<numberOfOperands; i++)
      {
        if (!BooleanExpression::createOperand(booleanOperand, input, errorHandler, symbols, option, state))
        {
          delete orOperator;
          return 0;
        }
        orOperator->addOperand(booleanOperand);
      }
      
      return orOperator;
    }
  case '!':
    XABSL_DEBUG_INIT(errorHandler.message("creating not operator"));
    
    if (!BooleanExpression::createOperand(booleanOperand, input, errorHandler, symbols, option, state))
      return 0;
    
    return new NotOperator(booleanOperand);
  case '=':
    XABSL_DEBUG_INIT(errorHandler.message("creating == operator"));
    
    if (!DecimalExpression::createOperand(decimalOperand1,input, errorHandler, symbols, option, state))
      return 0;
    
    if (!DecimalExpression::createOperand(decimalOperand2,input, errorHandler, symbols, option, state))
      return 0;
    
    relationalAndEqualityOperator = new EqualToOperator();
    relationalAndEqualityOperator->create(decimalOperand1,decimalOperand2);
    return relationalAndEqualityOperator;
  case 'n':
    XABSL_DEBUG_INIT(errorHandler.message("creating != operator"));
    
    if (!DecimalExpression::createOperand(decimalOperand1,input,errorHandler,symbols,option,state))
      return 0;
    
    if (!DecimalExpression::createOperand(decimalOperand2,input,errorHandler,symbols,option,state))
      return 0;
    
    relationalAndEqualityOperator = new NotEqualToOperator();
    relationalAndEqualityOperator->create(decimalOperand1,decimalOperand2);
    return relationalAndEqualityOperator;
  case '<':
    XABSL_DEBUG_INIT(errorHandler.message("creating < operator"));
    
    if (!DecimalExpression::createOperand(decimalOperand1,input,errorHandler,symbols,option,state))
      return 0;
    
    if (!DecimalExpression::createOperand(decimalOperand2,input,errorHandler,symbols,option,state))
      return 0;
    
    relationalAndEqualityOperator = new LessThanOperator();
    relationalAndEqualityOperator->create(decimalOperand1,decimalOperand2);
    return relationalAndEqualityOperator;
  case 'l':
    XABSL_DEBUG_INIT(errorHandler.message("creating <= operator"));
    
    if (!DecimalExpression::createOperand(decimalOperand1,input,errorHandler, symbols, option, state))
      return 0;
    
    if (!DecimalExpression::createOperand(decimalOperand2,input,errorHandler, symbols, option, state))
      return 0;
    
    relationalAndEqualityOperator = new LessThanOrEqualToOperator();
    relationalAndEqualityOperator->create(decimalOperand1,decimalOperand2);
    return relationalAndEqualityOperator;
  case '>':
    XABSL_DEBUG_INIT(errorHandler.message("creating > operator"));
    
    if (!DecimalExpression::createOperand(decimalOperand1,input,errorHandler, symbols, option, state))
      return 0;
    
    if (!DecimalExpression::createOperand(decimalOperand2,input,errorHandler, symbols, option, state))
      return 0;
    
    relationalAndEqualityOperator = new GreaterThanOperator();
    relationalAndEqualityOperator->create(decimalOperand1,decimalOperand2);
    return relationalAndEqualityOperator;
  case 'g':
    XABSL_DEBUG_INIT(errorHandler.message("creating >= operator"));
    
    if (!DecimalExpression::createOperand(decimalOperand1,input,errorHandler, symbols, option, state))
      return 0;
    
    if (!DecimalExpression::createOperand(decimalOperand2,input,errorHandler, symbols, option, state))
      return 0;
    
    relationalAndEqualityOperator = new GreaterThanOrEqualToOperator();
    relationalAndEqualityOperator->create(decimalOperand1,decimalOperand2);
    return relationalAndEqualityOperator;
  case 'x':
    return new ConflictCondition(errorHandler, state);
  default:
    errorHandler.error("XabslBooleanExpression::create(): unknown expression type \"%c\"",*c);
  }
  return 0;
}

bool BooleanExpression::createOperand(BooleanExpression*& operand,
                                            InputSource& input, 
                                            ErrorHandler& errorHandler,
                                            Symbols& symbols,
                                            Option& option,
                                            State& state)
{
  operand = BooleanExpression::create(input,errorHandler,symbols,option,state);
  
  if (operand == 0) 
  {
    errorHandler.error("XabslBooleanExpression::createOperand(): created operand is 0");
    return false;
  }
  
  if (errorHandler.errorsOccurred)
  {
    errorHandler.error("XabslBooleanExpression::createOperand(): could not create operand");
    delete operand;
    return false;
  }
  
  return true;
}

BooleanValue::BooleanValue(InputSource& input, 
                                       ErrorHandler& XABSL_DEBUG_INIT(errorHandler))
{
  char buf[6];
  input.readString(buf,5);

  value = (strcmp("true",buf) == 0);
  
  XABSL_DEBUG_INIT(errorHandler.message("created boolean value: \"%s\"",value?"true":"false"));
}

bool BooleanValue::getValue() const
{
  return value;
}

BooleanOptionParameterRef::BooleanOptionParameterRef(InputSource& input, 
                                                   ErrorHandler& errorHandler,
                                                   Option& option)
{
  char buf[100];
  input.readString(buf,99);
  
  XABSL_DEBUG_INIT(errorHandler.message("creating a reference to boolean option parameter \"%s\"",buf));
  
  if (!option.parameters->boolean.exists(buf))
  {
    errorHandler.error("XabslBooleanOptionParameterRef::BooleanOptionParameterRef(): boolean option parameter \"%s\" does not exist",buf);
    return;
  }
  
  parameter = option.parameters->boolean.getPElement(buf)->e;
}

bool BooleanOptionParameterRef::getValue() const
{
  return *parameter;
}

AndOperator::AndOperator() 
{
  operands.clear();
}

AndOperator::~AndOperator()
{
  for (int i=0; i< operands.getSize(); i++)
  {
    delete operands[i];
  }
}

bool AndOperator::getValue() const
{
  for (int i=0; i< operands.getSize(); i++)
  {
    if (operands[i]->getValue() == false) return false;
  }
  return true;
}

void AndOperator::addOperand(BooleanExpression* operand)
{
  operands.append(operand);
}

OrOperator::OrOperator() 
{
  operands.clear();
}

OrOperator::~OrOperator()
{
  for (int i=0; i< operands.getSize(); i++)
  {
    delete operands[i];
  }
}

bool OrOperator::getValue() const
{
  for (int i=0; i< operands.getSize(); i++)
  {
    if (operands[i]->getValue() == true) return true;
  }
  return false;
}

void OrOperator::addOperand(BooleanExpression* operand)
{
  operands.append(operand);
}

NotOperator::NotOperator(BooleanExpression* operand1) :
operand1(operand1)
{
}

NotOperator::~NotOperator()
{
  delete operand1;
}

bool NotOperator::getValue() const
{
  return !(operand1->getValue());
}

BooleanInputSymbolRef::BooleanInputSymbolRef(InputSource& input, 
                                                               ErrorHandler& errorHandler,
                                                               Symbols& symbols,
                                                               Option& option,
                                                               State& state) :
  symbol(0), parameters(0)
{
  char buf[100];
  input.readString(buf,99);
  
  XABSL_DEBUG_INIT(errorHandler.message("creating reference to boolean input symbol \"%s\"",buf));
  
  if (!symbols.booleanInputSymbols.exists(buf))
  {
    errorHandler.error("XabslBooleanInputSymbolRef::XabslBooleanInputSymbolRef(): boolean input symbol \"%s\" was not registered at the engine",buf);
    return;
  }
  
  symbol = symbols.booleanInputSymbols[buf];

  parameters = new ParameterAssignment(&symbol->parameters, errorHandler);

  parameters->create(input, symbols, option, state);
}
  
BooleanInputSymbolRef::~BooleanInputSymbolRef()
{
  delete parameters;
}

bool BooleanInputSymbolRef::getValue() const
{
  // set the symbol parameters
  if (parameters->set())
    symbol->parametersChanged();

  return symbol->getValue();
}

BooleanOutputSymbolRef::BooleanOutputSymbolRef(InputSource& input, 
                                                               ErrorHandler& errorHandler,
                                                               Symbols& symbols)
{
  char buf[100];
  input.readString(buf,99);
  
  XABSL_DEBUG_INIT(errorHandler.message("creating a reference to a boolean output symbol \"%s\"",buf));
  
  if (!symbols.booleanOutputSymbols.exists(buf))
  {
    errorHandler.error("XabslBooleanOutputSymbolRef::BooleanOutputSymbolRef(): boolean output symbol \"%s\" was not registered",buf);
    return;
  }
  
  symbol = symbols.booleanOutputSymbols[buf];
}

bool BooleanOutputSymbolRef::getValue() const
{
  return symbol->getValue();
}

SubsequentOptionReachedTargetStateCondition::SubsequentOptionReachedTargetStateCondition(
                                                   ErrorHandler& XABSL_DEBUG_INIT(errorHandler),
                                                   State& state)
                                                   : state(state)
{
  XABSL_DEBUG_INIT(errorHandler.message("creating a \"subsequent-option-reached-target-state\" element"));
}

bool SubsequentOptionReachedTargetStateCondition::getValue() const
{
  bool anySubsequentBehaviorReachedTargetState = false;
  for (int i = 0; i < state.actions.getSize(); i++)
    if (ActionOption* subsequentAction = dynamic_cast<ActionOption*>(state.actions[i]))
      if (subsequentAction->option->getOptionReachedATargetState())
        anySubsequentBehaviorReachedTargetState = true;
  return anySubsequentBehaviorReachedTargetState;
}

ConflictCondition::ConflictCondition(
                                   ErrorHandler& XABSL_DEBUG_INIT(errorHandler),
                                   State& state)
                                   : state(state)
{
  XABSL_DEBUG_INIT(errorHandler.message("creating a \"conflict\" element"));
}

bool ConflictCondition::getValue() const
{
  return state.getConflict();
}

EnumeratedInputSymbolComparison::EnumeratedInputSymbolComparison(InputSource& input,
  ErrorHandler& errorHandler,
  Symbols& symbols,
  Option& option,
  State& state)
{
  if (!EnumeratedExpression::createOperand(operand1,NULL,input,errorHandler,symbols,option,state))
  {
    errorHandler.error("XabslEnumeratedInputSymbolComparison::EnumeratedInputSymbolComparison(): could not create enumerated expression");
    operand1 = operand2 = 0;
    return;
  }
  if (!EnumeratedExpression::createOperand(operand2,operand1->enumeration,input,errorHandler,symbols,option,state))
  {
    errorHandler.error("XabslEnumeratedInputSymbolComparison::EnumeratedInputSymbolComparison(): could not create enumerated expression");
    operand2 = 0;
    return;
  }
}

EnumeratedInputSymbolComparison::~EnumeratedInputSymbolComparison()
{
  delete operand1;
  delete operand2;
}

bool EnumeratedInputSymbolComparison::getValue() const
{
  return (operand1->getValue() == operand2->getValue());
}

void RelationalAndEqualityOperator::create(DecimalExpression* operand1,
                                                 DecimalExpression* operand2)
{
  this->operand1 = operand1;
  this->operand2 = operand2;
}

RelationalAndEqualityOperator ::~RelationalAndEqualityOperator ()
{
  delete operand1;
  delete operand2;
}

bool EqualToOperator::getValue() const
{
  return (operand1->getValue() == operand2->getValue());
}

bool NotEqualToOperator::getValue() const
{
  return (operand1->getValue() != operand2->getValue());
}

bool LessThanOperator::getValue() const
{
  return (operand1->getValue() < operand2->getValue());
}

bool LessThanOrEqualToOperator::getValue() const
{
  return (operand1->getValue() <= operand2->getValue());
}

bool GreaterThanOperator::getValue() const
{
  return (operand1->getValue() > operand2->getValue());
}

bool GreaterThanOrEqualToOperator::getValue() const
{
  return (operand1->getValue() >= operand2->getValue());
}

} // namespace

