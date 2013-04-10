/**
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Mellmann, Heinrich</a>
 */

#ifndef _Process_h_
#define _Process_h_

#include "Callable.h"
#include "ActionList.h"
#include "ChannelActionCreator.h"


namespace naoth
{

/*
 *
 */
class Prosess
{
public:
  Prosess() : callback(NULL)
  {
  }

  ~Prosess()
  {
    std::cout << "destruct Prosess" << std::endl;
  }

  Callable* callback;

  ActionList preActions;
  ActionList postActions;
};


/*
 *
 */
class ProsessEnvironment
{
public:
  ProsessEnvironment()
  {
  }

  ~ProsessEnvironment()
  {
    std::cout << "destruct ProsessEnvironment" << std::endl;
  }
  
  // registered (available) actions
  TypedActionCreatorMap inputActions;
  TypedActionCreatorMap outputActions;

  ChannelActionCreator channelActionCreator;
};

}//end namespace naoth

#endif // _Process_h_

