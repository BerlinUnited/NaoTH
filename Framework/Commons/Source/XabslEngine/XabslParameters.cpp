/** 
* @file XabslParameters.cpp
*
* Implementation of class Parameters 
*
* @author <a href="http://www.sim.informatik.tu-darmstadt.de/pers/card/risler.html">Max Risler</a>
*/

#include "XabslParameters.h"
#include "XabslSymbols.h"
#include "XabslOption.h"
#include "XabslDecimalExpression.h"
#include "XabslBooleanExpression.h"
#include "XabslEnumeratedExpression.h"

namespace xabsl 
{

void Parameters::reset()
{
  for (int i=0; i<decimal.getSize(); i++) *decimal[i] = 0; 
  for (int i=0; i<boolean.getSize(); i++) *boolean[i] = false; 
  for (int i=0; i<enumerated.getSize(); i++) *enumerated[i] = enumerations[i]->enumElements[0]->v; 
}

void Parameters::registerEnumerations(NamedArray<Enumeration*>& enumerations)
{
  pEnumerations = &enumerations;
}

void Parameters::registerDecimal(const char* name, double& parameter)
{
  XABSL_DEBUG_INIT(errorHandler.message("registering decimal parameter \"%s\"",name));
  
  if (decimal.exists(name))
  {
    errorHandler.error("registerDecimalParameter(): decimal parameter \"%s\" was already registered",name);
    return;
  }
  decimal.append(name,&parameter);
  parameter = 0;
};

void Parameters::registerBoolean(const char* name, bool& parameter)
{
  XABSL_DEBUG_INIT(errorHandler.message("registering boolean parameter \"%s\"",name));
  
  if (boolean.exists(name))
  {
    errorHandler.error("registerBooleanParameter(): boolean parameter \"%s\" was already registered",name);
    return;
  }
  boolean.append(name,&parameter);
  parameter = false;
};

void Parameters::registerEnumerated(const char* name, const char* enumName, int& parameter)
{
  XABSL_DEBUG_INIT(errorHandler.message("registering enumerated parameter \"%s\"",name));
  
  if (pEnumerations == 0)
  {
    errorHandler.error("XabslParameters::registerEnumerated(): call Parameters::registerEnumerations first");
    return;
  }
  if (enumerated.exists(name))
  {
    errorHandler.error("XabslParameters::registerEnumerated(): enumerated parameter \"%s\" was already registered",name);
    return;
  }
  if (!pEnumerations->exists(enumName))
  {
    errorHandler.error("XabslParameters::registerEnumerated(): enumeration \"%s\" was not registered",enumName);
    return;
  }
  const Enumeration* enumeration = (*pEnumerations)[enumName];
  if (enumeration->enumElements.getSize() == 0)
  {
    errorHandler.error("XabslParameters::registerEnumerated(): no enumeration elements for \"%s\" were registered",enumName);
    return;
  }

  enumerations.append(name, enumeration);
  enumerated.append(name,&parameter);

  parameter = enumeration->enumElements[0]->v;
};

ParameterAssignment::ParameterAssignment(
  Parameters* refParameters,
  ErrorHandler& errorHandler) :
    Parameters(errorHandler)
{
  for (int i = 0; i < refParameters->decimal.getSize(); i++)
  {
    decimal.append(refParameters->decimal.getName(i), refParameters->decimal.getElement(i));
    decimalValues.append(refParameters->decimal.getName(i), 0);
    decimalExpressions.append(refParameters->decimal.getName(i), new DecimalValue(0));
  }
  for (int i = 0; i < refParameters->boolean.getSize(); i++)
  {
    boolean.append(refParameters->boolean.getName(i), refParameters->boolean.getElement(i));
    booleanValues.append(refParameters->boolean.getName(i), false);
    booleanExpressions.append(refParameters->boolean.getName(i), new BooleanValue(false));
  }
  for (int i = 0; i < refParameters->enumerated.getSize(); i++)
  {
    enumerated.append(refParameters->enumerated.getName(i), refParameters->enumerated.getElement(i));
    enumeratedValues.append(refParameters->enumerated.getName(i), false);
    enumeratedExpressions.append(refParameters->enumerated.getName(i), new EnumeratedValue(refParameters->enumerations[i], 0));
  }
}
    
void ParameterAssignment::create(
    InputSource& input,    
    Symbols& symbols,
    Option& option,
    State& state)
{
  int i;
  // read the parameters 
  int numberOfParameters = (int)input.readValue();
  
  for (i = 0; i<numberOfParameters; i++)
  {
    char c[2];
    char buf[100];
    input.readString(c,1);
    switch(c[0])
    {
      case 'd':
        {
          input.readString(buf,99);
          XABSL_DEBUG_INIT(errorHandler.message("creating expession for set decimal parameter \"%s\"",buf));
          
          const DecimalExpression* exp = DecimalExpression::create(input,errorHandler,symbols,option,state);
          if (errorHandler.errorsOccurred)
          {
            errorHandler.error("XabslParameterAssignment::create(): could not create decimal expression for parameter \"%s\"",buf);
            return;
          }
          if (!setDecimalParameter(buf, exp)) return;
        }
        break;
      case 'b':
        {
          input.readString(buf,99);
          XABSL_DEBUG_INIT(errorHandler.message("creating expession for set boolean parameter \"%s\"",buf));
          
          const BooleanExpression* exp = BooleanExpression::create(input,errorHandler,symbols,option,state);
          if (errorHandler.errorsOccurred)
          {
            errorHandler.error("XabslParameterAssignment::create(): could not create boolean expression for parameter \"%s\"",buf);
            return;
          }
          if (!setBooleanParameter(buf, exp)) return;
        }
        break;
      case 'e':
        {
          input.readString(buf,99);
          if(!symbols.enumerations.exists(buf))
          {
            errorHandler.error("XabslParameterAssignment::create(): enumeration \"%s\" was not registered",buf);
            return;
          }
          const Enumeration* enumeration = symbols.enumerations[buf];

          input.readString(buf,99);

          XABSL_DEBUG_INIT(errorHandler.message("creating expession for set enumerated parameter \"%s\"",buf));
          const EnumeratedExpression* exp = EnumeratedExpression::create(enumeration,input,errorHandler,symbols,option,state);
          if (errorHandler.errorsOccurred)
          {
            errorHandler.error("XabslParameterAssignment::create(): could not create enumerated expression for parameter \"%s\"",buf);
            return;
          }
          
          if (!setEnumeratedParameter(buf, exp)) return;
        }
        break;
    }
  }
}

ParameterAssignment::~ParameterAssignment()
{
  for (int i=0; i<decimalExpressions.getSize(); i++)
    delete decimalExpressions[i];
  for (int i=0; i<booleanExpressions.getSize(); i++)
    delete booleanExpressions[i];
  for (int i=0; i<enumeratedExpressions.getSize(); i++)
    delete enumeratedExpressions[i];
}

bool ParameterAssignment::setDecimalParameter(const char* param, const DecimalExpression* exp)
{
  if (!decimalExpressions.exists(param))
  {
    errorHandler.error("XabslParameterAssignment::setDecimalParameter(): decimal parameter \"%s\" does not exist", param);
    delete exp;
    return false;
  }
  delete decimalExpressions[param];
  decimalExpressions.setElement(param, exp);

  return true;
}

bool ParameterAssignment::setBooleanParameter(const char* param, const BooleanExpression* exp)
{
  if (!booleanExpressions.exists(param))
  {
    errorHandler.error("XabslParameterAssignment::setBooleanParameter(): boolean parameter \"%s\" does not exist",param);
    delete exp;
    return false;
  }
  delete booleanExpressions[param];
  booleanExpressions.setElement(param, exp);

  return true;
}

bool ParameterAssignment::setEnumeratedParameter(const char* param, const EnumeratedExpression* exp)
{
  if (!enumeratedExpressions.exists(param))
  {
    errorHandler.error("XabslParameterAssignment::create(): enumerated parameter \"%s\" does not exist",param);
    delete exp;
    return false;
  }
  if (exp->enumeration != enumeratedExpressions[param]->enumeration)
  {
    errorHandler.error("XabslParameterAssignment::create(): type mismatch on enumerated parameter \"%s\", expected \"%s\", found \"%s\"", param, enumeratedExpressions[param]->enumeration->n, exp->enumeration->n);
    return false;
  }
  delete enumeratedExpressions[param];
  enumeratedExpressions.setElement(param, exp);

  return true;
}

bool ParameterAssignment::setDecimalParameter(const char* param, double value)
{
  return setDecimalParameter(param, new DecimalValue(value));
}

bool ParameterAssignment::setBooleanParameter(const char* param, bool value)
{
  return setBooleanParameter(param, new BooleanValue(value));
}

bool ParameterAssignment::setEnumeratedParameter(const char* param, const Enumeration* enumeration, int value)
{
  return setEnumeratedParameter(param, new EnumeratedValue(enumeration, value));
}

bool ParameterAssignment::setEnumeratedParameter(const char* param, const char* value)
{
  if (!enumeratedExpressions.exists(param))
  {
    return false;
  }
  else if (!enumeratedExpressions[param]->enumeration->enumElements.exists(value))
  {
    return false;
  }
  else 
  { 
    return setEnumeratedParameter(param, enumeratedExpressions[param]->enumeration, enumeratedExpressions[param]->enumeration->enumElements[value]->v);
  }
}

bool ParameterAssignment::set()
{
  int i;
  bool parametersChanged = false;
 
  // Set the parameters.
  for (i=0; i<decimal.getSize(); i++)
  {
    decimalValues.setElement(i, decimalExpressions[i]->getValue());
    if (*decimal[i] != decimalValues[i])
    {
      parametersChanged = true;
      *decimal[i] = decimalValues[i];
    }
  }
  for (i=0; i<boolean.getSize(); i++)
  {
    booleanValues.setElement(i, booleanExpressions[i]->getValue());
    if (*boolean[i] != booleanValues[i])
    {
      parametersChanged = true;
      *boolean[i] = booleanValues[i];
    }
  }
  for (i=0; i<enumerated.getSize(); i++)
  {
    enumeratedValues.setElement(i, enumeratedExpressions[i]->getValue());
    if (*enumerated[i] != enumeratedValues[i])
    {
      parametersChanged = true;
      *enumerated[i] = enumeratedValues[i];
    }
  }

  return parametersChanged;
}

} // namespace

