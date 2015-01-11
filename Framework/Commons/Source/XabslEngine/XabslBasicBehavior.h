/** 
* @file XabslBasicBehavior.h
*
* Declaration class BasicBehavior
*
* @author <a href="http://www.martin-loetzsch.de">Martin Loetzsch</a>
* @author <a href="http://www.sim.informatik.tu-darmstadt.de/pers/card/risler.html">Max Risler</a>
* @author <a href="http://www.informatik.hu-berlin.de/~juengel">Matthias JŸngel</a>
*/

#ifndef __XabslBasicBehavior_h_
#define __XabslBasicBehavior_h_

#include "XabslTools.h"
#include "XabslBehavior.h"

namespace xabsl 
{

/**
* The base class for basic behaviors that are used by the Engine
*
* @author <a href="http://www.martin-loetzsch.de">Martin Loetzsch</a>
* @author <a href="http://www.sim.informatik.tu-darmstadt.de/pers/card/risler.html">Max Risler</a>
* @author <a href="http://www.informatik.hu-berlin.de/~juengel">Matthias JŸngel</a>
*/
class BasicBehavior : public Behavior
{
public:
  /** 
  * Constructor 
  * @param name The name of the basic behavior
  * @param errorHandler A reference to the error handler
  */
  BasicBehavior(const char* name, ErrorHandler& errorHandler)
    : Behavior(name), index(-1)
  {
    parameters = new Parameters(errorHandler);
  };
  
  /** Destructor */
  virtual ~BasicBehavior()
  {
    delete parameters;
  }

  /** Registers the parameters. */
  virtual void registerParameters() {}

  /** Index of the basic behavior in array basicBehaviors in corresponding engine */
  int index;
};



} // namespace

#endif // __XabslBasicBehavior_h_

