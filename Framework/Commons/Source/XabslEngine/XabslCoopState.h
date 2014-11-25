/**
* @file XabslCoopState.h
* 
* Definition of class CoopState and Helper classes
*
* @author <a href="http://www.sim.informatik.tu-darmstadt.de/pers/card/risler.html">Max Risler</a>
*/

#ifndef __XabslCoopState_h_
#define __XabslCoopState_h_

#include "XabslState.h"

namespace xabsl 
{

/**
* @class CoopState
*
* Represents a state which has features to connect to other cooperating agents
*
* @author <a href="http://www.sim.informatik.tu-darmstadt.de/pers/card/risler.html">Max Risler</a>
*/
class CoopState : public State
{
public:
  /** 
  * Constructor. Does not create the state.
  * @param name The name of the state. For debugging purposes.
  * @param errorHandler A reference to a ErrorHandler instance
  * @param pTimeFunction a pointer to a function that returns the system time in ms.
  * @param optionIndex Index of the corresponding option this state belongs to
  * @param index Index of the state in array states in corresponding option
  * @param agentPriority Priority value of the agent, used for solving conflicts in cooperative state assignment
  * @param synchronizationTicks Number of execution cycles that is required for synchonization with other agents, i.e time from sending a message until answer is received
  */
  CoopState(const char* name, 
    ErrorHandler& errorHandler,
    unsigned (*pTimeFunction)(),
    int optionIndex,
    int index,
    const int& agentPriority,
    const int& synchronizationTicks) : 
    State(name, errorHandler, pTimeFunction, optionIndex, index),
    numberOfAgentsExecuting(0),
    numberOfAgentsEntering(0),
    numberOfAgentsInOption(0),
    highestPriorityOfAgentsEntering(0),
    entering(false),
    enterCount(0),
    agentPriority(agentPriority),
    synchronizationTicks(synchronizationTicks)
  {};

  /** Number of other agents currently executing this state, not including myself */
  int numberOfAgentsExecuting;

  /** Number of other agents currently trying to execute this state, but are blocked due to cooperating agents, not including myself */
  int numberOfAgentsEntering;

  /** Number of other agents currently executing the option corresponding to this state, not including myself */
  int numberOfAgentsInOption;

  /** Highest priority values of agents currently entering this state, not including myself */
  int highestPriorityOfAgentsEntering;

  /** Flag whether the agent is currently trying to execute this state, but is blocked due to cooperating agents */
  bool entering;

  /** Counter for number of cycles that the enter flag of this state is set */
  int enterCount;

  /** Priority value of the agent, used for solving conflicts in cooperative state assignment */
  const int &agentPriority;

  /** Number of execution cycles that is required for synchonization with other agents, i.e time from sending a message until answer is received */
  const int &synchronizationTicks;

  /** Prepare for processing incoming team messages. This resets previously processed messages. */
  void prepareIncomingMessages()
  {
    numberOfAgentsExecuting = numberOfAgentsEntering = numberOfAgentsInOption = 0;
    if (entering) 
      enterCount++; 
    else 
      enterCount = 0;
    entering = false;
  }

};

/**
* @class SynchronizedState
*
* Represents a state which can only be entered by all agents simultaneously
*
* @author <a href="http://www.sim.informatik.tu-darmstadt.de/pers/card/risler.html">Max Risler</a>
*/
class SynchronizedState : public CoopState
{
public:
  /** 
  * Constructor. Does not create the state.
  * @param name The name of the state. For debugging purposes.
  * @param errorHandler A reference to a ErrorHandler instance
  * @param pTimeFunction a pointer to a function that returns the system time in ms.
  * @param optionIndex Index of the corresponding option this state belongs to
  * @param index Index of the state in array states in corresponding option
  * @param agentPriority Priority value of the agent, used for solving conflicts in cooperative state assignment
  * @param synchronizationTicks Number of execution cycles that is required for synchonization with other agents, i.e time from sending a message until answer is received
  */
  SynchronizedState(const char* name, 
    ErrorHandler& errorHandler,
    unsigned (*pTimeFunction)(),
    int optionIndex,
    int index,
    const int& agentPriority,
    const int& synchronizationTicks,
    int minAgents) : 
    CoopState(name, errorHandler, pTimeFunction, optionIndex, index, agentPriority, synchronizationTicks), minAgents(minAgents)
  {};

