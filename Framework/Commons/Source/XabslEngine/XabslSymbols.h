/**
* @file XabslSymbols.h
*
* Definition of class Symbols and helper classes
*
* @author <a href="http://www.martin-loetzsch.de">Martin Loetzsch</a>
* @author <a href="http://www.sim.informatik.tu-darmstadt.de/pers/card/risler.html">Max Risler</a>
*/

#ifndef __XabslSymbols_h_
#define __XabslSymbols_h_

#include "XabslTools.h"
#include "XabslParameters.h"

namespace xabsl 
{

/**
* @class EnumElement
* Represents an enum element that is part of an enumerated input or output symbol.
* @author <a href="http://www.martin-loetzsch.de">Martin Loetzsch</a>
*/
class EnumElement : public NamedItem
{
public:
/** 
* Constructor 
* @param name The name of the enum element as specified in the XML formalization
* @param value The value for the element from the software environment
  */
  EnumElement(const char* name, int value)
    : NamedItem(name), v(value) {};
  
  /** The enum value from a function or variable in the software environment */
  int v;
};
/**
* @class Enumeration
*
* Represents a list of enum elements
*/
class Enumeration : public NamedItem
{
public:
  /** 
  * Constructor 
  * @param name The name of the enumeration as specified in the XML formalization
  * @param index Index of the enumeration in array enumerations in corresponding engine
  */
  Enumeration(const char* name, int index) : NamedItem(name), index(index) {};

  /** Destructor. Deletes the enum elements */
  ~Enumeration();

  /** 
  * Assigns an enum value from a function or variable in the software environment 
  * to the enum-element string in the XML formalization.
  */
  NamedArray<EnumElement*> enumElements;

  /** Index of the enumeration in array enumerations in corresponding engine */
  int index;
};

/** 
* A Template for the input symbol classes
* @author <a href="http://www.martin-loetzsch.de">Martin Loetzsch</a>
*/
template<class T> class InputSymbol : public NamedItem
{
public:
  /** 
  * Constructor 
  * @param name The name of the symbol, for debugging purposes
  * @param pVariable A pointer to the variable that the symbol stands for
  * @param index Index of the symbol in array in corresponding engine
  */
  InputSymbol(const char* name, const T* pVariable, ErrorHandler& errorHandler, int index)
    : NamedItem(name), parameters(errorHandler), pParametersChanged(0), index(index), lastValue(0), pV(pVariable), pF(0)
  {};
  
  
  /** Constructor 
  * @param name The name of the symbol, for debugging purposes
  * @param pFunction A pointer to a boolean returning function in the software environment 
  * @param index Index of the symbol in array in corresponding engine
  */
  InputSymbol(const char* name,
    T (*pFunction)(),
    ErrorHandler& errorHandler, int index)
    : NamedItem(name), parameters(errorHandler),  pParametersChanged(0), index(index), lastValue(0), pV(0), pF(pFunction) {};
  
  /** returns the value of the symbol */
  T getValue() 
  { if (pF!=0) return lastValue = (*pF)(); else return lastValue = *pV; }

  /** Notify the software environment about a parameter change */
  void parametersChanged() const
  { if (pParametersChanged!=0) (*pParametersChanged)(); }
  
  /** The parameters of the input symbol*/
  Parameters parameters;

  /** A Pointer to a parameter change notification function in the software environment */
  void (*pParametersChanged)();
  
  /** Index of the symbol in array in corresponding engine */
  int index;

  /** Last queried value of the input symbol */
  T lastValue;

private:
  /** A pointer to a variable in the software environment */
  const T* pV; 
  
  /** A pointer to a T returning function in the software environment */
  T (*pF)(); 
};

/** 
* @class DecimalInputSymbol
*
* Represents a decimal input symbol of the Engine 
*
* @author <a href="http://www.martin-loetzsch.de">Martin Loetzsch</a>
*/
class DecimalInputSymbol : public InputSymbol<double>
{
public:
  /** 
  * Constructor 
  * @param name The name of the symbol, for debugging purposes
  * @param pVariable A pointer to the variable that the symbol stands for
  * @param index Index of the symbol in array in corresponding engine
  */
  DecimalInputSymbol(const char* name, const double* pVariable, ErrorHandler& errorHandler, int index)
    : InputSymbol<double>(name, pVariable, errorHandler, index) 
  {};
  
