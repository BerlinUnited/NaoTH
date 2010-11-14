/**
 * @file Cognition.cpp
 *
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
 * Implementation of the class Cognition
 */

#include "Cognition.h"

#include <PlatformInterface/Platform.h>

/////////////////////////////////////
// Modules
/////////////////////////////////////
//Infrastructure
#include "Modules/Infrastructure/IO/Sensor.h"
#include "Modules/Infrastructure/IO/Actuator.h"
#include "Modules/Infrastructure/LEDSetter/LEDSetter.h"
#include "Modules/Infrastructure/Debug/Debug.h"

using namespace std;

Cognition::Cognition()
{
  registerModule<LEDSetter > ("LEDSetter");
  registerModule<Debug > ("Debug");

  // use the configuration in order to set whether a module is activated or not
  Configuration& config = Platform::getInstance().theConfiguration;
  for(list<string>::const_iterator name=getExecutionList().begin();
    name != getExecutionList().end(); name++)
  {
    if(config.hasKey("modules", *name))
    {
      bool active = config.getBool("modules", *name);
      setModuleEnabled(*name, active);
      if(active)
      {
        g_message("activating module %s", (*name).c_str());
      }
    }
  }
}

Cognition::~Cognition()
{
}

void Cognition::init(naoth::PlatformDataInterface& platformInterface)
{
  g_debug("test configuration: %s",
    Platform::getInstance().theConfiguration.getString("test", "test").c_str());

  g_message("Cognition register start");

  // input
  ModuleCreator<Sensor>* sensor = registerModule<Sensor > ("Sensor");
  sensor->setEnabled(true);
  sensor->getModuleT()->init(platformInterface);

  // output
  ModuleCreator<Actuator>* actuator = registerModule<Actuator > ("Actuator");
  actuator->setEnabled(true);
  actuator->getModuleT()->init(platformInterface);

  g_message("Cognition register end");
}//end init

void Cognition::call()
{
  // execute all modules
  list<string>::const_iterator iter;
  for (iter = getExecutionList().begin(); iter != getExecutionList().end(); iter++)
  {
    // get entry
    AbstractModuleCreator* module = getModule(*iter);
    if (module != NULL && module->isEnabled())
    {
      module->execute();
    }//end if
  }//end for
}//end call