  /** Minimum number of agents that have to enter the state at once */
  int minAgents;

  /** Check whether this state can be entered, or whether entering is blocked due to cooperating agents */
  virtual bool coopCheck() 
  {
    entering = true;
    return (numberOfAgentsInOption + 1 >= minAgents &&
            numberOfAgentsExecuting + numberOfAgentsEntering == numberOfAgentsInOption &&
            enterCount > synchronizationTicks / 2);
  }
};

/**
* @class CapacityState
*
* Represents a state which can only be entered by at most the given number of agents simultaneously
*
* @author <a href="http://www.sim.informatik.tu-darmstadt.de/pers/card/risler.html">Max Risler</a>
*/
class CapacityState : public CoopState
{
public:
  /** 
  * Constructor. Does not create the state.
  * @param name The name of the state. For debugging purposes.
  * @param errorHandler A reference to a ErrorHandler instance
  * @param pTimeFunction a pointer to a function that returns the system time in ms.
  * @param optionIndex Index of the corresponding option this state belongs to
  * @param index Index of the state in array states in corresponding option
  * @param agentPriority Priority value of the agent, used for solving conflicts in cooperative state assignment
  * @param synchronizationTicks Number of execution cycles that is required for synchonization with other agents, i.e time from sending a message until answer is received
  * @param capacity Number of agents that can enter the state simultaneously
  */
  CapacityState(const char* name, 
    ErrorHandler& errorHandler,
    unsigned (*pTimeFunction)(),
    int optionIndex,
    int index,
    const int& agentPriority,
    const int& synchronizationTicks,
    int capacity) :
    CoopState(name, errorHandler, pTimeFunction, optionIndex, index, agentPriority, synchronizationTicks), capacity(capacity), conflict(false)
  {};

  /** Number of agents that can enter the state simultaneously */
  int capacity;

  /** Whether this state is currently conflicted, i.e. there are more robots executing than the given capacity.*/
  bool conflict;

  /** Whether this state is currently conflicted, i.e. there are more robots executing than the given capacity.*/
  virtual bool getConflict()
  {
    return conflict;
  }

  /** 
  * Executes the decision tree and determines the next active state (can be the same). 
  */
  virtual State* getNextState()
  {
    conflict = numberOfAgentsExecuting >= capacity;
    //if (conflict)
    //{
    //  errorHandler.message("xabsl::CapacityState::getNextState(): Conflict detected in capacity state \"%s\". Number of agents executing is %d while the capacity is %d. Number of synchronization ticks might need to be increased; current value is %d.",n,numberOfAgentsExecuting,capacity,synchronizationTicks);
    //}
    return CoopState::getNextState();
  }

  /** Check whether this state can be entered, or whether entering is blocked due to cooperating agents */
  virtual bool coopCheck() 
  {
    //if (numberOfAgentsInOption < capacity)
    //{
    //  // no potential conflict for state capacity => enter state
    //  return true;
    //}
    //else 
    if (numberOfAgentsExecuting >= capacity)
    {
      // state is currently busy
      entering = true;
      return false;
    }
    else if (numberOfAgentsExecuting + numberOfAgentsEntering >= capacity && highestPriorityOfAgentsEntering > agentPriority) 
    {
      // state seems to have available capacity but other agents with higher priority want to enter it
      entering = true;
      return false;
    }
    else if (numberOfAgentsExecuting + numberOfAgentsEntering >= capacity && highestPriorityOfAgentsEntering == agentPriority) 
    {
      // another agent wants to enter the state that has the same priority
      errorHandler.message("xabsl::CapacityState::coopCheck(): Conflict detected in capacity state \"%s\". Multiple agents with the same priority value %d are trying to execute the state.",n,agentPriority);
      // in order to resolve the conflict we do not set enter flag
      // if both agents remove their flag at the same time this situation might repeat indefinitely
      return false;
    }
    else if (enterCount < synchronizationTicks)
    {
      // state seems to have available capacity => set enter flag and wait specified tick count
      entering = true;
      return false;
    }
    else
    {
      // enter state only after enter count has reached specified count
      return true;
    }
  }
};

} // namespace

#endif //__XabslCoopState_h_
