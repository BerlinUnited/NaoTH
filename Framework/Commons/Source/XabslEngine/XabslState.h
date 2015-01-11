/**
* @file XabslState.h
* 
* Definition of class State and Helper classes
*
* @author <a href="http://www.sim.informatik.tu-darmstadt.de/pers/card/risler.html">Max Risler</a>
*/

#ifndef __XabslState_h_
#define __XabslState_h_

#include "XabslStatement.h"
#include "XabslBasicBehavior.h"
#include "XabslDecimalExpression.h"
#include "XabslBooleanExpression.h"
#include "XabslEnumeratedExpression.h"
#include "XabslAction.h"

namespace xabsl 
{

// class prototype needed for declaration of State
class Option;

/**
* @class State
*
* Represents a single state written in 
*
* @author <a href="http://www.martin-loetzsch.de">Martin Loetzsch</a>
*/
class State : public NamedItem
{
public:
  /** 
  * Constructor. Does not create the state.
  * @param name The name of the state. For debugging purposes.
  * @param errorHandler A reference to a ErrorHandler instance
  * @param pTimeFunction a pointer to a function that returns the system time in ms.
  * @param optionIndex Index of the corresponding option this state belongs to
  * @param index Index of the state in array states in corresponding option
  */
  State(const char* name, 
    ErrorHandler& errorHandler,
    unsigned (*pTimeFunction)(),
    int optionIndex,
    int index);
  
  /** Destructor */
  ~State();
  
  /** 
  * Creates the state and it's subelements from the intermediate code.
  * @param input An input source for the intermediate code. It must be opened and read until 
  *              A position where a state starts.
  * @param options All available options
  * @param basicBehaviors All available basicBehaviors
  * @param symbols All available symbols
  */
  void create(InputSource& input,    
    NamedArray<Option*>& options,
    NamedArray<BasicBehavior*>& basicBehaviors,
    Symbols& symbols);
  
  /** 
  * Executes the decision tree and determines the next active state (can be the same). 
  */
  virtual State* getNextState();
  
  /** The actions of the state */
  Array<Action*> actions;

  /** 
  * The first subsequent option that is executed after that option if the state is active. 
  * If 0, no subsequent option is executed after that option. 
  * This pointer is present just for compatibility reasons, please use getactions instead
  */
  Option* subsequentOption;  

  /** The time how long the state is already active */
  unsigned timeOfStateExecution;
  
  /** Sets the time when the state was activated to 0 */
  void reset();

  /** Returns wheter the state is a target state */
  bool isTargetState() const;

  /** Index of the state in array states in corresponding option */
  int index;

  /** The time, when the state was activated */
  unsigned timeWhenStateWasActivated;

  /** Index of the corresponding option this state belongs to */
  int optionIndex;

  /** Check whether this state can be entered, or whether entering is blocked due to cooperating agents */
  virtual bool coopCheck() {return true;}

  /** Whether this state is currently conflicted, i.e whether cooperation conditions are violated */
  virtual bool getConflict() {return false;}

protected:

  /** If true, the state is a target state */
  bool targetState;

  /** Used for error handling */
  ErrorHandler& errorHandler;
  
  /** The root element of the decision tree */
  Statement* decisionTree;

  /** A pointer to a function that returns the system time in ms. */
  unsigned (*pTimeFunction)();

};

} // namespace

#endif //__XabslState_h_