  /** Constructor 
  * @param name The name of the symbol, for debugging purposes
  * @param pFunction A pointer to a double returning function in the software environment 
  * @param index Index of the symbol in array in corresponding engine
  */
  DecimalInputSymbol(const char* name,
    double (*pFunction)(),
    ErrorHandler& errorHandler, int index)
    : InputSymbol<double>(name, pFunction, errorHandler, index) {};
};

/** 
* @class BooleanInputSymbol
*
* Represents a boolean input symbol of the Engine 
*
* @author <a href="http://www.martin-loetzsch.de">Martin Loetzsch</a>
*/
class BooleanInputSymbol : public InputSymbol<bool>
{
public:
  /** 
  * Constructor 
  * @param name The name of the symbol, for debugging purposes
  * @param pVariable A pointer to the variable that the symbol stands for
  * @param index Index of the symbol in array in corresponding engine
  */
  BooleanInputSymbol(const char* name, const bool* pVariable, ErrorHandler& errorHandler, int index)
    : InputSymbol<bool>(name, pVariable, errorHandler, index) 
  {};
  
  /** Constructor 
  * @param name The name of the symbol, for debugging purposes
  * @param pFunction A pointer to a boolean returning function in the software environment 
  * @param index Index of the symbol in array in corresponding engine
  */
  BooleanInputSymbol(const char* name, 
    bool (*pFunction)(),
    ErrorHandler& errorHandler, int index)
    : InputSymbol<bool>(name, pFunction, errorHandler, index) {};
};

/** 
* @class EnumeratedInputSymbol
*
* Represents a enumerated input symbol of the Engine 
*
* @author <a href="http://www.martin-loetzsch.de">Martin Loetzsch</a>
*/
class EnumeratedInputSymbol : public InputSymbol<int>
{
public:
  /** 
  * Constructor 
  * @param name The name of the symbol, for debugging purposes
  * @param pVariable A pointer to the variable that the symbol stands for
  * @param index Index of the symbol in array in corresponding engine
  */
  EnumeratedInputSymbol(const char* name, Enumeration* enumeration, const int* pVariable, 
    ErrorHandler& errorHandler, int index)
    : InputSymbol<int>(name, pVariable, errorHandler, index), enumeration(enumeration) 
  {};
  
  /** Constructor 
  * @param name The name of the symbol, for debugging purposes
  * @param enumeration Pointer to the list of enumeration elements
  * @param pFunction A pointer to an int returning function in the software environment 
  * @param index Index of the symbol in array in corresponding engine
  */
  EnumeratedInputSymbol(const char* name, Enumeration* enumeration, 
    int (*pFunction)(),
    ErrorHandler& errorHandler, int index)
    : InputSymbol<int>(name, pFunction, errorHandler, index), enumeration(enumeration) {};

