/**
* @file XabslTeamMessage.h
* 
* Definition of class TeamMessage
*
* @author <a href="http://www.sim.informatik.tu-darmstadt.de/pers/card/risler.html">Max Risler</a>
*/

#ifndef __XabslTeamMessage_h_
#define __XabslTeamMessage_h_

#include "XabslArray.h"

namespace xabsl 
{

/**
* @class TeamMessage
*
* Represents an incoming or outgoing message transmitted from/to other cooperating agents
*
* @author <a href="http://www.sim.informatik.tu-darmstadt.de/pers/card/risler.html">Max Risler</a>
*/
class TeamMessage
{
public:
  /** 
  * Constructor.
  */
  TeamMessage() : agentPriority(0)
  {};

  /** An array containing indexes of currently executed cooperative states */
  Array<int> coopStatesExecuted;

  /** An array containing indexes of cooperative states, currently trying to be executed but blocked due to cooperating agents */
  Array<int> coopStatesEntering;

  /** An array containing indexes of cooperative states, whose corresponding option is being executed */
  Array<int> coopStatesOptionExecuted;

  /** Priority value of the agent, used for solving conflicts in cooperative state assignment */
  int agentPriority;

  /** Reset the content of the message */
  void reset()
  {
    coopStatesExecuted.clear();
    coopStatesEntering.clear();
    coopStatesOptionExecuted.clear();
  }
};

} // namespace

#endif //__XabslTeamMessage_h_
