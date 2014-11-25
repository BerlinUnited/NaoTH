/** 
* @file XabslDecimalExpression.h
*
* Definition of DecimalExpression and derivates
* 
* @author <a href="http://www.martin-loetzsch.de">Martin Loetzsch</a>
* @author <a href="http://www.sim.informatik.tu-darmstadt.de/pers/card/risler.html">Max Risler</a>
* @author <a href="http://www.informatik.hu-berlin.de/~juengel">Matthias JŸngel</a>
*/

#ifndef __XabslDecimalExpression_h_
#define __XabslDecimalExpression_h_

#include "XabslSymbols.h"

namespace xabsl 
{

// class prototypes used by symbol parameters / conditional expressions
class BooleanExpression;
class EnumeratedExpression;
class Option;
class State;

/** 
* @class DecimalExpression
* 
* Base class for all decimal expressions inside an option graph.
*
* @author <a href="http://www.martin-loetzsch.de">Martin Loetzsch</a>
* @author <a href="http://www.sim.informatik.tu-darmstadt.de/pers/card/risler.html">Max Risler</a>
*/
class DecimalExpression
{
public:
  /** Calculates the value of the decimal expression. */
  virtual double getValue() const = 0;
  
  /**
  * Creates a decimal expression depending on the input.
  * @param input An input source for the intermediate code. It must be opened and read until 
  *              A position where a decimal expression starts.
  * @param errorHandler A reference to a ErrorHandler instance
  * @param symbols All available symbols
  * @param option The current option
  * @param state The current state
  */
  static DecimalExpression* create(InputSource& input, 
    ErrorHandler& errorHandler,
    Symbols& symbols,
    Option& option,
    State& state);

  /** Destructor */
  virtual ~DecimalExpression() = 0;
  
  /** 
  * Creates a decimal expression depending on the input. 
  * Uses the create() function to create decimal operands.
  * @param operand The expression to be created
  * @param input An input source for the intermediate code. It must be opened and read until 
  *              A position where a decimal operand starts.
  * @param errorHandler A reference to a ErrorHandler instance
  * @param symbols All available symbols
  * @param option The current option
  * @param state The current state
  * @return If the creation was successful
  */
  static bool createOperand(
    DecimalExpression*& operand,
    InputSource& input, 
    ErrorHandler& errorHandler,
    Symbols& symbols,
    Option& option,
    State& state);
};

/** 
* @class DecimalValue
* 
* Represents a decimal value.
*
* @author <a href="http://www.martin-loetzsch.de">Martin Loetzsch</a>
*/
class DecimalValue : public DecimalExpression
{
public:
  /**
  * Constructor. Creates the value
  * @param input An input source for the intermediate code. It must be opened and read until 
  *              A position where a value starts.
  * @param errorHandler A reference to a ErrorHandler instance
  */
  DecimalValue(InputSource& input, 
    ErrorHandler& errorHandler);

  /**
  * Constructor. Creates an expression for a fixed decimal value
  * @param value The decimal value
  */
  DecimalValue(double value) : value(value) {}
  
  /** Calculates the value of the decimal expression. */
  virtual double getValue() const;
  
private:
  /** The value */
  double value;
};

/** 
* @class DecimalOptionParameterRef
* 
* Represents a reference to a decimal option parameter.
*
* @author <a href="http://www.martin-loetzsch.de">Martin Loetzsch</a>
* @author <a href="http://www.sim.informatik.tu-darmstadt.de/pers/card/risler.html">Max Risler</a>
*/
class DecimalOptionParameterRef : public DecimalExpression
{
public:
  /**
  * Constructor. Creates the reference 
  * @param input An input source for the intermediate code. It must be opened and read until 
  *              A position where the expression starts.
  * @param errorHandler A reference to a ErrorHandler instance
  * @param option The current option
  */
  DecimalOptionParameterRef(InputSource& input, 
    ErrorHandler& errorHandler,
    Option& option);
  
  /** Calculates the value of the decimal expression. */
  virtual double getValue() const;
  
private:
  /** A pointer to the parameter */
  double* parameter;
};

/** 
* @class ArithmeticOperator
* 
* Base class for the +, -, *, / and % operator.
*
* @author <a href="http://www.martin-loetzsch.de">Martin Loetzsch</a>
*/
class ArithmeticOperator : public DecimalExpression
{
public:
/**
* Creates the operator
* @param operand1 The first operand
* @param operand2 The second operand
  */
  void create(DecimalExpression* operand1, DecimalExpression* operand2);
  
  /** Calculates the value of the decimal expression. */
  virtual double getValue() const = 0;
  
  /** Destructor. Deletes the operands */
  ~ArithmeticOperator();
  
protected:
  /** The first operand */
  DecimalExpression* operand1;
  
