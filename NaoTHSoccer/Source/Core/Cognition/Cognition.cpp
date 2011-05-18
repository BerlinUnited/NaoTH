/**
 * @file Cognition.cpp
 *
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
 * Implementation of the class Cognition
 */

#include "Cognition.h"

#include <PlatformInterface/Platform.h>
#include <Tools/Debug/Stopwatch.h>
#include <Tools/Debug/DebugImageDrawings.h>

#include <glib.h>

/////////////////////////////////////
// Modules
/////////////////////////////////////
//Infrastructure
#include "Modules/Infrastructure/IO/Sensor.h"
#include "Modules/Infrastructure/IO/Actuator.h"
#include "Modules/Infrastructure/LEDSetter/LEDSetter.h"
#include "Modules/Infrastructure/Debug/Debug.h"
#include "Modules/Infrastructure/Debug/ParameterListDebugLoader.h"
#include "Modules/Infrastructure/Debug/StopwatchSender.h"
#include "Modules/Perception/VisualCortex/GridProvider.h"

#include "Core/Tools/Debug/DebugRequest.h"
#include <Tools/Debug/Stopwatch.h>

using namespace std;

Cognition::Cognition()
{
  
}

Cognition::~Cognition()
{
}

void Cognition::init(naoth::PlatformDataInterface& platformInterface)
{
  g_message("Cognition register start");

  // input
  ModuleCreator<Sensor>* sensor = registerModule<Sensor > ("Sensor");
  sensor->setEnabled(true);
  sensor->getModuleT()->init(platformInterface);

  // output
  ModuleCreator<Actuator>* actuator = registerModule<Actuator > ("Actuator");
  actuator->setEnabled(true);
  actuator->getModuleT()->init(platformInterface);

  // BEGIN MODULES
  // register of the modules
  registerModule<GridProvider>("GridProvider");  
  registerModule<LEDSetter > ("LEDSetter");  
  registerModule<Debug > ("Debug");
  registerModule<ParameterListDebugLoader> ("ParameterListDebugLoader");
  registerModule<StopwatchSender>("StopwatchSender");
  
  //END MODULES
  
  packageLoader.loadPackages("Packages/", *this);
  
  // use the configuration in order to set whether a module is activated or not
  const naoth::Configuration& config = Platform::getInstance().theConfiguration;
  
  for(list<string>::const_iterator name=getExecutionList().begin();
    name != getExecutionList().end(); name++)
  {
    bool active = false;
    if(config.hasKey("modules", *name))
    {    
      active = config.getBool("modules", *name);      
    }
    setModuleEnabled(*name, active);
    if(active)
    {
      g_message("activating module %s", (*name).c_str());
    }
  }
  
  // auto-generate the execution list
  calculateExecutionList();

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
      std::string name(*iter);
      //g_debug("executing %s", name.c_str());
      STOPWATCH_START(name);
      module->execute();
      STOPWATCH_STOP(name);
    }//end if
  }//end for all modules
  
  // HACK: reset all the debug stuff before executing the modules
  DebugImageDrawings::getInstance().reset();
  
}//end call



