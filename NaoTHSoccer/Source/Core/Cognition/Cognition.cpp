/**
 * @file Cognition.cpp
 *
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
 * Implementation of the class Cognition
 */

#include "Cognition.h"


#include "Tools/Debug/DebugDrawings3D.h"



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
#include "Modules/Infrastructure/Debug/FrameRateCheck.h"
#include "Modules/Infrastructure/TeamCommunicator/TeamCommSender.h"
#include "Modules/Infrastructure/TeamCommunicator/TeamCommReceiver.h"
#include "Modules/Infrastructure/TeamCommunicator/RCTCHandler.h"
#include "Modules/Infrastructure/GameController/GameController.h"
#include "Modules/Infrastructure/OpenCV/OpenCVImageProvider.h"
#include "Modules/Infrastructure/BatteryAlert/BatteryAlert.h"
#include "Modules/Infrastructure/Camera/CameraInfoSetter.h"

// Perception
#include "Modules/Perception/KinematicChainProvider/KinematicChainProvider.h"
#include "Modules/Perception/CameraMatrixCorrector/CameraMatrixCorrector.h"
#include "Modules/Perception/VisualCortex/ImageCorrector.h"
#include "Modules/Perception/VisualCortex/BaseColorClassifier.h"
#include "Modules/Perception/VisualCortex/FieldColorClassifier.h"
#include "Modules/Perception/VisualCortex/ColorProvider.h"
#include "Modules/Perception/VisualCortex/GridProvider.h"
#include "Modules/Perception/VisualCortex/ImageProcessor.h"
#include "Modules/Perception/VirtualVisionProcessor/VirtualVisionProcessor.h"
#include "Modules/Perception/PerceptProjector/PerceptProjector.h"
#include "Modules/Perception/PerceptionsVisualization/PerceptionsVisualization.h"
#include "Modules/Perception/OpenCV/FieldSideDetector.h"
#include "Modules/Perception/OpenCV/OpenCVDebug.h"
#include "Modules/Perception/ArtificialHorizonCalculator/ArtificialHorizonCalculator.h"

// neo vision
#include "Modules/Perception/NeoVision/NeoVision.h"


// Modeling
#include "Modules/Modeling/BodyStateProvider/BodyStateProvider.h"
#include "Modules/Modeling/BallLocator/ParticleFilterBallLocator/ParticleFilterBallLocator.h"
#include "Modules/Modeling/BallLocator/KalmanFilterBallLocator/KalmanFilterBallLocator.h"
#include "Modules/Modeling/BallLocator/TeamBallLocator/TeamBallLocator.h"
#include "Modules/Modeling/GoalLocator/ActiveGoalLocator/ActiveGoalLocator.h"
#include "Modules/Modeling/GoalLocator/ActiveGoalLocator/CompareGoalModels.h"
#include "Modules/Modeling/GoalLocator/WholeGoalLocator/WholeGoalLocator.h"
#include "Modules/Modeling/GoalLocator/DummyActiveGoalLocator/DummyActiveGoalLocator.h"
#include "Modules/Modeling/GoalLocator/TestPostParticleFilter/TestPostParticleFilter.h"
#include "Modules/Modeling/SelfLocator/GPS_SelfLocator/GPS_SelfLocator.h"
#include "Modules/Modeling/SelfLocator/OdometrySelfLocator/OdometrySelfLocator.h"
#include "Modules/Modeling/ObstacleLocator/UltraSoundObstacleLocator.h"
#include "Modules/Modeling/ObstacleLocator/VisualObstacleLocator.h"
#include "Modules/Modeling/SelfLocator/MonteCarloSelfLocator/MonteCarloSelfLocator.h"
#include "Modules/Modeling/FieldCompass/FieldCompass.h"
#include "Modules/Modeling/SoccerStrategyProvider/SoccerStrategyProvider.h"
#include "Modules/Modeling/PlayersLocator/PlayersLocator.h"
#include "Modules/Modeling/PotentialFieldProvider/PotentialFieldProvider.h"
#include "Modules/Modeling/AttentionAnalyzer/AttentionAnalyzer.h"
#include "Modules/Modeling/PathPlanner/PathPlanner.h"
#include "Modules/Modeling/CollisionDetector/CollisionDetector.h"
#include "Modules/Modeling/Camera/CameraMatrixFinder.h"

// Behavior
#include "Modules/BehaviorControl/SensorBehaviorControl/SensorBehaviorControl.h"
#include "Modules/BehaviorControl/SimpleMotionBehaviorControl/SimpleMotionBehaviorControl.h"
#include "Modules/BehaviorControl/XABSLBehaviorControl2011/XABSLBehaviorControl2011.h"
#include "Modules/BehaviorControl/XABSLBehaviorControl2012/XABSLBehaviorControl2012.h"
#include "Modules/BehaviorControl/XABSLBehaviorControl/XABSLBehaviorControl.h"
#include "Modules/BehaviorControl/CalibrationBehaviorControl/CalibrationBehaviorControl.h"
#include "Modules/BehaviorControl/GraspingBehaviorControl/GraspingBehaviorControl.h"
#include "Modules/BehaviorControl/ArmMotionBehaviorControl/ArmMotionBehaviorControl.h"

