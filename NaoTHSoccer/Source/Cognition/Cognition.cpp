/**
 * @file Cognition.cpp
 *
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
 * Implementation of the class Cognition
 */

#include "Cognition.h"

#include <PlatformInterface/Platform.h>

// debug
#include "Tools/Debug/DebugBufferedOutput.h"
#include "Tools/Debug/DebugDrawings.h"
#include "Tools/Debug/DebugImageDrawings.h"
#include "Tools/Debug/DebugDrawings3D.h"
#include <Representations/Debug/Stopwatch.h>

/////////////////////////////////////
// Modules
/////////////////////////////////////

// infrastructure
#include "Modules/Infrastructure/IO/Sensor.h"
#include "Modules/Infrastructure/IO/Actuator.h"
#include "Modules/Infrastructure/ButtonEventMonitor/ButtonEventMonitor.h"
#include "Modules/Infrastructure/BatteryAlert/BatteryAlert.h"
#include "Modules/Infrastructure/Debug/FrameRateCheck.h"
#include "Modules/Infrastructure/Debug/DebugExecutor.h"
#include "Modules/Infrastructure/Debug/Debug.h"

// tools
#include "Tools/Debug/Trace.h"

using namespace std;

Cognition::Cognition()
  : ModuleManagerWithDebug("Cognition")
{
}

Cognition::~Cognition()
{
}


#define REGISTER_MODULE(module) \
  std::cout << "[Cognition] Register " << #module << std::endl;\
  registerModule<module>(std::string(#module))


void Cognition::init(naoth::ProcessInterface& platformInterface, const naoth::PlatformBase& platform)
{
  std::cout << "[Cognition] Cognition register start" << std::endl;

  // register input module
  ModuleCreator<Sensor>* sensor = registerModule<Sensor>(std::string("Sensor"), true);
  sensor->getModuleT()->init(platformInterface, platform);

  /* 
  * to register a module use
  *   REGISTER_MODULE(ModuleClassName);
  *
  * Remark: to enable the module don't forget 
  *         to set the value in modules.cfg
  */

  // -- BEGIN REGISTER MODULES --

  // infrastructure
//  REGISTER_MODULE(TeamCommReceiver);
//  REGISTER_MODULE(GameController);
//  REGISTER_MODULE(OpenCVGrayScaleImageProvider);
//  REGISTER_MODULE(OpenCVImageProvider);
  REGISTER_MODULE(BatteryAlert);
  REGISTER_MODULE(ButtonEventMonitor);

  // debug
  REGISTER_MODULE(Debug);
  REGISTER_MODULE(FrameRateCheck);
  REGISTER_MODULE(DebugExecutor);

  // -- END REGISTER MODULES --

  // register output module
  ModuleCreator<Actuator>* actuator = registerModule<Actuator>(std::string("Actuator"), true);
  actuator->getModuleT()->init(platformInterface, platform);

  // use the configuration in order to set whether a module is activated or not
  const naoth::Configuration& config = Platform::getInstance().theConfiguration;
  
  list<string>::const_iterator name = getExecutionList().begin();
  for(;name != getExecutionList().end(); ++name)
  {
    bool active = false;
    if(config.hasKey("modules", *name)) {    
      active = config.getBool("modules", *name);      
    }
    if(active) {
      std::cout << "[Cognition] activating module " << *name << std::endl;
    }
    setModuleEnabled(*name, active);
  }//end for

  // auto-generate the execution list
  //calculateExecutionList();

  std::cout << "[Cognition] register end" << std::endl;

  stopwatch.start();
}//end init


void Cognition::call()
{
  // BEGIN cognition frame rate measuring
  stopwatch.stop();
  stopwatch.start();
  PLOT("_CognitionCycle", stopwatch.lastValue);
  // END cognition frame rate measuring


  STOPWATCH_START("CognitionExecute");

  // execute all modules
  list<AbstractModuleCreator*>::const_iterator iter;
  for (iter = getModuleExecutionList().begin(); iter != getModuleExecutionList().end(); ++iter)
  {
    AbstractModuleCreator* module = *iter;
    if (module != NULL && module->isEnabled())
    {
      std::string name(module->getModule()->getName());
      GT_TRACE("executing " << name);
      module->execute();
    }
  }
  
  STOPWATCH_STOP("CognitionExecute");


  // HACK: reset all the debug stuff before executing the modules
  STOPWATCH_START("Debug ~ Init");
  DebugBufferedOutput::getInstance().update();
  DebugDrawings::getInstance().update();
  DebugImageDrawings::getInstance().reset();
  DebugDrawings3D::getInstance().update();
  STOPWATCH_STOP("Debug ~ Init");
}//end call
