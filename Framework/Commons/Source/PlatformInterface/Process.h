/**
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Mellmann, Heinrich</a>
 */

#ifndef _Process_h_
#define _Process_h_

#include "Callable.h"
#include "ActionList.h"
#include "ChannelActionCreator.h"

#undef PRINT_DEBUG
#ifdef DEBUG_PLATFORM
#  define PRINT_DEBUG(m) std::err << m << std::endl
#else
#  define PRINT_DEBUG(m) ((void)0)
#endif

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
    PRINT_DEBUG("destruct Prosess");
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
    PRINT_DEBUG("destruct ProsessEnvironment");
  }
  
  // registered (available) actions
  TypedActionCreatorMap inputActions;
  TypedActionCreatorMap outputActions;

  ChannelActionCreator channelActionCreator;
};

}//end namespace naoth

#endif // _Process_h_

