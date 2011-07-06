/**
 * @file Cognition.cpp
 *
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
 * Implementation of the class Cognition
 */

#include "Cognition.h"

#include <PlatformInterface/Platform.h>

#include <Tools/Debug/DebugImageDrawings.h>
#include "Tools/Debug/DebugDrawings.h"
#include "Tools/Debug/DebugBufferedOutput.h"
#include "Tools/Debug/DebugDrawings3D.h"
#include <Tools/Debug/Stopwatch.h>
#include "Tools/Debug/DebugRequest.h"
#include "Tools/NaoTime.h"


// list the modules and representations on the blackboard
#include "DebugCommandServer.h"
#include "Messages/Messages.pb.h"
#include <google/protobuf/io/zero_copy_stream_impl.h>


#include <glib.h>

/////////////////////////////////////
// Modules
/////////////////////////////////////

// Infrastructure
#include "Modules/Infrastructure/IO/Sensor.h"
#include "Modules/Infrastructure/IO/Actuator.h"
#include "Modules/Infrastructure/LEDSetter/LEDSetter.h"
#include "Modules/Infrastructure/Debug/Debug.h"
#include "Modules/Infrastructure/Debug/DebugExecutor.h"
#include "Modules/Infrastructure/Debug/StopwatchSender.h"
#include "Modules/Infrastructure/Debug/RoboViz.h"
#include "Modules/Infrastructure/Debug/CameraDebug.h"
#include "Modules/Infrastructure/TeamCommunicator/TeamCommSender.h"
#include "Modules/Infrastructure/TeamCommunicator/TeamCommReceiver.h"
#include "Modules/Infrastructure/GameController/GameController.h"

// Perception
#include "Modules/Perception/CameraMatrixProvider/CameraMatrixProvider.h"
#include "Modules/Perception/VisualCortex/FieldColorClassifier.h"
#include "Modules/Perception/VisualCortex/ColorProvider.h"
#include "Modules/Perception/VisualCortex/GridProvider.h"
#include "Modules/Perception/VisualCortex/ImageProcessor.h"
#include "Modules/Perception/VirtualVisionProcessor/VirtualVisionProcessor.h"
#include "Modules/Perception/PerceptProjector/PerceptProjector.h"
#include "Modules/Perception/PerceptionsVisualization/PerceptionsVisualization.h"

// Modeling
#include "Modules/Modeling/BodyStateProvider/BodyStateProvider.h"
#include "Modules/Modeling/BallLocator/ParticleFilterBallLocator/ParticleFilterBallLocator.h"
#include "Modules/Modeling/BallLocator/KalmanFilterBallLocator/KalmanFilterBallLocator.h"
#include "Modules/Modeling/BallLocator/TeamBallLocator/TeamBallLocator.h"
#include "Modules/Modeling/GoalLocator/ActiveGoalLocator/ActiveGoalLocator.h"
#include "Modules/Modeling/GoalLocator/WholeGoalLocator/WholeGoalLocator.h"
#include "Modules/Modeling/SelfLocator/GPS_SelfLocator/GPS_SelfLocator.h"
#include "Modules/Modeling/SelfLocator/OdometrySelfLocator/OdometrySelfLocator.h"
#include "Modules/Modeling/ObstacleLocator/VisualObstacleLocator.h"
#include "Modules/Modeling/SelfLocator/MonteCarloSelfLocator/MonteCarloSelfLocator.h"
#include "Modules/Modeling/SoccerStrategyProvider/SoccerStrategyProvider.h"

#include "Modules/Modeling/PotentialFieldProvider/PotentialFieldProvider.h"
#include "Modules/Modeling/AttentionAnalyzer/AttentionAnalyzer.h"

// Behavior
#include "Modules/BehaviorControl/SensorBehaviorControl/SensorBehaviorControl.h"
#include "Modules/BehaviorControl/SimpleMotionBehaviorControl/SimpleMotionBehaviorControl.h"
#include "Modules/BehaviorControl/XABSLBehaviorControl/XABSLBehaviorControl.h"

// Experiment
#include "Modules/Experiment/Evolution/Evolution.h"


