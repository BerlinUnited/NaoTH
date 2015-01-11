/** 
* @file XabslEnumeratedExpression.h
*
* Definition of EnumeratedExpression and derivates
*
* @author <a href="http://www.sim.informatik.tu-darmstadt.de/pers/card/risler.html">Max Risler</a>
*/

#ifndef __XabslEnumeratedExpression_h_
#define __XabslEnumeratedExpression_h_

#include "XabslSymbols.h"
#include "XabslDecimalExpression.h"

namespace xabsl 
{

/** 
* @class EnumeratedExpression
* 
* Base class for all enumerated expressions inside an option graph.
*
* @author <a href="http://www.sim.informatik.tu-darmstadt.de/pers/card/risler.html">Max Risler</a>
*/
class EnumeratedExpression
{
public:
  /** Evaluates the enumerated expression. */
  virtual int getValue() const = 0;
  
  /**
  * Creates an enumerated expression depending on the input.
  * @param enumeration A reference to the enumeration which is the domain of this expression
  * @param input An input source for the intermediate code. It must be opened and read until 
  *              A position where a enumerated expression starts.
  * @param errorHandler A reference to a ErrorHandler instance
  * @param symbols All available symbols
  * @param option The current option
  * @param state The current state
  */
  static EnumeratedExpression* create(const Enumeration* enumeration,
    InputSource& input, 
    ErrorHandler& errorHandler,
    Symbols& symbols,
    Option& option,
    State& state);
  
  /** 
  * Creates an enumerated expression depending on the input. 
  * Uses the create() function to create enumerated operands.
  * @param operand The expression to be created
  * @param enumeration A reference to the enumeration which is the domain of this expression
  * @param input An input source for the intermediate code. It must be opened and read until 
  *              A position where a decimal operand starts.
  * @param errorHandler A reference to a ErrorHandler instance
  * @param symbols All available symbols
  * @param option The current option
  * @param state The current state
  * @return If the creation was successful
  */
  static bool createOperand(
    const EnumeratedExpression*& operand,
    const Enumeration* enumeration,
    InputSource& input, 
    ErrorHandler& errorHandler,
    Symbols& symbols,
    Option& option,
    State& state);

  /** Destructor */
  virtual ~EnumeratedExpression() = 0;

  const Enumeration* enumeration;
};

/** 
* @class EnumeratedValue
* 
* Represents a enumerated value.
*
* @author <a href="http://www.sim.informatik.tu-darmstadt.de/pers/card/risler.html">Max Risler</a>
*/
class EnumeratedValue : public EnumeratedExpression
{
public:
  /**
  * Constructor. Creates the value
  * @param enumeration A reference to the enumeration which is the domain of this expression
  * @param input An input source for the intermediate code. It must be opened and read until 
  *              A position where a value starts.
  * @param errorHandler A reference to a ErrorHandler instance
  */
  EnumeratedValue(const Enumeration* enumeration,
    InputSource& input, 
    ErrorHandler& errorHandler);
  
  /**
  * Constructor. Creates an expression for a fixed enumerated value
  * @param value The enumerated value
  */
  EnumeratedValue(const Enumeration* enumeration, int value) : 
    value(value) 
  {
    this->enumeration = enumeration;
  }

  /** Calculates the value of the decimal expression. */
  virtual int getValue() const;
  
private:
  /** The value */
  int value;
};

/** 
* @class EnumeratedOptionParameterRef
* 
* Represents a reference to a enumerated option parameter.
*
* @author <a href="http://www.sim.informatik.tu-darmstadt.de/pers/card/risler.html">Max Risler</a>
*/
class EnumeratedOptionParameterRef : public EnumeratedExpression
{
public:
  /**
  * Constructor. Creates the reference 
  * @param enumeration A reference to the enumeration which is the domain of this expression
  * @param input An input source for the intermediate code. It must be opened and read until 
  *              A position where the expression starts.
  * @param errorHandler A reference to a ErrorHandler instance
  * @param option The current option
  */
  EnumeratedOptionParameterRef(const Enumeration* enumeration,
    InputSource& input, 
    ErrorHandler& errorHandler,
    Option& option);
  
