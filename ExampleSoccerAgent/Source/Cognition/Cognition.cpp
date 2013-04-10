/**
 * @file Cognition.cpp
 *
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
 * Implementation of the class Cognition
 */

#include "Cognition.h"

#include <PlatformInterface/Platform.h>

#include <Tools/Debug/Stopwatch.h>
#include <Tools/Debug/Trace.h>
#include "Tools/Debug/DebugRequest.h"
#include "Tools/NaoTime.h"


// list the modules and representations on the blackboard
#include <DebugCommunication/DebugCommandManager.h>
#include "Messages/Messages.pb.h"
#include <google/protobuf/io/zero_copy_stream_impl.h>

/////////////////////////////////////
// Modules
/////////////////////////////////////

// Infrastructure
#include "Infrastructure/IO/Sensor.h"
#include "Infrastructure/IO/Actuator.h"
#include "Infrastructure/Debug/Debug.h"

// Perception
#include "Perception/VirtualVisionProcessor/VirtualVisionProcessor.h"

// Modeling
#include "Modeling/SelfLocator/OdometrySelfLocator/OdometrySelfLocator.h"

// Behavior
#include "BehaviorControl/SensorBehaviorControl/SensorBehaviorControl.h"

// tools
#include "Tools/NaoTime.h"

using namespace std;

Cognition::Cognition()
{
  
}

Cognition::~Cognition()
{
}


#define REGISTER_MODULE(module) \
  registerModule<module>(std::string(#module));


void Cognition::init(naoth::ProcessInterface& platformInterface, const naoth::PlatformBase& platform)
{
  // register of the modules

  // input
  ModuleCreator<Sensor>* sensor = REGISTER_MODULE(Sensor);
  sensor->setEnabled(true);
  sensor->getModuleT()->init(platformInterface, platform);

  /* 
   * to register a module use
   *   REGISTER_MODULE(ModuleClassName);
   *
   * Remark: to enable the module don't forget 
   *         to set the value in modules.cfg
   */

  // -- BEGIN MODULES --

  // perception
  REGISTER_MODULE(VirtualVisionProcessor);

  // modeling
  REGISTER_MODULE(OdometrySelfLocator);

  // behavior
  REGISTER_MODULE(SensorBehaviorControl);

  // debug
  REGISTER_MODULE(Debug);

  // -- END MODULES --

  // output
  ModuleCreator<Actuator>* actuator = REGISTER_MODULE(Actuator);
  actuator->setEnabled(true);
  actuator->getModuleT()->init(platformInterface, platform);


  // loat external modules
  //packageLoader.loadPackages("Packages/", *this);
  

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
  }//end for
  
  // auto-generate the execution list
  //calculateExecutionList();
}//end init


void Cognition::call()
{
  STOPWATCH_START("CognitionExecute");


  GT_TRACE("beginning to iterate over all modules");
  // execute all modules
  list<string>::const_iterator iter;
  for (iter = getExecutionList().begin(); iter != getExecutionList().end(); iter++)
  {
    // get entry
    AbstractModuleCreator* module = getModule(*iter);
    if (module != NULL && module->isEnabled())
    {
      std::string name(*iter);

      stringstream s;
      s << "executing " << name;
      Trace::getInstance().setCurrentLine(__FILE__, __LINE__, s.str());
      STOPWATCH_START_GENERIC(name);
      module->execute();
      STOPWATCH_STOP_GENERIC(name);
    }//end if
  }//end for all modules
  
  GT_TRACE("end module iteration");
  STOPWATCH_STOP("CognitionExecute");
}//end call

void Cognition::executeDebugCommand(const std::string &command,
                                    const std::map<std::string,
                                    std::string> &arguments,
                                    std::ostream &outstream)
{

}