// tools
#include "Tools/NaoTime.h"

using namespace std;

Cognition::Cognition()
{
  REGISTER_DEBUG_COMMAND("modules:list",
    "return the list of registered modules with provided and required representations", this);
  REGISTER_DEBUG_COMMAND("modules:set",
    "enables or diables the execution of a module. usage: modules:set [moduleName=on|off]+", this);
  REGISTER_DEBUG_COMMAND("modules:store",
    "store te corrent module configuration to the config file listed in the scheme", this);


  REGISTER_DEBUG_COMMAND("representation:list", "Stream out the list of all registered representations", this);
  REGISTER_DEBUG_COMMAND("representation:get", "Stream out all the representations listet", this);
  REGISTER_DEBUG_COMMAND("representation:getbinary", "Stream out serialized represenation", this);
}

Cognition::~Cognition()
{
}


#define REGISTER_MODULE(module) \
  registerModule<module>(std::string(#module));


void Cognition::init(naoth::PlatformInterfaceBase& platformInterface)
{
  g_message("Cognition register start");

  // register of the modules

  // input
  ModuleCreator<Sensor>* sensor = REGISTER_MODULE(Sensor);
  sensor->setEnabled(true);
  sensor->getModuleT()->init(platformInterface);

  /* 
   * to register a module use
   *   REGISTER_MODULE(ModuleClassName);
   *
   * Remark: to enable the module don't forget 
   *         to set the value in modules.cfg
   */

  // -- BEGIN MODULES --

  // infrastructure
  REGISTER_MODULE(TeamCommReceiver);
  REGISTER_MODULE(GameController);

  // perception
  REGISTER_MODULE(CameraMatrixProvider);
  REGISTER_MODULE(FieldColorClassifier);
  REGISTER_MODULE(ColorProvider);
  REGISTER_MODULE(GridProvider);
  REGISTER_MODULE(ImageProcessor);
  REGISTER_MODULE(VirtualVisionProcessor);
  REGISTER_MODULE(PerceptProjector);
  REGISTER_MODULE(PerceptionsVisualization);

  // modeling
  REGISTER_MODULE(BodyStateProvider);
  REGISTER_MODULE(ParticleFilterBallLocator);
  REGISTER_MODULE(KalmanFilterBallLocator);
  REGISTER_MODULE(ActiveGoalLocator);
  REGISTER_MODULE(WholeGoalLocator);
  REGISTER_MODULE(GPS_SelfLocator);
  REGISTER_MODULE(OdometrySelfLocator);
  REGISTER_MODULE(VisualObstacleLocator);
  REGISTER_MODULE(MonteCarloSelfLocator);
  REGISTER_MODULE(TeamBallLocator);
  REGISTER_MODULE(SoccerStrategyProvider);

  REGISTER_MODULE(PotentialFieldProvider);
  REGISTER_MODULE(AttentionAnalyzer);

  // behavior
  REGISTER_MODULE(SensorBehaviorControl);
  REGISTER_MODULE(SimpleMotionBehaviorControl);
  REGISTER_MODULE(XABSLBehaviorControl);

  // experiment
  REGISTER_MODULE(Evolution);

  // infrastructure
  REGISTER_MODULE(TeamCommSender);

  // debug
  REGISTER_MODULE(LEDSetter);
  REGISTER_MODULE(Debug);
  REGISTER_MODULE(RoboViz);
  REGISTER_MODULE(StopwatchSender);
  REGISTER_MODULE(DebugExecutor);
  REGISTER_MODULE(CameraDebug);

  // -- END MODULES --

  // output
  ModuleCreator<Actuator>* actuator = REGISTER_MODULE(Actuator);
  actuator->setEnabled(true);
  actuator->getModuleT()->init(platformInterface);


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

  g_message("Cognition register end");

  Stopwatch::getInstance().notifyStart(stopwatch);
}//end init


void Cognition::call()
{
  // BEGIN cognition frame rate measuring
  Stopwatch::getInstance().notifyStop(stopwatch);
  Stopwatch::getInstance().notifyStart(stopwatch);
  PLOT("_CognitionCycle", stopwatch.lastValue);
  // END cognition frame rate measuring


  STOPWATCH_START("CognitionExecute");

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
      STOPWATCH_START_GENERIC(name);
      module->execute();
      STOPWATCH_START_GENERIC(name);
    }//end if
  }//end for all modules
  

  STOPWATCH_STOP("CognitionExecute");


  // HACK: reset all the debug stuff before executing the modules
  STOPWATCH_START("Debug ~ Init");
  DebugBufferedOutput::getInstance().update();
  DebugDrawings::getInstance().update();
  DebugImageDrawings::getInstance().reset();
  DebugDrawings3D::getInstance().update();
  STOPWATCH_STOP("Debug ~ Init");
}//end call