  /** Calculates the value of the enumerated expression. */
  virtual int getValue() const;
  
private:
  /** A pointer to the parameter */
  int* parameter;
};

/** 
* @class EnumeratedInputSymbolRef
*
* Represents an 'enumerated-input-symbol-ref' element of the option graph 
*
* @author <a href="http://www.sim.informatik.tu-darmstadt.de/pers/card/risler.html">Max Risler</a>
* @author <a href="http://www.martin-loetzsch.de">Martin Loetzsch</a>
*/
class EnumeratedInputSymbolRef : public EnumeratedExpression
{
public:
  /**
  * Constructor. Creates the element
  * @param enumeration A reference to the enumeration which is the domain of this expression
  * @param input An input source for the intermediate code. It must be opened and read until 
  *              A position where a expression starts.
  * @param errorHandler A reference to a ErrorHandler instance
  * @param symbols All available symbols
  * @param option The current option
  * @param state The current state
  */
  EnumeratedInputSymbolRef(const Enumeration* enumeration,
    InputSource& input, 
    ErrorHandler& errorHandler,
    Symbols& symbols,
    Option& option,
    State& state);
  
  /** Destructor */
  ~EnumeratedInputSymbolRef();

  /** Evaluates the enumerated expression. */
  virtual int getValue() const;
  
private:
  /** The referenced symbol */
  EnumeratedInputSymbol* symbol;

  /** The parameter assignments of the referenced symbol */
  ParameterAssignment* parameters;
};

/** 
* @class EnumeratedOutputSymbolRef
* 
* Represents a reference to a enumerated input symbol.
*
* @author <a href="http://www.martin-loetzsch.de">Martin Loetzsch</a>
* @author <a href="http://www.sim.informatik.tu-darmstadt.de/pers/card/risler.html">Max Risler</a>
*/
class EnumeratedOutputSymbolRef : public EnumeratedExpression
{
public:
  /** Calculates the value of the enumerated expression. */
  virtual int getValue() const;
  
  /**
  * Constructor. Creates the function call depending on the input.
  * @param enumeration A reference to the enumeration which is the domain of this expression
  * @param input An input source for the intermediate code. It must be opened and read until 
  *              A position where the function reference starts.
  * @param errorHandler A reference to a ErrorHandler instance
  * @param symbols All available symbols
  */
  EnumeratedOutputSymbolRef(const Enumeration* enumeration,
    InputSource& input, 
    ErrorHandler& errorHandler,
    Symbols& symbols);

private:
  /** The referenced symbol */
  const EnumeratedOutputSymbol* symbol;
};

/** 
* @class ConditionalEnumeratedExpression
* 
* Represents an ANSI C (condition?expression:expression) question mark operator
*
* @author <a href="http://www.martin-loetzsch.de">Martin Loetzsch</a>
* @author <a href="http://www.sim.informatik.tu-darmstadt.de/pers/card/risler.html">Max Risler</a>
*/
class ConditionalEnumeratedExpression : public EnumeratedExpression
{
public:
  /**
  * Constructor. Creates the expression
  * @param input An input source for the intermediate code. It must be opened and read until 
  *              A position where a expression starts.
  * @param errorHandler A reference to a ErrorHandler instance
  * @param symbols All available symbols
  * @param option The current option
  * @param state The current state
  */
  ConditionalEnumeratedExpression(const Enumeration* enumeration,
    InputSource& input, 
    ErrorHandler& errorHandler,
    Symbols& symbols,
    Option& option,
    State& state);  

  /** Destructor */
  ~ConditionalEnumeratedExpression();

  /** Calculates the value of the decimal expression. */
  virtual int getValue() const;

private:
  /** The condition */
  const BooleanExpression* condition;

  /** The expression that is returned when the condition evaluates true */
  const EnumeratedExpression* expression1;

  /** The expression that is returned when the condition evaluates false */
  const EnumeratedExpression* expression2;
};

} // namespace

#endif //__XabslEnumeratedExpression_h_