// Experiment
#include "Modules/Experiment/Evolution/Evolution.h"

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
  // register of the modules

  // input module
  ModuleCreator<Sensor>* sensor = registerModule<Sensor>(std::string("Sensor"), true);
  sensor->getModuleT()->init(platformInterface, platform);

  /* 
   * to register a module use
   *   REGISTER_MODULE(ModuleClassName);
   *
   * Remark: to enable the module don't forget 
   *         to set the value in modules.cfg
   */

  // -- BEGIN MODULES --

  // infrastructure
  REGISTER_MODULE(RCTCHandler);
  REGISTER_MODULE(TeamCommReceiver);
  REGISTER_MODULE(GameController);
  REGISTER_MODULE(OpenCVImageProvider);
  REGISTER_MODULE(BatteryAlert);
  REGISTER_MODULE(CameraInfoSetter);

  // pre-modelling
  REGISTER_MODULE(CameraMatrixFinder);

  // perception
  REGISTER_MODULE(KinematicChainProvider);
  REGISTER_MODULE(CameraMatrixCorrector);
  REGISTER_MODULE(ArtificialHorizonCalculator);
  REGISTER_MODULE(ImageCorrector);
  REGISTER_MODULE(FieldColorClassifier);
  REGISTER_MODULE(BaseColorClassifier);
  REGISTER_MODULE(ColorProvider);
  REGISTER_MODULE(GridProvider);
  REGISTER_MODULE(ImageProcessor);
  REGISTER_MODULE(VirtualVisionProcessor);
  REGISTER_MODULE(FieldSideDetector);
  REGISTER_MODULE(OpenCVDebug);
  // neo vision
  REGISTER_MODULE(NeoVision);


  // scene analysers 
  // (analyze the visual information seen in the image)
  REGISTER_MODULE(WholeGoalLocator);
  REGISTER_MODULE(PerceptProjector);
  REGISTER_MODULE(PerceptionsVisualization);


  // modeling
  REGISTER_MODULE(BodyStateProvider);
  REGISTER_MODULE(ParticleFilterBallLocator);
  REGISTER_MODULE(KalmanFilterBallLocator);
  REGISTER_MODULE(ActiveGoalLocator);
  REGISTER_MODULE(CompareGoalModels);
  REGISTER_MODULE(TestPostParticleFilter);
  REGISTER_MODULE(GPS_SelfLocator);
  REGISTER_MODULE(OdometrySelfLocator);
  REGISTER_MODULE(UltraSoundObstacleLocator);
  REGISTER_MODULE(VisualObstacleLocator);
  REGISTER_MODULE(MonteCarloSelfLocator);
  REGISTER_MODULE(DummyActiveGoalLocator); // has to be after MonteCarloSelfLocator
  REGISTER_MODULE(FieldCompass);
  REGISTER_MODULE(TeamBallLocator);
  REGISTER_MODULE(PlayersLocator);
  REGISTER_MODULE(PotentialFieldProvider);
  REGISTER_MODULE(AttentionAnalyzer);
  REGISTER_MODULE(SoccerStrategyProvider);
  REGISTER_MODULE(PathPlanner);
  REGISTER_MODULE(CollisionDetector);

  // behavior
  REGISTER_MODULE(SensorBehaviorControl);
  REGISTER_MODULE(SimpleMotionBehaviorControl);
  REGISTER_MODULE(CalibrationBehaviorControl);
  REGISTER_MODULE(XABSLBehaviorControl2011);
  REGISTER_MODULE(XABSLBehaviorControl2012);
  REGISTER_MODULE(XABSLBehaviorControl);
  REGISTER_MODULE(GraspingBehaviorControl);
  REGISTER_MODULE(ArmMotionBehaviorControl);

  // experiment
  REGISTER_MODULE(Evolution);

  // infrastructure
  REGISTER_MODULE(TeamCommSender);

  // debug
  REGISTER_MODULE(FrameRateCheck);
  REGISTER_MODULE(LEDSetter);
  REGISTER_MODULE(Debug);
  REGISTER_MODULE(RoboViz);
  REGISTER_MODULE(StopwatchSender);
  REGISTER_MODULE(DebugExecutor);
  REGISTER_MODULE(CameraDebug);

  // -- END MODULES --

  // output module
  ModuleCreator<Actuator>* actuator = registerModule<Actuator>(std::string("Actuator"), true);
  actuator->getModuleT()->init(platformInterface, platform);

  // loat external modules
  //packageLoader.loadPackages("Packages/", *this);
  

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
  DebugDrawings3D::getInstance().update();

  DebugBottomImageDrawings::getInstance().reset();
  DebugTopImageDrawings::getInstance().reset();
  STOPWATCH_STOP("Debug ~ Init");
}//end call
