/**
* @file XabslAction.h
* 
* Definition of class Action and Helper classes
*
* @author <a href="http://www.sim.informatik.tu-darmstadt.de/pers/card/risler.html">Max Risler</a>
*/

#ifndef __XabslAction_h_
#define __XabslAction_h_

#include "XabslBasicBehavior.h"
#include "XabslDecimalExpression.h"
#include "XabslBooleanExpression.h"
#include "XabslEnumeratedExpression.h"

namespace xabsl 
{

// class prototype needed for declaration of Action
class Option;

/**
* @class Action
*
* Represents an action execution. This is either a subsequent option or basic behavior to be executed, or an 
* output symbol assignment.
*
* @author Max Risler
*/
class Action
{
public:
  /**
  * Constructor.
  * @param pTimeFunction a pointer to a function that returns the system time in ms.
  */
  Action(
    unsigned (*pTimeFunction)()) :
      pTimeFunction(pTimeFunction)
  {}

  /**
  * Virtual destructor.
  */
  virtual ~Action() {}

  /** 
  * Creates an action definition.
  * @param input An input source for the intermediate code. It must be opened and read until 
  *              A position where a state starts.
  * @param options All available options
  * @param basicBehaviors All available basicBehaviors
  * @param symbols All available symbols
  * @param option The current option
  * @param state The current state
  * @param errorHandler A reference to a ErrorHandler instance
  * @param pTimeFunction a pointer to a function that returns the system time in ms.
  */
  static Action* create(
    InputSource& input,    
    NamedArray<Option*>& options,
    NamedArray<BasicBehavior*>& basicBehaviors,
    Symbols& symbols,
    Option& option,
    State& state,
    ErrorHandler& errorHandler,
    unsigned (*pTimeFunction)());
  
  /** 
  * Creates an action definition which just calls a single option or basic behavior
  * without setting any parameters.
  * @param behavior The referenced option or basic behavior.
  * @param errorHandler A reference to a ErrorHandler instance
  * @param pTimeFunction a pointer to a function that returns the system time in ms.
  */
  static Action* create(
    Behavior* behavior,
    ErrorHandler& errorHandler,
    unsigned (*pTimeFunction)());
  
  /** Execute the behavior or assign the output symbol */
  virtual void execute() = 0;

  /** Returns a pointer to the option or basic behavior to be executed, or 0 if an output symbol is set */
  Behavior* getBehavior();
  const Behavior* getBehavior() const;

  /** Returns a pointer to the option, if an option is to be executed, 0 otherwise */
  Option* getOption();
  const Option* getOption() const;

  /** Returns a pointer to the basic behavior, if a basic behavior is to be executed, 0 otherwise */
  BasicBehavior* getBasicBehavior();
  const BasicBehavior* getBasicBehavior() const;
  
  /** Returns a pointer to the parameter assignments for an option or basic behavior, or 0 if an output symbol is set */
  ParameterAssignment* getParameters();
  const ParameterAssignment* getParameters() const;

  /** Returns a pointer to the output symbol, if a decimal output symbol is to be assigned, 0 otherwise */
  const DecimalOutputSymbol* getDecimalOutputSymbol() const;

  /** Returns a pointer to the output symbol, if a boolean output symbol is to be assigned, 0 otherwise */
  const BooleanOutputSymbol* getBooleanOutputSymbol() const;

  /** Returns a pointer to the output symbol, if an enumerated output symbol is to be assigned, 0 otherwise */
  const EnumeratedOutputSymbol* getEnumeratedOutputSymbol() const;

  /** Returns the last symbol value, if a decimal output symbol is to be assigned, 0 otherwise */
  double getDecimalOutputSymbolValue() const;

  /** Returns the last symbol value, if a boolean output symbol is to be assigned, 0 otherwise */
  bool getBooleanOutputSymbolValue() const;

  /** Returns the last symbol value, if an enumerated output symbol is to be assigned, 0 otherwise */
  int getEnumeratedOutputSymbolValue() const;
  
protected:
  /** A pointer to a function that returns the system time in ms. */
  unsigned (*pTimeFunction)();
};

/**
* @class ActionBehavior
*
* Represents an action execution. This is either a subsequent option or basic behavior to be executed.
*
* @author Max Risler
*/
class ActionBehavior : public Action
{
public:
  /**
  * Constructor.
  * @param pTimeFunction a pointer to a function that returns the system time in ms.
  */
  ActionBehavior(
    unsigned (*pTimeFunction)()) :
      Action(pTimeFunction),
      parameters(0)
  {}

  /** Destructor */
  ~ActionBehavior();

  /**
  * Parameters of the option or basic behavior that is executed
  */
  ParameterAssignment* parameters;

