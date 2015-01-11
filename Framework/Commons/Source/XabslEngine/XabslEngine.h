/** 
* @file XabslEngine.h
*
* Declaration class Engine
*
* @author <a href="http://www.martin-loetzsch.de">Martin Loetzsch</a>
* @author <a href="http://www.sim.informatik.tu-darmstadt.de/pers/card/risler.html">Max Risler</a>
* @author <a href="http://www.informatik.hu-berlin.de/~juengel">Matthias JŸngel</a>
*/

#ifndef __XabslEngine_h_
#define __XabslEngine_h_

#include "XabslAgent.h"
#include "XabslOption.h"
#include "XabslCoopState.h"
#include "XabslTeamMessage.h"

namespace xabsl 
{

/**
* Executes a behavior that was specified in the Xabsl language.
*
* @author <a href="http://www.martin-loetzsch.de">Martin Loetzsch</a>
* @author <a href="http://www.sim.informatik.tu-darmstadt.de/pers/card/risler.html">Max Risler</a>
* @author <a href="http://www.informatik.hu-berlin.de/~juengel">Matthias JŸngel</a>
*/
class Engine : public Symbols
{
public:
  /** 
  * Constructor 
  * @param e Is invoked when there are problems during initialization
  * @param pTimeFunction a pointer to a function that returns the system time in ms.
  */
  Engine(ErrorHandler& e, unsigned (*pTimeFunction)());

  /** Destructor */
  ~Engine();

  /** 
  * Executes the engine for the selected agent starting from the root option.
  * (Including the selected basic behavior)
  */
  void execute();

  /**
  * Reads the intermediate code from an input source and creates the option graph.
  * Note that the basic behaviors and symbols have to be registered before that function is 
  * called.
  */
  void createOptionGraph(InputSource& input);

  /** 
  * Registers a basic behavior at the engine. 
  * This must be done before the intermediate code is read.
  * @param basicBehavior A reference to the basic behavior
  */
  void registerBasicBehavior(BasicBehavior& basicBehavior);

  /** 
  * Sets the selected Agent.
  * If the last selected agent was different from the new one, 
  * the root option is changed depending on the new agent.
  * @param name The name of the agent
  * @return if the requested agent exists
  */
  bool setSelectedAgent(const char* name);

  /**
  * Resets all active options.
  * Next cycle will execute initial state of currently set root option.
  */
  void reset();

private:

  /** The agents of the engine */
  NamedArray<Agent*> agents;

  /** The selected agent */
  Agent* selectedAgent;

  /** The options of the engine */
  NamedArray<Option*> options;

  /** The actions for the execution of the start of the option graph */
  Array<Action*> rootActions;

  /** The registered basic behaviors of the engine */
  NamedArray<BasicBehavior*> basicBehaviors;

  /** Is invoked when there are problems during initialization */
  ErrorHandler& errorHandler;

  /** 
  * A recursive function that is used to check for loops in the option graph.
  * @param currenOptionPath An array of the currently traced option path
  * @param currentDepth The depth of the current option path
  * @return If true, then a loop was detected.
  */
  bool checkForLoops(Option* currenOptionPath[], int currentDepth);

  /** If true, the engine was successfully initialized */
  bool initialized;

  /** A pointer to a function that returns the system time in ms. */
  unsigned (*pTimeFunction)();

  /** Arrays containing internal symbol values */
  NamedArray<double> internalDecimalSymbols;
  NamedArray<bool> internalBooleanSymbols;
  NamedArray<int> internalEnumeratedSymbols;

  /** Array of states which are used for cooperating agents */
  Array<CoopState*> coopStates;

  /** Priority value of the agent, used for solving conflicts in cooperative state assignment */
  int agentPriority;

  /** Number of execution cycles that is required for synchonization with other agents, i.e time from sending a message until answer is received */
  int synchronizationTicks;

public:
  //!@name Debugging Interface 
  //!@{

  /** Returns the agents of the engine */
  const NamedArray<Agent*>& getAgents() const {return agents;}

  /** Return the options of the engine */
  const NamedArray<Option*>& getOptions() const {return options;}

  /** Returns the registered basic behaviors of the engine */
  const NamedArray<BasicBehavior*>& getBasicBehaviors() const {return basicBehaviors;}

  /**
  * Clears the list of actions executed starting the option graph.
  */
  void clearRootActions();

  /** 
  * Adds a given option or basic behavior to the list of actions executed starting the option graph.
  * Can be called to test a single option or basic behavior.
  * @param name The name of the option or basic behavior
  * @param isOption True for an option, false for a basic behavior
  * @return When false, the option is not known to the engine
  */
  bool addRootAction(const char* name, bool isOption = true);

  /** 
  * Executes the option graph starting from a given option or basic behavior.
  * Can be called to test a single option or basic behavior.
  * @param name The name of the option or basic behavior
  * @param isOption True for an option, false for a basic behavior
  * @return When false, the option is not known to the engine
  */
  bool setRootAction(const char* name, bool isOption = true);

  /** 
  * Sets the root option of the selected agent
  */
  void setRootAction();

  /** 
  * Adds the specified option or basic behavior to the list of root actions.
  */
  void addRootAction(Behavior* behavior);

  /**
  * Adds a decimal output symbol assignment to the list of root actions.
  */
  bool addRootActionDecimalOutputSymbol(const char* name, double value);

  /**
  * Adds a boolean output symbol assignment to the list of root actions.
  */
  bool addRootActionBooleanOutputSymbol(const char* name, bool value);

  /**
  * Adds an enumerated output symbol assignment to the list of root actions.
  */
  bool addRootActionEnumeratedOutputSymbol(const char* name, const char* value);

  /** 
  * Sets the root option to the specified option or basic behavior
  */
  void setRootAction(Behavior* behavior);

  /** Returns the selected root actions */
  const Action* getRootAction(int i) const;

  /** Returns the selected root actions */
  Action* getRootAction(int i);

  /** Returns the selected root actions */
  const Array<Action*>& getRootActions() const;

  /** Returns the selected root actions */
  Array<Action*>& getRootActions();

  /** Returns the name of the selected agent */
  const char* getSelectedAgentName() const;

  /** Prepare for processing incoming team messages. This resets previously processed messages. */
  void prepareIncomingMessages();

  /** Process an incoming team message from another agent */
  void processIncomingMessage(const TeamMessage& message);

  /** Generate the outgoing team message to be sent to other agents */
  void generateOutgoingMessage(TeamMessage& message);

  /** Set the number of execution cycles that is required for synchonization with other agents, i.e time from sending a message until answer is received */
  void setSynchronizationTicks(int ticks) {synchronizationTicks = ticks;}

  /** Sets the priority value of the agent, used for solving conflicts in cooperative state assignment */
  void setAgentPriority(int priority) {agentPriority = priority;}
  //!@}
};

} // namespace

#endif // __XabslEngine_h_

