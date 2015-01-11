/**
* @file XabslBehavior.h
* 
* Definition of class Behavior
*
* @author <a href="http://www.sim.informatik.tu-darmstadt.de/pers/card/risler.html">Max Risler</a>
*/

#ifndef __XabslBehavior_h_
#define __XabslBehavior_h_

#include "XabslArray.h"
#include "XabslSymbols.h"

namespace xabsl 
{

/** 
* @class Behavior
*
* Parent class for Option and BasicBehavior
*
* @author <a href="http://www.sim.informatik.tu-darmstadt.de/pers/card/risler.html">Max Risler</a>
*/
class Behavior : public NamedItem
{
public:
  /** 
  * Constructor.
  * @param name The name of the behavior. For debugging purposes.
  */
  Behavior(const char* name) : 
      NamedItem(name), 
      isActive(false),
      wasActive(false),
      timeOfExecution(0),
      timeWhenActivated(0),
      parameters(0)
  {};

  /**
  * Destructor
  */
  virtual ~Behavior() {}

  /** the behavior is activated in the current path through the option graph */
  bool isActive;
  
  /** the behavior was activated in the last path trough the option graph */
  bool wasActive;

  /** 
  * executes the behavior
  */
  virtual void execute() = 0;

  /** the time how long the option is already active */
  unsigned timeOfExecution;

  /** The time, when the option was activated */
  unsigned timeWhenActivated;

  /** The parameters of the behavior */
  Parameters* parameters; 

  /** Notify the software environment about a parameter change */
  virtual void parametersChanged() {}
};

} // namespace

#endif //__XabslBehavior_h_
