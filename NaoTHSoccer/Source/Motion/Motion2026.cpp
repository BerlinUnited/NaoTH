/**
 * @file Motion.cpp
 *
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Mellmann, Heinrich</a>
 *
 */

#include "Motion2026.h"

 // needed for sleep_for in debug kill 
#include "Tools/ThreadUtil.h"

using namespace naoth;

Motion2026::Motion2026()
  : ModuleManagerWithDebug(""),
    theLogProvider(NULL),
    motionLogger("Motion2026Log")
{
  REGISTER_DEBUG_COMMAND(motionLogger.getCommand(), motionLogger.getDescription(), &motionLogger);
  registerLogableRepresentationList();

  getDebugParameterList().add(&parameter);
}

Motion2026::~Motion2026()
{
  getDebugParameterList().remove(&parameter);
}

void Motion2026::init(naoth::ProcessInterface& platformInterface, const naoth::PlatformBase& platform)
{
  // try to get the log provider
  theLogProvider = ModuleManager::getModule("LogProvider");

  // copy robot info to the blackboard
  // copy and provide RobotInfo
  getRobotInfo().platform       = platform.getPlatformName();
  //
  getRobotInfo().robotName      = platform.getRobotName();
  getRobotInfo().headId         = platform.getHeadId();
  getRobotInfo().bodyNickName   = platform.getBodyNickName();
  getRobotInfo().bodyId         = platform.getBodyId();
  //
  getRobotInfo().basicTimeStep  = platform.getBasicTimeStep();


  std::cout << "[Motion] register begin" << std::endl;
#define REG_INPUT(R)                                                    \
  platformInterface.registerInput(get##R())

  REG_INPUT(SensorJointData);
  REG_INPUT(FrameInfo);
  REG_INPUT(InertialSensorData);
  REG_INPUT(FSRData);
  REG_INPUT(AccelerometerData);
  REG_INPUT(GyrometerData);
  REG_INPUT(ButtonData);

  REG_INPUT(DebugMessageInMotion);

#define REG_OUTPUT(R)                                                   \
  platformInterface.registerOutput(get##R())

  REG_OUTPUT(MotorJointData);
  REG_OUTPUT(DebugMessageOut);
  //REG_OUTPUT(LEDData);

  // messages from motion to cognition
  platformInterface.registerOutputChanel(getCameraMatrix());
  platformInterface.registerOutputChanel(getCameraMatrixTop());
  platformInterface.registerOutputChanel(getMotionStatus());
  platformInterface.registerOutputChanel(getOdometryData());
  //platformInterface.registerOutputChanel(getCalibrationData());
  platformInterface.registerOutputChanel(getInertialModel());
  platformInterface.registerOutputChanel(getBodyStatus());
  platformInterface.registerOutputChanel(getGroundContactModel());
  platformInterface.registerOutputChanel(getCollisionPercept());
  platformInterface.registerOutputChanel(getIMUData());

  // messages from cognition to motion
  platformInterface.registerInputChanel(getCameraInfo());
  platformInterface.registerInputChanel(getCameraInfoTop());
  platformInterface.registerInputChanel(getCameraMatrixOffset());
  platformInterface.registerInputChanel(getHeadMotionRequest());
  platformInterface.registerInputChanel(getMotionRequest());
  platformInterface.registerInputChanel(getBodyState());

  std::cout << "[Motion] register end" << std::endl;

  cycleStopwatch.start();
}//end init



void Motion2026::call()
{
  cycleStopwatch.stop();
  cycleStopwatch.start();
  PLOT("Motion.Cycle", cycleStopwatch.lastValue);
  STOPWATCH_START("MotionExecute");

  // run the theLogProvider if avalieble
  if(theLogProvider) {
    theLogProvider->execute();
  }


  /**
  * run the motion engine
  */


  // logger
  motionLogger.log(getFrameInfo().getFrameNumber());


  // todo: execute debug commands => find a better place for this
  getDebugMessageOut().reset();

  for(std::list<DebugMessageIn::Message>::const_iterator iter = getDebugMessageInMotion().messages.begin();
      iter != getDebugMessageInMotion().messages.end(); ++iter)
  {
    debug_answer_stream.clear();
    debug_answer_stream.str("");

    getDebugCommandManager().handleCommand(iter->command, iter->arguments, debug_answer_stream);
    getDebugMessageOut().addResponse(iter->id, debug_answer_stream);
  }

  // HACK: reset all the debug stuff before executing the modules
  STOPWATCH_START("Motion.Debug.Init");
  getDebugDrawings().reset();
  getDebugImageDrawings().reset();
  getDebugImageDrawingsTop().reset();
  getDebugDrawings3D().reset();
  STOPWATCH_STOP("Motion.Debug.Init");

  STOPWATCH_STOP("MotionExecute");

}//end call