  /** 
  * The option or basic behavior that is executed
  */
  virtual const Behavior* getBehavior() const = 0;  
  virtual Behavior* getBehavior() = 0;  

  /** Execute the behavior */
  virtual void execute();
};

/**
* @class ActionBasicBehavior
*
* Represents an action execution. In this case a basic behavior is to be executed.
*
* @author Max Risler
*/
class ActionBasicBehavior : public ActionBehavior
{
public:
  /**
  * Constructor.
  * @param pTimeFunction a pointer to a function that returns the system time in ms.
  */
  ActionBasicBehavior(
    unsigned (*pTimeFunction)()) :
      ActionBehavior(pTimeFunction),
      basicBehavior(0)
  {}

  BasicBehavior* basicBehavior;

  virtual const Behavior* getBehavior() const;
  virtual Behavior* getBehavior();
};

/**
* @class ActionOption
*
* Represents an action execution. In this case an option is to be executed.
*
* @author Max Risler
*/
class ActionOption : public ActionBehavior
{
public:
  /**
  * Constructor.
  * @param pTimeFunction a pointer to a function that returns the system time in ms.
  */
  ActionOption(
    unsigned (*pTimeFunction)()) :
      ActionBehavior(pTimeFunction),
      option(0)
  {}

  Option* option;

  virtual const Behavior* getBehavior() const;
  virtual Behavior* getBehavior();
};

/**
* @class ActionDecimalOutputSymbol
*
* Represents an action execution, in this case a decimal output symbol assignment
*
* @author Max Risler
*/
class ActionDecimalOutputSymbol : public Action
{
public:
  /**
  * Constructor.
  * @param pTimeFunction a pointer to a function that returns the system time in ms.
  */
  ActionDecimalOutputSymbol(
   unsigned (*pTimeFunction)()) :
      Action(pTimeFunction),
      decimalOutputSymbol(0),
      decimalOutputSymbolExpression(0),
      decimalOutputSymbolValue(0)
  {}

  /** Destructor */
  ~ActionDecimalOutputSymbol();

  /** A decimal output symbol that is set if the state is active, null when a behavior is executed or another output symbol is set */
  DecimalOutputSymbol* decimalOutputSymbol;
  /** The expression for the decimal output symbol that is set if the state is active */
  const DecimalExpression* decimalOutputSymbolExpression;
  /** Current decimal output symbol value, this is stored just for debugging purposes */
  double decimalOutputSymbolValue;

  /** Execute the behavior */
  virtual void execute();
};

/**
* @class ActionBooleanOutputSymbol
*
* Represents an action execution, in this case a boolean output symbol assignment
*
* @author Max Risler
*/
class ActionBooleanOutputSymbol : public Action
{
public:
  /**
  * Constructor.
  * @param pTimeFunction a pointer to a function that returns the system time in ms.
  */
  ActionBooleanOutputSymbol(
    unsigned (*pTimeFunction)()) :
      Action(pTimeFunction),
      booleanOutputSymbol(0),
      booleanOutputSymbolExpression(0),
      booleanOutputSymbolValue(false)
  {}

  /** Destructor */
  ~ActionBooleanOutputSymbol();

  /** A boolean output symbol that is set if the state is active, null when a behavior is executed or another output symbol is set  */
  BooleanOutputSymbol* booleanOutputSymbol;
  /** The expression for the boolean output symbol that is set if the state is active */
  const BooleanExpression* booleanOutputSymbolExpression;
  /** Current boolean output symbol value, this is stored just for debugging purposes */
  bool booleanOutputSymbolValue;

  /** Execute the behavior */
  virtual void execute();
};

/**
* @class ActionEnumeratedOutputSymbol
*
* Represents an action execution, in this case an enumerated output symbol assignment
*
* @author Max Risler
*/
class ActionEnumeratedOutputSymbol : public Action
{
public:
  /**
  * Constructor.
  * @param pTimeFunction a pointer to a function that returns the system time in ms.
  */
  ActionEnumeratedOutputSymbol(
    unsigned (*pTimeFunction)()) :
      Action(pTimeFunction),
      enumeratedOutputSymbol(0),
      enumeratedOutputSymbolExpression(0),
      enumeratedOutputSymbolValue(0)
  {}

  /** Destructor */
  ~ActionEnumeratedOutputSymbol();

  /** An enumerated output symbol that is set if the state is active, null when a behavior is executed or another output symbol is set  */
  EnumeratedOutputSymbol* enumeratedOutputSymbol;
  /** The expression for the enumerated output symbol that is set if the state is active */
  const EnumeratedExpression* enumeratedOutputSymbolExpression;
  /** Current enumerated output symbol value, this is stored just for debugging purposes */
  int enumeratedOutputSymbolValue;

  /** Execute the behavior */
  virtual void execute();
};

} // namespace

#endif //__XabslAction_h_
