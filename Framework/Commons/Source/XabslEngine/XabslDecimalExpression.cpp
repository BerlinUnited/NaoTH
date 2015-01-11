/** 
* @file XabslDecimalExpression.cpp
*
* Implementation of DecimalExpression and derivates
* 
* @author <a href="http://www.martin-loetzsch.de">Martin Loetzsch</a>
* @author <a href="http://www.sim.informatik.tu-darmstadt.de/pers/card/risler.html">Max Risler</a>
*/

#include "XabslOption.h"
#include "XabslDecimalExpression.h"
#include "XabslBooleanExpression.h"
#include "XabslEnumeratedExpression.h"

namespace xabsl 
{
  
DecimalExpression::~DecimalExpression()
{
}

DecimalExpression* DecimalExpression::create(InputSource& input, 
                                                         ErrorHandler& errorHandler,
                                                         Symbols& symbols,
                                                         Option& option,
                                                         State& state)
{
  char c[100];
  input.readString(c,1);
  ArithmeticOperator* arithmeticOperator;
  DecimalExpression* operand1;
  DecimalExpression* operand2;
  
  switch (*c)
  {
  case 'i':
    return new DecimalInputSymbolRef(input,errorHandler,symbols,option,state);
  case 'o':
    return new DecimalOutputSymbolRef(input,errorHandler,symbols);
  case 'c':
    // constants are treates as decimal values (there is no difference from the engines point of view.)
  case 'v':
    return new DecimalValue(input,errorHandler);
  case 'p':
    return new DecimalOptionParameterRef(input,errorHandler,option);
  case '+':
    if (!DecimalExpression::createOperand(operand1,input,errorHandler,symbols,option,state))
      return 0;
    if (!DecimalExpression::createOperand(operand2,input,errorHandler,symbols,option,state))
      return 0;
    
    XABSL_DEBUG_INIT(errorHandler.message("creating + operator"));
    arithmeticOperator = new PlusOperator();
    arithmeticOperator->create(operand1,operand2);
    return arithmeticOperator;
  case '-':
    if (!DecimalExpression::createOperand(operand1,input,errorHandler,symbols,option,state))
      return 0;
    if (!DecimalExpression::createOperand(operand2,input,errorHandler,symbols,option,state))
      return 0;
    
    XABSL_DEBUG_INIT(errorHandler.message("creating - operator"));
    arithmeticOperator = new MinusOperator();
    arithmeticOperator->create(operand1,operand2);
    return arithmeticOperator;
  case '*':
    if (!DecimalExpression::createOperand(operand1,input,errorHandler,symbols,option,state))
      return 0;
    if (!DecimalExpression::createOperand(operand2,input,errorHandler,symbols,option,state))
      return 0;
    
    XABSL_DEBUG_INIT(errorHandler.message("creating * operator"));
    arithmeticOperator = new MultiplyOperator();
    arithmeticOperator->create(operand1,operand2);
    return arithmeticOperator;
  case 'd':
    if (!DecimalExpression::createOperand(operand1,input,errorHandler,symbols,option,state))
      return 0;
    if (!DecimalExpression::createOperand(operand2,input,errorHandler,symbols,option,state))
      return 0;
    
    XABSL_DEBUG_INIT(errorHandler.message("creating / operator"));
    arithmeticOperator = new DivideOperator();
    arithmeticOperator->create(operand1,operand2);
    return arithmeticOperator;
    
  case '%':
    if (!DecimalExpression::createOperand(operand1,input,errorHandler,symbols,option,state))
      return 0;
    if (!DecimalExpression::createOperand(operand2,input,errorHandler,symbols,option,state))
      return 0;
    
    XABSL_DEBUG_INIT(errorHandler.message("creating % operator"));
    arithmeticOperator = new ModOperator();
    arithmeticOperator->create(operand1,operand2);
    return arithmeticOperator;
  case 's':
    return new TimeRef(errorHandler,state.timeOfStateExecution);
  case 't':
    return new TimeRef(errorHandler,option.timeOfExecution);
  case 'q':
    return new ConditionalDecimalExpression(input,errorHandler,symbols,option,state);
  default:
    errorHandler.error("XabslDecimalExpression::create(): unknown expression type: \"%c\"",*c);
    return 0;
  }
}

bool DecimalExpression::createOperand(DecimalExpression*& operand,
                                            InputSource& input, 
                                            ErrorHandler& errorHandler,
                                            Symbols& symbols,
                                            Option& option,
                                            State& state)
{
  operand = DecimalExpression::create(input,errorHandler,symbols,option,state);
  
  if (operand == 0) 
  {
    errorHandler.error("XabslDecimalExpression::createOperand(): created operand is 0");
    return false;
  }
  
  if (errorHandler.errorsOccurred)
  {
    errorHandler.error("XabslDecimalExpression::createOperand(): could not create operand");
    delete operand;
    return false;
  }
  
  return true;
}

DecimalValue::DecimalValue(InputSource& input, 
                                       ErrorHandler& XABSL_DEBUG_INIT(errorHandler))
{
  value = input.readValue();
  
  XABSL_DEBUG_INIT(errorHandler.message("created decimal value: \"%.2f\"",value));
}

double DecimalValue::getValue() const
{
  return value;
}

DecimalOptionParameterRef::DecimalOptionParameterRef(InputSource& input, 
                                                   ErrorHandler& errorHandler,
                                                   Option& option)
{
  char buf[100];
  input.readString(buf,99);
  
  XABSL_DEBUG_INIT(errorHandler.message("creating a reference to decimal option parameter \"%s\"",buf));
  
  if (!option.parameters->decimal.exists(buf))
  {
    errorHandler.error("XabslDecimalOptionParameterRef::DecimalOptionParameterRef(): decimal option parameter \"%s\" does not exist",buf);
    return;
  }
  
  parameter = option.parameters->decimal.getPElement(buf)->e;
}

double DecimalOptionParameterRef::getValue() const
{
  return *parameter;
}

void ArithmeticOperator::create(DecimalExpression* operand1, DecimalExpression* operand2)
{
  this->operand1 = operand1;
  this->operand2 = operand2;
}

ArithmeticOperator::~ArithmeticOperator()
{
  delete operand1;
  delete operand2;
}

double PlusOperator::getValue() const
{
  return operand1->getValue() + operand2->getValue();
}

double MinusOperator::getValue() const
{
  return operand1->getValue() - operand2->getValue();
}

double MultiplyOperator::getValue() const
{
  return operand1->getValue() * operand2->getValue();
}

double DivideOperator::getValue() const
{
  double o2 = operand2->getValue();
  if (o2==0) 
    return operand1->getValue() / 0.0000001;
  else
    return operand1->getValue() / o2;
}

double ModOperator::getValue() const
{
  return (int)operand1->getValue() % (int)operand2->getValue();
}

TimeRef::TimeRef(ErrorHandler& XABSL_DEBUG_INIT(errorHandler),
                             unsigned& time) :
time(time)
{
  XABSL_DEBUG_INIT(errorHandler.message("creating a reference to state or option execution time"));
}

double TimeRef::getValue() const
{
  return time;
}

DecimalInputSymbolRef::DecimalInputSymbolRef(InputSource& input, 
                                                               ErrorHandler& errorHandler,
                                                               Symbols& symbols,
                                                               Option& option,
                                                               State& state) :
  symbol(0), parameters(0)
{
  char buf[100];
  input.readString(buf,99);
  
  XABSL_DEBUG_INIT(errorHandler.message("creating a reference to decimal input symbol \"%s\"",buf));
  
  if (!symbols.decimalInputSymbols.exists(buf))
  {
    errorHandler.error("XabslDecimalInputSymbolRef::DecimalInputSymbolRef(): decimal input symbol \"%s\" was not registered",buf);
    return;
  }
  
  symbol = symbols.decimalInputSymbols[buf];

  parameters = new ParameterAssignment(&symbol->parameters, errorHandler);

  parameters->create(input, symbols, option, state);
}

DecimalInputSymbolRef::~DecimalInputSymbolRef()
{
  delete parameters;
}

double DecimalInputSymbolRef::getValue() const
{
  // set the symbol parameters
  if (parameters->set())
    symbol->parametersChanged();

  return symbol->getValue();
}

DecimalOutputSymbolRef::DecimalOutputSymbolRef(InputSource& input, 
                                                               ErrorHandler& errorHandler,
                                                               Symbols& symbols)
{
  char buf[100];
  input.readString(buf,99);
  
  XABSL_DEBUG_INIT(errorHandler.message("creating a reference to a decimal output symbol \"%s\"",buf));
  
  if (!symbols.decimalOutputSymbols.exists(buf))
  {
    errorHandler.error("XabslDecimalOutputSymbolRef::DecimalOutputSymbolRef(): decimal output symbol \"%s\" was not registered",buf);
    return;
  }
  
  symbol = symbols.decimalOutputSymbols[buf];
}

double DecimalOutputSymbolRef::getValue() const
{
  return symbol->getValue();
}

ConditionalDecimalExpression::ConditionalDecimalExpression(InputSource& input, 
    ErrorHandler& errorHandler,
    Symbols& symbols,
    Option& option,
    State& state)
{
  XABSL_DEBUG_INIT(errorHandler.message("creating a question mark operator"));

  condition = BooleanExpression::create(input,errorHandler,symbols,option,state);

  if (condition == 0) 
  {
    errorHandler.error("XabslQuestionMarkOperator::QuestionMarkOperator(): created condition is 0");
    return;
  }
  else if (errorHandler.errorsOccurred)
  {
    errorHandler.error("XabslQuestionMarkOperator::QuestionMarkOperator(): could not create condition");
    delete condition;
    return;
  }

  if (!DecimalExpression::createOperand(expression1,input,errorHandler,symbols,option,state))
  {
    errorHandler.error("XabslQuestionMarkOperator::QuestionMarkOperator(): could not create decimal expression1");
    return;
  }
  
  if (!DecimalExpression::createOperand(expression2,input,errorHandler,symbols,option,state))
  {
    errorHandler.error("XabslQuestionMarkOperator::QuestionMarkOperator(): could not create decimal expression2");
    return;
  }
  
}

ConditionalDecimalExpression::~ConditionalDecimalExpression()
{
  delete condition;
  delete expression1;
  delete expression2;
}

double ConditionalDecimalExpression::getValue() const
{
  if (condition->getValue())
  {
    return expression1->getValue();
  }
  else
  {
    return expression2->getValue();
  }
}

} // namespace

