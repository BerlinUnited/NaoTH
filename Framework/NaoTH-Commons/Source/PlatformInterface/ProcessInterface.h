/**
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Mellmann, Heinrich</a>
 */

#ifndef _ProcessInterface_h_
#define _ProcessInterface_h_

#include "Process.h"
#include "ActionList.h"

#include <typeinfo>

namespace naoth
{

/*
 *
 */
class ProcessInterface
{
public:
  ProcessInterface(Prosess& process, ProsessEnvironment& environment)
    : process(process), environment(environment)
  {

  }

  template<class T>
  bool registerInput(T& data)
  {
    bool result = registerAction(data, environment.inputActions, process.preActions);
    if(result)
      std::cout << "register input: " << typeid(T).name() << std::endl;
    else
      std::cerr << "invalid input: " << typeid(T).name() << std::endl;
    return result;
  }//end registerInput


  template<class T>
  bool registerOutput(const T& data)
  {
    bool result = registerAction(data, environment.outputActions, process.postActions);
    if(result)
      std::cout << "register output: " << typeid(T).name() << std::endl;
    else
      std::cerr << "invalid output: " << typeid(T).name() << std::endl;
    return result;
  }//end registerOutput


  template<class T>
  bool registerInputChanel(T& data) 
  {
    AbstractAction* action = environment.channelActionCreator.createInputChanelAction<T>(data);
    if(action != NULL) process.preActions.push_back(action);
    return (action != NULL);
  }

  template<class T, int maxSize>
  bool registerBufferedInputChanel(RingBuffer<T, maxSize>& buffer)
  {
    AbstractAction* action =
        environment.channelActionCreator.createBufferedInputChanelAction<T, maxSize>(buffer);
    if(action != NULL) process.preActions.push_back(action);
    return (action != NULL);
  }

  template<class T>
  bool registerOutputChanel(const T& data)
  { 
    AbstractAction* action = environment.channelActionCreator.createOutputChanelAction<T>(data);
    if(action != NULL) process.postActions.push_back(action); 
    return (action != NULL);
  }

private:

  template<class T>
  bool registerAction(T& data, const TypedActionCreatorMap& avaliableActions, ActionList& actionList)
  {
    AbstractAction* action = avaliableActions.createAction(data);
      
    // if an action could be created put it in the list
    if(action) actionList.push_back(action);

    return action != NULL;
  }//end registerCognitionInput


  Prosess& process;
  ProsessEnvironment& environment;
};// end ProsessInterface

}// namespace naoth

#endif  /* _ProcessInterface_h_ */