  /** Pointer to the list of enumeration elements */
  Enumeration* enumeration;
};

/** 
* @class OutputSymbol
*
* A Template for the output symbol classes
*
* @author <a href="http://www.sim.informatik.tu-darmstadt.de/pers/card/risler.html">Max Risler</a>
*/
template<class T> class OutputSymbol : public NamedItem
{
public:
  /** 
  * Constructor 
  * @param name The name of the symbol, for debugging purposes
  * @param pVariable A pointer to the variable that the symbol stands for
  * @param defaultValue The default value
  * @param index Index of the symbol in array in corresponding engine
  */
  OutputSymbol(const char* name, T* pVariable, int index)
    : NamedItem(name), activeValueWasSet(false), index(index), lastValue(0), pV(pVariable), pSetF(0), pGetF(0)
  {};
  
  
  /** Constructor 
  * @param name The name of the symbol, for debugging purposes
  * @param pSetFunction A pointer to a boolean accepting function in the software environment 
  * @param pGetFunction A pointer to a boolean returning function in the software environment 
  * @param defaultValue The default value
  * @param index Index of the symbol in array in corresponding engine
  */
  OutputSymbol(const char* name,
    void (*pSetFunction)(T),
    T (*pGetFunction)(), int index)
    : NamedItem(name), activeValueWasSet(false), index(index), lastValue(0), pV(0), pSetF(pSetFunction), pGetF(pGetFunction)
  {};


  /** Set the value of the symbol. */
  void setValue(T value)
  {
    if (pSetF!=0) 
    {
      (*pSetF)(value);
    }
    else 
    {
      *pV=value;
    }
    lastValue = value;
    activeValueWasSet = true;
  }

  /** Returns the current value of the symbol. */
  T getValue() const
  {
    if (pGetF!=0) 
    {
      return (*pGetF)();
    }
    else 
    {
      return *pV;
    }
  }

  /** If true, the value was set during the last execution of the option graph. */
  bool activeValueWasSet;

  /** Index of the symbol in array in corresponding engine */
  int index;

  /** Last set value of the output symbol */
  T lastValue;

private:
  /** A pointer to a variable in the software environment */
  T* pV; 
  
  /** A pointer to a function that sets the value of the symbol in the software environment */
  void (*pSetF)(T); 

  /** A pointer to a function that gets the value of the symbol from the software environment */
  T (*pGetF)(); 
};

/** 
* @class DecimalOutputSymbol
*
* Represents a decimal output symbol of the Engine 
*
* @author <a href="http://www.sim.informatik.tu-darmstadt.de/pers/card/risler.html">Max Risler</a>
*/
class DecimalOutputSymbol : public OutputSymbol<double>
{
public:
  /** 
  * Constructor 
  * @param name The name of the symbol, for debugging purposes
  * @param pVariable A pointer to the variable that the symbol stands for
  * @param index Index of the symbol in array in corresponding engine
  */
  DecimalOutputSymbol(const char* name, double* pVariable, int index)
    : OutputSymbol<double>(name, pVariable, index)
  {};
    
  /** Constructor 
  * @param name The name of the symbol, for debugging purposes
  * @param pSetFunction A pointer to a function in the software environment that sets the value of the symbol
  * @param pGetFunction A pointer to a function in the software environment that returns the value of the symbol
  * @param index Index of the symbol in array in corresponding engine
  */
  DecimalOutputSymbol(const char* name, 
    void (*pSetFunction)(double),
    double (*pGetFunction)(), int index)
    : OutputSymbol<double>(name, pSetFunction, pGetFunction, index)
  {};
};

/** 
* @class BooleanOutputSymbol
*
* Represents a boolean output symbol of the Engine 
*
* @author <a href="http://www.sim.informatik.tu-darmstadt.de/pers/card/risler.html">Max Risler</a>
*
*/
class BooleanOutputSymbol : public OutputSymbol<bool>
{
public:
  /** 
  * Constructor 
  * @param name The name of the symbol, for debugging purposes
  * @param pVariable A pointer to the variable that the symbol stands for
  * @param index Index of the symbol in array in corresponding engine
  */
  BooleanOutputSymbol(const char* name, bool* pVariable, int index)
    : OutputSymbol<bool>(name, pVariable, index)
  {};
    
  /** Constructor 
  * @param name The name of the symbol, for debugging purposes
  * @param pSetFunction A pointer to a function in the software environment that sets the value of the symbol
  * @param pGetFunction A pointer to a function in the software environment that returns the value of the symbol
  * @param index Index of the symbol in array in corresponding engine
  */
  BooleanOutputSymbol(const char* name, 
    void (*pSetFunction)(bool),
    bool (*pGetFunction)(), int index)
    : OutputSymbol<bool>(name, pSetFunction, pGetFunction, index)
  {};
};

/** 
* @class EnumeratedOutputSymbol
*
* Represents a enumerated output symbol of the Engine 
*
* @author <a href="http://www.martin-loetzsch.de">Martin Loetzsch</a>
*/
class EnumeratedOutputSymbol : public OutputSymbol<int>
{
public:
  /** 
  * Constructor 
  * @param name The name of the symbol, for debugging purposes
  * @param enumeration Pointer to the list of enumeration elements
  * @param pVariable A pointer to the variable that the symbol stands for
  * @param index Index of the symbol in array in corresponding engine
  */
  EnumeratedOutputSymbol(const char* name, Enumeration* enumeration, int* pVariable, int index)
    : OutputSymbol<int>(name, pVariable, index), enumeration(enumeration)
  {};
  
  
  /** Constructor 
  * @param name The name of the symbol, for debugging purposes
  * @param enumeration Pointer to the list of enumeration elements
  * @param pSetFunction A pointer to a function in the software environment that sets the value of the symbol
  * @param pGetFunction A pointer to a function in the software environment that returns the value of the symbol
  * @param index Index of the symbol in array in corresponding engine
  */
  EnumeratedOutputSymbol(const char* name, Enumeration* enumeration,
    void (*pSetFunction)(int),
    int (*pGetFunction)(), int index)
    : OutputSymbol<int>(name, pSetFunction, pGetFunction, index), enumeration(enumeration)
  {};
  
