/**
* @file XabslStatement.h
* 
* Definition of class Statement and Helper classes
*
* @author <a href="http://www.sim.informatik.tu-darmstadt.de/pers/card/risler.html">Max Risler</a>
*/

#ifndef __XabslStatement_h_
#define __XabslStatement_h_

#include "XabslBooleanExpression.h"

namespace xabsl 
{

// class prototype needed for the declaration of Statement
class State;

/** 
* @class Statement 
* 
* An element of a decision that that determines a transition to a state.
* It can be either a transition to a state or a if/else-if/else block containing other statements.
* 
* @author <a href="http://www.martin-loetzsch.de">Martin Loetzsch</a>
* @author <a href="http://www.sim.informatik.tu-darmstadt.de/pers/card/risler.html">Max Risler</a>
*/
class Statement
{
public:
  /** Executes the statement and determines the next active state (can be the same). */
  virtual State* getNextState() = 0;
  
  /** 
  * Creates a statement depending on the input.
  * @param input An input source for the intermediate code. It must be opened and read until 
  *              A position where a statement starts.
  * @param errorHandler A reference to a ErrorHandler instance
  * @param symbols All available symbols
  * @param option The current option
  * @param state The current state
  */
  static Statement* createStatement(InputSource& input,    
    ErrorHandler& errorHandler,
    Symbols& symbols,    
    Option& option,
    State& state);
  
  /** Destructor */
  virtual ~Statement() = 0;
};

/**
* @class IfElseBlock
*
* An element of a decision tree that that contains of an if - (else-if) - else block
* 
* @author <a href="http://www.martin-loetzsch.de">Martin Loetzsch</a>
*/
class IfElseBlock : public Statement
{
public:
  /** 
  * Constructor. Creates the if / else statement
  * @param input An input source for the intermediate code. It must be opened and read until 
  *              A position where a transition starts.
  * @param errorHandler A reference to a ErrorHandler instance
  * @param symbols All available symbols
  * @param option The current option
  * @param state The current state
  */
  IfElseBlock(InputSource& input, 
    ErrorHandler& errorHandler,
    Symbols& symbols,
    Option& option,
    State& state
    );
  
  /** Destructor. Deletes all statements and conditions */
  ~IfElseBlock();
  
  /** Executes the statement and determines the next active state (can be the same). */
  virtual State* getNextState();
  
private:
  /** The boolean expression that is evaluated for the if case */
  BooleanExpression* ifCondition;
  
  /** The statement that is executed if the if condition is true */
  Statement* ifStatement;
  
  /** The boolean expressions for the else-if cases */
  NamedArray<BooleanExpression*> elseIfConditions;
  
  /** The statements that are executed if the else-if condition are true */
  NamedArray<Statement*> elseIfStatements;
  
  /** The statement that is executed if all if and else-if conditions fail */
  Statement* elseStatement;
};


/**
* Represents a transition to a state inside a decision tree
*/
class TransitionToState : public Statement
{
public:
  /** 
  * Constructor. Creates the transition object
  * @param input An input source for the intermediate code. It must be opened and read until 
  *              A position where a transition starts.
  * @param errorHandler A reference to a ErrorHandler instance
  * @param option The current option
  */
  TransitionToState(InputSource& input, 
    ErrorHandler& errorHandler,
    Option& option);
  
  /** Executes the statement and determines the next active state (can be the same). */
  virtual State* getNextState();
  
private:
  /** The state where that transition points to */
  State* nextState;
};

} // namespace

#endif // __XabslStatement_h_
