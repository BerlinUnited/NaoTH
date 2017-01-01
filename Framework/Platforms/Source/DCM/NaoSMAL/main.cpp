/**
 * @file libnaoth.cpp
 *
 * @author <a href="mailto:welter@informatik.hu-berlin.de">Oliver Welter</a>
 *
 */

#include <glib.h>
#include <glib-object.h>


#include "alincludes.h"

#include "SMALModule.h"

extern "C"
{

int _createModule(boost::shared_ptr<AL::ALBroker> broker)
{      
  g_type_init();

  // init broker with the main broker instance
  // from the parent executable
  AL::ALBrokerManager::setInstance(broker->fBrokerManager.lock());
  AL::ALBrokerManager::getInstance()->addBroker(broker);
    
  // create modules instance. This will register automatically to the broker
  std::cout << "NaoSMAL is about to be created" << std::endl;
  AL::ALModule::createModule<naoth::SMALModule>(broker,"NaoSMAL");
  
  return 0;
}

int _closeModule(  )
{
  // Delete module instance. Will unregister automatically
  return 0;
}

}//end extern "C"
