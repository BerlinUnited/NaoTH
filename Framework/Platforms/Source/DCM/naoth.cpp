/**
 * @file naoth.cpp
 *
 * @author <a href="mailto:welter@informatik.hu-berlin.de">Oliver Welter</a>
 *
 */

#include <glib.h>
#include <glib-object.h>

#include "naoth.h"
#include "NaothModule.h"

using namespace std;
using namespace AL;

#define ALCALL

#ifdef __cplusplus
extern "C"
{
#endif

ALCALL int _createModule(ALPtr<ALBroker> pBroker )
{      
  g_type_init();
  
  // init broker with the main broker instance 
  // from the parent executable
  ALBrokerManager::setInstance(pBroker->fBrokerManager.lock());
  ALBrokerManager::getInstance()->addBroker(pBroker);
    
  // create modules instance. This will register automatically to the broker
  cout << "NaothModule is about to be created" << endl;
  ALModule::createModule<naoth::NaothModule>(pBroker,"NaothModule");
  
  return 0;
}

ALCALL int _closeModule(  )
{
  // Delete module instance. Will unregister automatically
//  delete lMyModule;
  return 0;
}

# ifdef __cplusplus
}
# endif