void Cognition::executeDebugCommand(const std::string& command, 
                                    const std::map<std::string,std::string>& arguments, 
                                    std::ostream& outstream)
{
  if (command == "modules:list")
  {
    naothmessages::ModuleList msg;
    
    list<string>::const_iterator iterModule;
    for (iterModule = getExecutionList().begin(); iterModule != getExecutionList().end(); ++iterModule)
    {
      naothmessages::Module* m = msg.add_modules();

      // get the module holder
      AbstractModuleCreator* moduleCreator = getModule(*iterModule);
      ASSERT(moduleCreator != NULL); // shold never happen

      // module name
      m->set_name(*iterModule);
      m->set_active(moduleCreator->isEnabled());
      
      if(moduleCreator->isEnabled())
      {
        Module* module = moduleCreator->getModule();
        ASSERT(module != NULL); // shold never happen


        std::list<Representation*>::const_iterator iterRep;
        for (iterRep = module->getRequiredRepresentations().begin();
          iterRep != module->getRequiredRepresentations().end(); iterRep++)
        {
          m->add_usedrepresentations((**iterRep).getName());
        }

        for (iterRep = module->getProvidedRepresentations().begin();
          iterRep != module->getProvidedRepresentations().end(); iterRep++)
        {
          m->add_providedrepresentations((**iterRep).getName());
        }
      }//end if
    }//end for iterModule

    google::protobuf::io::OstreamOutputStream buf(&outstream);
    msg.SerializeToZeroCopyStream(&buf);
  }
  else if( command == "modules:store" )
  {
    naoth::Configuration& config = Platform::getInstance().theConfiguration;
    for(list<string>::const_iterator name=getExecutionList().begin();
      name != getExecutionList().end(); name++)
    {
      config.setBool("modules", *name, getModule(*name)->isEnabled());
    }//end for

    // write the config to file
    config.save();
    outstream << "modules saved to private/modules.cfg" << endl;
  }
  else if (command == "modules:set")
  {
    std::map<std::string, std::string>::const_iterator iter;
    for (iter = arguments.begin(); iter != arguments.end(); ++iter)
    {
      AbstractModuleCreator* moduleCreator = getModule(iter->first);
      if (moduleCreator != NULL)
      {
        if ((iter->second).compare("on") == 0)
        {
          moduleCreator->setEnabled(true);
          outstream << "set " << (iter->first) << " on" << endl;
        }
        else if ((iter->second).compare("off") == 0)
        {
          moduleCreator->setEnabled(false);
          outstream << "set " << (iter->first) << " off" << endl;
        }
        else
        {
          outstream << "unknown value " << (iter->second) << endl;
        }
      }
      else
      {
        outstream << "unknown module " << (iter->first) << endl;
      }
    }//end for
  }
  else if (command == "representation:list")
  {
    printRepresentationList(outstream);
  }
  else if (command == "representation:get")
  {
    std::map<std::string, std::string>::const_iterator iter;
    for (iter = arguments.begin(); iter != arguments.end(); ++iter)
    {
      printRepresentation(outstream, iter->first, false);
    }//end for
  }
  else if (command == "representation:getbinary")
  {
    std::map<std::string, std::string>::const_iterator iter;
    for (iter = arguments.begin(); iter != arguments.end(); ++iter)
    {
      printRepresentation(outstream, iter->first, true);
    }
  }
}//end executeDebugCommand

