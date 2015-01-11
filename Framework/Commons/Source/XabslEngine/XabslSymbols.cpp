/**
* @file XabslSymbols.cpp
*
* Implementation of class Symbols and helper classes
*
* @author <a href="http://www.martin-loetzsch.de">Martin Loetzsch</a>
* @author <a href="http://www.sim.informatik.tu-darmstadt.de/pers/card/risler.html">Max Risler</a>
*/

#include "XabslSymbols.h"

namespace xabsl 
{

Enumeration::~Enumeration()
{
  int i;
  for (i=0; i< enumElements.getSize(); i++)
  {
    delete enumElements[i];
  }
}

Symbols::~Symbols()
{
  int i;
  for (i=0; i< enumerations.getSize(); i++) delete enumerations[i];
  for (i=0; i< decimalInputSymbols.getSize(); i++) delete decimalInputSymbols[i];
  for (i=0; i< booleanInputSymbols.getSize(); i++) delete booleanInputSymbols[i];
  for (i=0; i< enumeratedInputSymbols.getSize(); i++) delete enumeratedInputSymbols[i];
  for (i=0; i< decimalOutputSymbols.getSize(); i++) delete decimalOutputSymbols[i];
  for (i=0; i< booleanOutputSymbols.getSize(); i++) delete booleanOutputSymbols[i];
  for (i=0; i< enumeratedOutputSymbols.getSize(); i++) delete enumeratedOutputSymbols[i];
}

void Symbols::registerEnumElement(const char* symbolName, const char* name, int value)
{
  XABSL_DEBUG_INIT(errorHandler.message("registering enum element \"%s\" for enumeration \"%s\"",name, symbolName));

  if (!enumerations.exists(symbolName))
  {
    enumerations.append(symbolName, new Enumeration(symbolName, enumerations.getSize()));
  }
  if (enumerations[symbolName]->enumElements.exists(name))
  {
    errorHandler.error("registerEnumElement(): enum element \"%s\" for enumeration \"%s\" was already registered.", name, symbolName);
    return;
  }
  enumerations[symbolName]->enumElements.append(name, new EnumElement(name,value));
}

void Symbols::registerDecimalInputSymbol(const char* name, const double* pVariable)
{
  XABSL_DEBUG_INIT(errorHandler.message("registering decimal input symbol \"%s\"",name));
  
  if (decimalInputSymbols.exists(name))
  {
    errorHandler.error("registerDecimalInputSymbol(): symbol \"%s\" was already registered",name);
    return;
  }
  decimalInputSymbols.append(name,new DecimalInputSymbol(name, pVariable, errorHandler, decimalInputSymbols.getSize()));
}


void Symbols::registerDecimalInputSymbol
(const char* name,
 double (*pFunction)())
{
  XABSL_DEBUG_INIT(errorHandler.message("registering decimal input symbol \"%s\"",name));
  
  if (decimalInputSymbols.exists(name))
  {
    errorHandler.error("registerDecimalInputSymbol(): symbol \"%s\" was already registered",name);
    return;
  }
  decimalInputSymbols.append(name,new DecimalInputSymbol(name, pFunction, errorHandler, decimalInputSymbols.getSize()));
}

void Symbols::registerDecimalInputSymbolParametersChanged(const char* name,
    void (*pFunction)())
{
  XABSL_DEBUG_INIT(errorHandler.message("registering parameter change notification for decimal input symbol\"%s\"",name));

  if (!decimalInputSymbols.exists(name))
  {
    errorHandler.error("registerDecimalInputSymbolParametersChanged(): symbol \"%s\" was not registered",name);
    return;
  }

  decimalInputSymbols[name]->pParametersChanged = pFunction;
}

void Symbols::registerDecimalInputSymbolDecimalParameter(const char* symbolName, 
    const char* name, double* pParam)
{
  XABSL_DEBUG_INIT(errorHandler.message("registering decimal parameter \"%s\" for decimal input symbol\"%s\"",name, symbolName));

  if (!decimalInputSymbols.exists(symbolName))
  {
    errorHandler.error("registerDecimalInputSymbolDecimalParameter(): symbol \"%s\" was not registered",symbolName);
    return;
  }
  if (decimalInputSymbols[symbolName]->parameters.decimal.exists(name))
  {
    errorHandler.error("registerDecimalInputSymbolDecimalParameter(): parameter \"%s\" was already registered",name);
    return;
  }

  decimalInputSymbols[symbolName]->parameters.decimal.append(name,pParam);
}

void Symbols::registerDecimalInputSymbolBooleanParameter(const char* symbolName, 
    const char* name, bool* pParam)
{
  XABSL_DEBUG_INIT(errorHandler.message("registering boolean parameter \"%s\" for decimal input symbol\"%s\"",name, symbolName));

  if (!decimalInputSymbols.exists(symbolName))
  {
    errorHandler.error("registerDecimalInputSymbolBooleanParameter(): symbol \"%s\" was not registered",symbolName);
    return;
  }
  if (decimalInputSymbols[symbolName]->parameters.boolean.exists(name))
  {
    errorHandler.error("registerDecimalInputSymbolBooleanParameter(): parameter \"%s\" was already registered",name);
    return;
  }

  decimalInputSymbols[symbolName]->parameters.boolean.append(name,pParam);
}

void Symbols::registerDecimalInputSymbolEnumeratedParameter(const char* symbolName, 
    const char* name, const char* enumName, int* pParam)
{
  XABSL_DEBUG_INIT(errorHandler.message("registering enumerated parameter \"%s\" for decimal input symbol\"%s\"",name, symbolName));

  if (!decimalInputSymbols.exists(symbolName))
  {
    errorHandler.error("registerDecimalInputSymbolEnumeratedParameter(): symbol \"%s\" was not registered",symbolName);
    return;
  }
  if (decimalInputSymbols[symbolName]->parameters.enumerated.exists(name))
  {
    errorHandler.error("registerDecimalInputSymbolEnumeratedParameter(): parameter \"%s\" was already registered",name);
    return;
  }

  if (!enumerations.exists(enumName))
  {
    enumerations.append(enumName, new Enumeration(enumName, enumerations.getSize()));
  }
  decimalInputSymbols[symbolName]->parameters.enumerations.append(name,enumerations.getElement(enumName));
  decimalInputSymbols[symbolName]->parameters.enumerated.append(name,pParam);
}

void Symbols::registerBooleanInputSymbol(const char* name, const bool* pVariable)
{
  XABSL_DEBUG_INIT(errorHandler.message("registering boolean input symbol \"%s\"",name));
  
  if (booleanInputSymbols.exists(name))
  {
    errorHandler.error("registerBooleanInputSymbol(): symbol \"%s\" was already registered",name);
    return;
  }
  booleanInputSymbols.append(name,new BooleanInputSymbol(name, pVariable, errorHandler, booleanInputSymbols.getSize()));
}


void Symbols::registerBooleanInputSymbol(const char* name,
                                               bool (*pFunction)())
{
  XABSL_DEBUG_INIT(errorHandler.message("registering boolean input symbol \"%s\"",name));
  
  if (booleanInputSymbols.exists(name))
  {
    errorHandler.error("registerBooleanInputSymbol(): symbol \"%s\" was already registered",name);
    return;
  }
  booleanInputSymbols.append(name,new BooleanInputSymbol(name, pFunction, errorHandler, booleanInputSymbols.getSize()));
}

void Symbols::registerBooleanInputSymbolParametersChanged(const char* name,
    void (*pFunction)())
{
  XABSL_DEBUG_INIT(errorHandler.message("registering parameter change notification for boolean input symbol\"%s\"",name));

  if (!booleanInputSymbols.exists(name))
  {
    errorHandler.error("registerBooleanInputSymbolParametersChanged(): symbol \"%s\" was not registered",name);
    return;
  }

  booleanInputSymbols[name]->pParametersChanged = pFunction;
}

void Symbols::registerBooleanInputSymbolDecimalParameter(const char* symbolName, 
    const char* name, double* pParam)
{
  XABSL_DEBUG_INIT(errorHandler.message("registering decimal parameter \"%s\" for boolean input symbol\"%s\"",name, symbolName));

  if (!booleanInputSymbols.exists(symbolName))
  {
    errorHandler.error("registerBooleanInputSymbolDecimalParameter(): symbol \"%s\" was not registered",symbolName);
    return;
  }
  if (booleanInputSymbols[symbolName]->parameters.decimal.exists(name))
  {
    errorHandler.error("registerBooleanInputSymbolDecimalParameter(): parameter \"%s\" was already registered",name);
    return;
  }

  booleanInputSymbols[symbolName]->parameters.decimal.append(name,pParam);
}

void Symbols::registerBooleanInputSymbolBooleanParameter(const char* symbolName, 
    const char* name, bool* pParam)
{
  XABSL_DEBUG_INIT(errorHandler.message("registering boolean parameter \"%s\" for boolean input symbol\"%s\"",name, symbolName));

  if (!booleanInputSymbols.exists(symbolName))
  {
    errorHandler.error("registerBooleanInputSymbolBooleanParameter(): symbol \"%s\" was not registered",symbolName);
    return;
  }
  if (booleanInputSymbols[symbolName]->parameters.boolean.exists(name))
  {
    errorHandler.error("registerBooleanInputSymbolBooleanParameter(): parameter \"%s\" was already registered",name);
    return;
  }

  booleanInputSymbols[symbolName]->parameters.boolean.append(name,pParam);
}

void Symbols::registerBooleanInputSymbolEnumeratedParameter(const char* symbolName, 
    const char* name, const char* enumName, int* pParam)
{
  XABSL_DEBUG_INIT(errorHandler.message("registering enumerated parameter \"%s\" for boolean input symbol\"%s\"",name, symbolName));

  if (!booleanInputSymbols.exists(symbolName))
  {
    errorHandler.error("registerBooleanInputSymbolEnumeratedParameter(): symbol \"%s\" was not registered",symbolName);
    return;
  }
  if (booleanInputSymbols[symbolName]->parameters.enumerated.exists(name))
  {
    errorHandler.error("registerBooleanInputSymbolEnumeratedParameter(): parameter \"%s\" was already registered",name);
    return;
  }
  if (!enumerations.exists(enumName))
  {
    enumerations.append(enumName, new Enumeration(enumName, enumerations.getSize()));
  }
  booleanInputSymbols[symbolName]->parameters.enumerations.append(name,enumerations.getElement(enumName));
  booleanInputSymbols[symbolName]->parameters.enumerated.append(name,pParam);
}

void Symbols::registerEnumeratedInputSymbol(const char* name, const char* enumName, const int* pVariable)
{
  XABSL_DEBUG_INIT(errorHandler.message("registering enumerated input symbol \"%s\"",name));
  
  if (enumeratedInputSymbols.exists(name))
  {
    errorHandler.error("registerEnumeratedInputSymbol(): symbol \"%s\" was already registered",name);
    return;
  }
  if (!enumerations.exists(enumName))
  {
    enumerations.append(enumName, new Enumeration(enumName, enumerations.getSize()));
  }
  enumeratedInputSymbols.append(name,new EnumeratedInputSymbol(name, enumerations[enumName], pVariable, errorHandler, enumeratedInputSymbols.getSize()));
}

void Symbols::registerEnumeratedInputSymbol(const char* name, const char* enumName,
    int (*pFunction)())
{
  XABSL_DEBUG_INIT(errorHandler.message("registering enumerated input symbol \"%s\"",name));
  
  if (enumeratedInputSymbols.exists(name))
  {
    errorHandler.error("registerEnumeratedInputSymbol(): symbol \"%s\" was already registered",name);
    return;
  }
  if (!enumerations.exists(enumName))
  {
    enumerations.append(enumName, new Enumeration(enumName, enumerations.getSize()));
  }
  enumeratedInputSymbols.append(name,new EnumeratedInputSymbol(name, enumerations[enumName], pFunction, errorHandler, enumeratedInputSymbols.getSize()));
}

void Symbols::registerEnumeratedInputSymbolParametersChanged(const char* name,
    void (*pFunction)())
{
  XABSL_DEBUG_INIT(errorHandler.message("registering parameter change notification for enumerated input symbol\"%s\"",name));

  if (!enumeratedInputSymbols.exists(name))
  {
    errorHandler.error("registerEnumeratedInputSymbolParametersChanged(): symbol \"%s\" was not registered",name);
    return;
  }

  enumeratedInputSymbols[name]->pParametersChanged = pFunction;
}

void Symbols::registerEnumeratedInputSymbolDecimalParameter(const char* symbolName, 
    const char* name, double* pParam)
{
  XABSL_DEBUG_INIT(errorHandler.message("registering decimal parameter \"%s\" for enumerated input symbol\"%s\"",name, symbolName));

  if (!enumeratedInputSymbols.exists(symbolName))
  {
    errorHandler.error("registerEnumeratedInputSymbolDecimalParameter(): symbol \"%s\" was not registered",symbolName);
    return;
  }
  if (enumeratedInputSymbols[symbolName]->parameters.decimal.exists(name))
  {
    errorHandler.error("registerEnumeratedInputSymbolDecimalParameter(): parameter \"%s\" was already registered",name);
    return;
  }

  enumeratedInputSymbols[symbolName]->parameters.decimal.append(name,pParam);
}

void Symbols::registerEnumeratedInputSymbolBooleanParameter(const char* symbolName, 
    const char* name, bool* pParam)
{
  XABSL_DEBUG_INIT(errorHandler.message("registering boolean parameter \"%s\" for enumerated input symbol\"%s\"",name, symbolName));

  if (!enumeratedInputSymbols.exists(symbolName))
  {
    errorHandler.error("registerEnumeratedInputSymbolBooleanParameter(): symbol \"%s\" was not registered",symbolName);
    return;
  }
  if (enumeratedInputSymbols[symbolName]->parameters.boolean.exists(name))
  {
    errorHandler.error("registerEnumeratedInputSymbolBooleanParameter(): parameter \"%s\" was already registered",name);
    return;
  }

  enumeratedInputSymbols[symbolName]->parameters.boolean.append(name,pParam);
}

void Symbols::registerEnumeratedInputSymbolEnumeratedParameter(const char* symbolName, 
    const char* name, const char* enumName, int* pParam)
{
  XABSL_DEBUG_INIT(errorHandler.message("registering enumerated parameter \"%s\" for enumerated input symbol\"%s\"",name, symbolName));

  if (!enumeratedInputSymbols.exists(symbolName))
  {
    errorHandler.error("registerEnumeratedInputSymbolEnumeratedParameter(): symbol \"%s\" was not registered",symbolName);
    return;
  }
  if (enumeratedInputSymbols[symbolName]->parameters.enumerated.exists(name))
  {
    errorHandler.error("registerEnumeratedInputSymbolEnumeratedParameter(): parameter \"%s\" was already registered",name);
    return;
  }
  if (!enumerations.exists(enumName))
  {
    enumerations.append(enumName, new Enumeration(enumName, enumerations.getSize()));
  }
  enumeratedInputSymbols[symbolName]->parameters.enumerations.append(name,enumerations.getElement(enumName));
  enumeratedInputSymbols[symbolName]->parameters.enumerated.append(name,pParam);
}

void Symbols::registerDecimalOutputSymbol(const char* name, double* pVariable)
{
  XABSL_DEBUG_INIT(errorHandler.message("registering decimal output symbol \"%s\"",name));
  
  if (decimalOutputSymbols.exists(name))
  {
    errorHandler.error("registerDcimalOutputSymbol(): symbol \"%s\" was already registered",name);
    return;
  }
  decimalOutputSymbols.append(name,new DecimalOutputSymbol(name, pVariable, decimalOutputSymbols.getSize()));
}

void Symbols::registerDecimalOutputSymbol(const char* name, 
  void (*pSetFunction)(double),
  double (*pGetFunction)()
)
{
  XABSL_DEBUG_INIT(errorHandler.message("registering decimal output symbol \"%s\"",name));
  
  if (decimalOutputSymbols.exists(name))
  {
    errorHandler.error("registerDecimalOutputSymbol(): symbol \"%s\" was already registered",name);
    return;
  }
  decimalOutputSymbols.append(name,new DecimalOutputSymbol(name, pSetFunction, pGetFunction, decimalOutputSymbols.getSize()));
}

void Symbols::registerBooleanOutputSymbol(const char* name, bool* pVariable)
{
  XABSL_DEBUG_INIT(errorHandler.message("registering boolean output symbol \"%s\"",name));
  
  if (booleanOutputSymbols.exists(name))
  {
    errorHandler.error("registerBooleanOutputSymbol(): symbol \"%s\" was already registered",name);
    return;
  }
  booleanOutputSymbols.append(name,new BooleanOutputSymbol(name, pVariable, booleanOutputSymbols.getSize()));
}

void Symbols::registerBooleanOutputSymbol(const char* name,
  void (*pSetFunction)(bool),
  bool (*pGetFunction)()
)
{
  XABSL_DEBUG_INIT(errorHandler.message("registering boolean output symbol \"%s\"",name));
  
  if (booleanOutputSymbols.exists(name))
  {
    errorHandler.error("registerBooleanOutputSymbol(): symbol \"%s\" was already registered",name);
    return;
  }
  booleanOutputSymbols.append(name,new BooleanOutputSymbol(name, pSetFunction, pGetFunction, booleanOutputSymbols.getSize()));
}

void Symbols::registerEnumeratedOutputSymbol(const char* name, const char* enumName, int* pVariable)
{
  XABSL_DEBUG_INIT(errorHandler.message("registering enumerated output symbol \"%s\"",name));
  
  if (enumeratedOutputSymbols.exists(name))
  {
    errorHandler.error("registerEnumeratedOutputSymbol(): symbol \"%s\" was already registered",name);
    return;
  }
  if (!enumerations.exists(enumName))
  {
    enumerations.append(enumName, new Enumeration(enumName, enumerations.getSize()));
  }
  enumeratedOutputSymbols.append(name,new EnumeratedOutputSymbol(name, enumerations[enumName], pVariable, enumeratedOutputSymbols.getSize()));
}

void Symbols::registerEnumeratedOutputSymbol(const char* name, const char* enumName,
  void (*pSetFunction)(int),
  int (*pGetFunction)()
)
{
  XABSL_DEBUG_INIT(errorHandler.message("registering enumerated output symbol \"%s\"",name));
  
  if (enumeratedOutputSymbols.exists(name))
  {
    errorHandler.error("registerEnumeratedOutputSymbol(): symbol \"%s\" was already registered",name);
    return;
  }
  if (!enumerations.exists(enumName))
  {
    enumerations.append(enumName, new Enumeration(enumName, enumerations.getSize()));
  }
  enumeratedOutputSymbols.append(name,new EnumeratedOutputSymbol(name, enumerations[enumName], pSetFunction, pGetFunction, enumeratedOutputSymbols.getSize()));
}

void Symbols::resetOutputSymbols()
{
  for (int i=0;i<decimalOutputSymbols.getSize();i++)
    decimalOutputSymbols[i]->activeValueWasSet = false;
  for (int i=0;i<booleanOutputSymbols.getSize();i++)
    booleanOutputSymbols[i]->activeValueWasSet = false;
  for (int i=0;i<enumeratedOutputSymbols.getSize();i++)
    enumeratedOutputSymbols[i]->activeValueWasSet = false;
}

} // namespace