  /** Pointer to the list of enumeration elements */
  Enumeration* enumeration;
};

/**
* @class Symbols
*
* Handles the symbols of the Engine.
*
* @author <a href="http://www.martin-loetzsch.de">Martin Loetzsch</a>
* @author <a href="http://www.sim.informatik.tu-darmstadt.de/pers/card/risler.html">Max Risler</a>
*/
class Symbols
{
public:
/** 
* Constructor.
* @param errorHandler Is invoked when errors occur
  */
  Symbols(ErrorHandler& errorHandler)
    : errorHandler(errorHandler) {};
  
  /** Destructor */
  virtual ~Symbols();
  
  /**
  * Registers an enum element for an enumeration.
  * @param enumName The name of the enumeration
  * @param name The name of the enum element
  * @param value The value of the element
  */
  void registerEnumElement(const char* enumName, 
    const char* name, int value);

  /** 
  * Registers the address of a variable for a decimal input symbol.
  * @param name The name of the symbol
  * @param pVariable A pointer to a variable in the software environment 
  */
  void registerDecimalInputSymbol(const char* name, const double* pVariable);
  
  /** 
  * Registers the address of a function for a decimal input symbol.
  * @param name The name of the symbol
  * @param pFunction A pointer to a function that calculates a value for the symbol
  */
  void registerDecimalInputSymbol(const char* name,
    double (*pFunction)());
  
  /** 
  * Registers the address of a function for parameter change notification for a decimal input symbol.
  * @param name The name of the symbol
  * @param pFunction A pointer to the parameter change notification function
  */
  void registerDecimalInputSymbolParametersChanged(const char* name,
    void (*pFunction)());

  /** 
  * Registers a parameter of a parameterized decimal input symbol.
  * @param symbolName The name of the symbol
  * @param name The name of the parameter
  * @param pParam A pointer to the parameter
  */
  void registerDecimalInputSymbolDecimalParameter(const char* symbolName, 
    const char* name, double* pParam);
  void registerDecimalInputSymbolBooleanParameter(const char* symbolName, 
    const char* name, bool* pParam);
  void registerDecimalInputSymbolEnumeratedParameter(const char* symbolName, 
    const char* name, const char* enumName, int* pParam);

  /** 
  * Registers the address of a variable for a boolean input symbol.
  * @param name The name of the symbol
  * @param pVariable A pointer to a variable in the software environment 
  */
  void registerBooleanInputSymbol(const char* name, const bool* pVariable);
  
  /** 
  * Registers the address of a function for a boolean input symbol.
  * @param name The name of the symbol
  * @param pFunction A pointer to a function that calculates a value for the symbol
  */
  void registerBooleanInputSymbol(const char* name, 
    bool (*pFunction)());
  
  /** 
  * Registers the address of a function for parameter change notification for a boolean input symbol.
  * @param name The name of the symbol
  * @param pFunction A pointer to the parameter change notification function
  */
  void registerBooleanInputSymbolParametersChanged(const char* name,
    void (*pFunction)());

  /** 
  * Registers a parameter of a parameterized boolean input symbol.
  * @param symbolName The name of the symbol
  * @param name The name of the parameter
  * @param pParam A pointer to the parameter
  */
  void registerBooleanInputSymbolDecimalParameter(const char* symbolName, 
    const char* name, double* pParam);
  void registerBooleanInputSymbolBooleanParameter(const char* symbolName, 
    const char* name, bool* pParam);
  void registerBooleanInputSymbolEnumeratedParameter(const char* symbolName, 
    const char* name, const char* enumName, int* pParam);

  /** 
  * Registers the address of a variable for a enumerated input symbol.
  * @param name The name of the symbol
  * @param enumName The name of the associated enumeration
  * @param pVariable A pointer to a variable in the software environment 
  */
  void registerEnumeratedInputSymbol(const char* name, const char* enumName, const int* pVariable);
  