  /** The second operand */
  DecimalExpression* operand2;
};

/** 
* @class PlusOperator
*
* Represents a + operator in the option graph 
* 
* @author <a href="http://www.martin-loetzsch.de">Martin Loetzsch</a>
*/
class PlusOperator : public ArithmeticOperator
{
public:
  /** Calculates the value of the decimal expression. */
  virtual double getValue() const;
};

/** 
* @class MinusOperator
*
* Represents a - operator in the option graph 
* 
* @author <a href="http://www.martin-loetzsch.de">Martin Loetzsch</a>
*/
class MinusOperator : public ArithmeticOperator
{
public:
  /** Calculates the value of the decimal expression. */
  virtual double getValue() const;
};


/** 
* @class MultiplyOperator
*
* Represents a * operator in the option graph
* 
* @author <a href="http://www.martin-loetzsch.de">Martin Loetzsch</a>
*/
class MultiplyOperator : public ArithmeticOperator
{
public:
  /** Calculates the value of the decimal expression. */
  virtual double getValue() const;
};

/** 
* @class DivideOperator
*
* Represents a / operator in the option graph 
* 
* @author <a href="http://www.martin-loetzsch.de">Martin Loetzsch</a>
*/
class DivideOperator : public ArithmeticOperator
{
public:
  /** Calculates the value of the decimal expression. */
  virtual double getValue() const;
};

/** 
* @class ModOperator
*
* Represents a % operator in the option graph 
* 
* @author <a href="http://www.martin-loetzsch.de">Martin Loetzsch</a>
*/
class ModOperator : public ArithmeticOperator
{
public:
  /** Calculates the value of the decimal expression. */
  virtual double getValue() const;
};

/** 
* @class TimeRef
*
* Represents a time-of-option-execution or time-of-state-execution element in the option graph 
* 
* @author <a href="http://www.martin-loetzsch.de">Martin Loetzsch</a>
*/
class TimeRef : public DecimalExpression
{
public:
  /** 
  * Constructor
  * @param errorHandler A reference to a ErrorHandler instance
  * @param time the referenced time
  */
  TimeRef(ErrorHandler& errorHandler,
    unsigned& time);
  
  /** Calculates the value of the decimal expression. */
  virtual double getValue() const;
  
private:
  /** The referenced time */
  unsigned& time;
};

/** 
* @class DecimalInputSymbolRef
* 
* Represents a reference to a decimal input symbol.
*
* @author <a href="http://www.martin-loetzsch.de">Martin Loetzsch</a>
* @author <a href="http://www.sim.informatik.tu-darmstadt.de/pers/card/risler.html">Max Risler</a>
*/
class DecimalInputSymbolRef : public DecimalExpression
{
public:
  /** Calculates the value of the decimal expression. */
  virtual double getValue() const;
  
  /**
  * Constructor. Creates the function call depending on the input.
  * @param input An input source for the intermediate code. It must be opened and read until 
  *              A position where the function reference starts.
  * @param errorHandler A reference to a ErrorHandler instance
  * @param symbols All available symbols
  * @param option The current option
  * @param state The current state
  */
  DecimalInputSymbolRef(InputSource& input, 
    ErrorHandler& errorHandler,
    Symbols& symbols,
    Option& option,
    State& state);
  
  /** Destructor */
  ~DecimalInputSymbolRef();

private:
  /** The referenced symbol */
  DecimalInputSymbol* symbol;

  /** The parameter assignments of the referenced symbol */
  ParameterAssignment* parameters;
};

/** 
* @class DecimalOutputSymbolRef
* 
* Represents a reference to a decimal input symbol.
*
* @author <a href="http://www.sim.informatik.tu-darmstadt.de/pers/card/risler.html">Max Risler</a>
*/
class DecimalOutputSymbolRef : public DecimalExpression
{
public:
  /** Calculates the value of the decimal expression. */
  virtual double getValue() const;
  
  /**
  * Constructor. Creates the function call depending on the input.
  * @param input An input source for the intermediate code. It must be opened and read until 
  *              A position where the function reference starts.
  * @param errorHandler A reference to a ErrorHandler instance
  * @param symbols All available symbols
  */
  DecimalOutputSymbolRef(InputSource& input, 
    ErrorHandler& errorHandler,
    Symbols& symbols);

private:
  /** The referenced symbol */
  DecimalOutputSymbol* symbol;
};

/** 
* @class ConditionalDecimalExpression
* 
* Represents an ANSI C (condition?expression:expression) question mark operator
*
* @author <a href="http://www.martin-loetzsch.de">Martin Loetzsch</a>
* @author <a href="http://www.sim.informatik.tu-darmstadt.de/pers/card/risler.html">Max Risler</a>
*/
class ConditionalDecimalExpression : public DecimalExpression
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
  ConditionalDecimalExpression(InputSource& input, 
    ErrorHandler& errorHandler,
    Symbols& symbols,
    Option& option,
    State& state);

  /** Destructor */
  ~ConditionalDecimalExpression();

  /** Calculates the value of the decimal expression. */
  virtual double getValue() const;

private:
  /** The condition */
  BooleanExpression* condition;

  /** The expression that is returned when the condition evaluates true */
  DecimalExpression* expression1;

  /** The expression that is returned when the condition evaluates false */
  DecimalExpression* expression2;
};

} // namespace

#endif //__XabslDecimalExpression_h_