  /** 
  * Registers the address of a function for a enumerated input symbol.
  * @param name The name of the symbol
  * @param enumName The name of the associated enumeration
  * @param pFunction A pointer to a function that calculates a value for the symbol
  */
  void registerEnumeratedInputSymbol(const char* name, const char* enumName, 
    int (*pFunction)());
  
  /** 
  * Registers the address of a function for parameter change notification for an enumerated input symbol.
  * @param name The name of the symbol
  * @param pFunction A pointer to the parameter change notification function
  */
  void registerEnumeratedInputSymbolParametersChanged(const char* name,
    void (*pFunction)());

  /** 
  * Registers a parameter of an enumerated input symbol.
  * @param symbolName The name of the symbol
  * @param name The name of the parameter
  * @param pParam A pointer to the parameter
  */
  void registerEnumeratedInputSymbolDecimalParameter(const char* symbolName, 
    const char* name, double* pParam);
  void registerEnumeratedInputSymbolBooleanParameter(const char* symbolName, 
    const char* name, bool* pParam);
  void registerEnumeratedInputSymbolEnumeratedParameter(const char* symbolName, 
    const char* name, const char* enumName, int* pParam);

  /** 
  * Registers the address of a variable for a decimal output symbol.
  * @param name The name of the symbol
  * @param pVariable A pointer to a variable in the software environment 
  */
  void registerDecimalOutputSymbol(const char* name, double* pVariable);

  /** 
  * Registers the address of a function for a decimal output symbol.
  * @param name The name of the symbol
  * @param pSetFunction A pointer to a function that sets a value for the symbol
  * @param pGetFunction A pointer to a function that returns a value for the symbol
  */
  void registerDecimalOutputSymbol(const char* name,
    void (*pSetFunction)(double),
    double (*pGetFunction)()
  );

  /** 
  * Registers the address of a variable for a boolean output symbol.
  * @param name The name of the symbol
  * @param pVariable A pointer to a variable in the software environment 
  */
  void registerBooleanOutputSymbol(const char* name, bool* pVariable);

  /** 
  * Registers the address of a function for a boolean output symbol.
  * @param name The name of the symbol
  * @param pSetFunction A pointer to a function that sets a value for the symbol
  * @param pGetFunction A pointer to a function that returns a value for the symbol
  */
  void registerBooleanOutputSymbol(const char* name, 
    void (*pSetFunction)(bool),
    bool (*pGetFunction)()
  );

  /** 
  * Registers the address of a variable for a enumerated output symbol.
  * @param name The name of the symbol
  * @param enumName The name of the associated enumeration
  * @param pVariable A pointer to a variable in the software environment 
  */
  void registerEnumeratedOutputSymbol(const char* name, const char* enumName, int* pVariable);
  
  /** 
  * Registers the address of a function for a enumerated output symbol.
  * @param name The name of the symbol
  * @param enumName The name of the associated enumeration
  * @param pSetFunction A pointer to a function that sets a value for the symbol
  * @param pGetFunction A pointer to a function that returns a value for the symbol
  */
  void registerEnumeratedOutputSymbol(const char* name, const char* enumName, 
    void (*pSetFunction)(int),
    int (*pGetFunction)()
  );
  
  /** Sets all output symbols to unset */
  void resetOutputSymbols();
  
  /** The enumerations */
  NamedArray<Enumeration*> enumerations;

  /** The decimal input symbols */
  NamedArray<DecimalInputSymbol*> decimalInputSymbols;
    
  /** The boolean input symbols */
  NamedArray<BooleanInputSymbol*> booleanInputSymbols;
  
  /** The enumerated input symbols */
  NamedArray<EnumeratedInputSymbol*> enumeratedInputSymbols;
  
  /** The decimal output symbols */
  NamedArray<DecimalOutputSymbol*> decimalOutputSymbols;

  /** The boolean output symbols */
  NamedArray<BooleanOutputSymbol*> booleanOutputSymbols;

  /** The enumerated output symbols */
  NamedArray<EnumeratedOutputSymbol*> enumeratedOutputSymbols;

private:
  /** Is invoked when errors occur */
  ErrorHandler& errorHandler;
};

} // namespace

#endif //__XabslSymbols_h_
