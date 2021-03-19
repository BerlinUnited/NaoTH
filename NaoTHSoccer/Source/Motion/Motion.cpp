/**
 * @file Motion.cpp
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Mellmann, Heinrich</a>
 *
 */

#include "Motion.h"

#include <stdlib.h>

#ifndef WIN32
#include <unistd.h>
#endif

#include "MorphologyProcessor/ForwardKinematics.h"

#include "Tools/CameraGeometry.h"


using namespace naoth;

Motion::Motion()
  : ModuleManagerWithDebug(""),
    theLogProvider(NULL),
    motionLogger("MotionLog")
{
  REGISTER_DEBUG_COMMAND(motionLogger.getCommand(), motionLogger.getDescription(), &motionLogger);
  registerLogableRepresentationList();

  DEBUG_REQUEST_REGISTER("Motion:KinematicChain:orientation_test", "", false);

  DEBUG_REQUEST_REGISTER("Motion:KinematicChain:drawMotor3D", "", false);
  DEBUG_REQUEST_REGISTER("Motion:KinematicChain:drawSensor3D", "", false);

  REGISTER_DEBUG_COMMAND("DebugPlot:get", "get the plots", &getDebugPlot());

  // parameter
  REGISTER_DEBUG_COMMAND("ParameterList:list", "list all registered parameters", &getDebugParameterList());
  REGISTER_DEBUG_COMMAND("ParameterList:get", "get the parameter list with the given name", &getDebugParameterList());
  REGISTER_DEBUG_COMMAND("ParameterList:set", "set the parameter list with the given name", &getDebugParameterList());

  // register the modules
  theInertiaSensorCalibrator = registerModule<InertiaSensorCalibrator>("InertiaSensorCalibrator", true);
  theInertiaSensorFilterBH = registerModule<InertiaSensorFilter>("InertiaSensorFilter", true);
  theIMUModel = registerModule<IMUModel>("IMUModel", true);

  theFootGroundContactDetector = registerModule<FootGroundContactDetector>("FootGroundContactDetector", true);
  theOdometryCalculator = registerModule<OdometryCalculator>("OdometryCalculator", true);
  theKinematicChainProvider = registerModule<KinematicChainProviderMotion>("KinematicChainProvider", true);

  theArmCollisionDetector = registerModule<ArmCollisionDetector>("ArmCollisionDetector", false);
  theArmCollisionDetector2018 = registerModule<ArmCollisionDetector2018>("ArmCollisionDetector2018", true);
  theBumperCollisionDetector = registerModule<BumperCollisionDetector>("BumperCollisionDetector", false);

  theBodyCollisionDetector = registerModule<BodyCollisionDetector>("BodyCollisionDetector", false);  // WIP module - help wanted


  theMotionEngine = registerModule<MotionEngine>("MotionEngine", true);
  theCoPProvider  = registerModule<CoPProvider>("CoPProvider", true);
  theSensorLogger = registerModule<SensorLogger>("theSensorLogger", true);

  getDebugParameterList().add(&parameter);
  getWalk2018Parameters().init(getDebugParameterList());
}

Motion::~Motion()
{
  getDebugParameterList().remove(&parameter);
  getWalk2018Parameters().remove(getDebugParameterList());
}

void Motion::init(naoth::ProcessInterface& platformInterface, const naoth::PlatformBase& platform)
{
  // try to get the log provider
  theLogProvider = ModuleManager::getModule("LogProvider");

  // TODO: need a better solution for this
  // load the joint limits from the config
  JointData::loadJointLimitsFromConfig();


  // init robot info
  getRobotInfo().platform = platform.getPlatformName();
  getRobotInfo().headNickName = platform.getHeadNickName();
  getRobotInfo().bodyNickName = platform.getBodyNickName();
  getRobotInfo().bodyID = platform.getBodyID();
  getRobotInfo().basicTimeStep = platform.getBasicTimeStep();

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
  platformInterface.registerOutputChanel(getCalibrationData());
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



void Motion::call()
{
  cycleStopwatch.stop();
  cycleStopwatch.start();
  PLOT("Motion.Cycle", cycleStopwatch.lastValue);
  STOPWATCH_START("MotionExecute");

  // run the theLogProvider if avalieble
  if(theLogProvider) {
    theLogProvider->execute();
  }

  // process sensor data
  STOPWATCH_START("Motion:processSensorData");
  processSensorData();
  STOPWATCH_STOP("Motion:processSensorData");

  /**
  * run the motion engine
  */
  theMotionEngine->execute();

  // TODO: do we need it, is was never used so far
  // calibrate the foot touch detector
  /*
  if(getMotionRequest.calibrateFootTouchDetector)
    theFootTouchCalibrator.execute();
    */

  modifyJointOffsets();

  STOPWATCH_START("Motion:postProcess");
  postProcess();
  STOPWATCH_STOP("Motion:postProcess");

  DEBUG_REQUEST("Motion:KinematicChain:drawSensor3D",  drawRobot3D(getKinematicChainSensor()); );
  DEBUG_REQUEST("Motion:KinematicChain:drawMotor3D",  drawRobot3D(getKinematicChainMotor()); );

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


void Motion::processSensorData()
{

  // check all joint stiffness
  int i = getSensorJointData().checkStiffness();
  if(i != -1) {
    THROW("Get ILLEGAL Stiffness: "<<JointData::getJointName(JointData::JointID(i))<<" = "<<getSensorJointData().stiffness[i]);
  }

  // log sensor data
  if(parameter.recordSensorData) {
    theSensorLogger->execute();
  }

  // remove the offset from sensor joint data
  for( i = 0; i < JointData::numOfJoint; i++) {
      getSensorJointData().position[i] = getSensorJointData().position[i] - getOffsetJointData().position[i];
  }

  theIMUModel->execute();

  // calibrate inertia sensors
  theInertiaSensorCalibrator->execute();

  //HACK: override the sensor data with the calibrated versions
  //TODO: introduce calibrated versions of the data
  //TODO: correct the sensors z is inverted => don't forget to check all modules requiring/providing GyrometerData
  getGyrometerData().data      += getCalibrationData().gyroSensorOffset;
  getInertialSensorData().data += getCalibrationData().inertialSensorOffset;
  getAccelerometerData().data  += getCalibrationData().accSensorOffset;

  // TODO: is this still used?
  theInertiaSensorFilterBH->execute();

  // HACK: override InertialModel, which is usually provided by InertiaSensorFilterBH
  // only to enable transparent switching with InertiaSensorFilter
  if(parameter.letIMUModelProvideInertialModel) {
      getInertialModel().orientation = getIMUData().orientation;
  }

  //
  theFootGroundContactDetector->execute();

  //
  theKinematicChainProvider->execute();

  //
  theCoPProvider->execute();

  //
  updateCameraMatrix();

  //
  theOdometryCalculator->execute();

  //theArmCollisionDetector->execute();
  theArmCollisionDetector2018->execute();
  theBodyCollisionDetector->execute();
  theBumperCollisionDetector->execute();

  // store the MotorJointData
  theLastMotorJointData = getMotorJointData();

  // update the body status
  for(int i = 0; i < JointData::numOfJoint; ++i) {
    getBodyStatus().currentSum[i] += getSensorJointData().electricCurrent[i];
  }
  getBodyStatus().timestamp = getFrameInfo().getTime();


  debugPlots();
}//end processSensorData


void Motion::postProcess()
{
  motionLogger.log(getFrameInfo().getFrameNumber());

  MotorJointData& mjd = getMotorJointData();
  double basicStepInS = getRobotInfo().getBasicTimeStepInSecond();

#ifdef DEBUG
  int i = mjd.checkStiffness();
  if(i != -1) {
    THROW("Get ILLEGAL Stiffness: " << JointData::getJointName(JointData::JointID(i)) << " = " << mjd.stiffness[i]);
  }
#endif

  // apply the offset to motor joint data
  for(int i = 0; i < JointData::numOfJoint; i++){
      mjd.position[i] = mjd.position[i] + getOffsetJointData().position[i];
  }

  mjd.clamp();
  mjd.updateSpeed(theLastMotorJointData, basicStepInS);
  mjd.updateAcceleration(theLastMotorJointData, basicStepInS);

}//end postProcess

void Motion::modifyJointOffsets()
{
    MODIFY("Motion:Offsets:LHipYawPitch", getOffsetJointData().position[JointData::LHipYawPitch]);
    MODIFY("Motion:Offsets:RHipPitch",    getOffsetJointData().position[JointData::RHipPitch]);
    MODIFY("Motion:Offsets:LHipPitch",    getOffsetJointData().position[JointData::LHipPitch]);
    MODIFY("Motion:Offsets:RHipRoll",     getOffsetJointData().position[JointData::RHipRoll]);
    MODIFY("Motion:Offsets:LHipRoll",     getOffsetJointData().position[JointData::LHipRoll]);
    MODIFY("Motion:Offsets:RKneePitch",   getOffsetJointData().position[JointData::RKneePitch]);
    MODIFY("Motion:Offsets:LKneePitch",   getOffsetJointData().position[JointData::LKneePitch]);
    MODIFY("Motion:Offsets:RAnklePitch",  getOffsetJointData().position[JointData::RAnklePitch]);
    MODIFY("Motion:Offsets:LAnklePitch",  getOffsetJointData().position[JointData::LAnklePitch]);
    MODIFY("Motion:Offsets:RAnkleRoll",   getOffsetJointData().position[JointData::RAnkleRoll]);
    MODIFY("Motion:Offsets:LAnkleRoll",   getOffsetJointData().position[JointData::LAnkleRoll]);
}


void Motion::debugPlots()
{
  // some basic plots
  // plotting sensor data
  PLOT("Motion:GyrometerData:data:x", getGyrometerData().data.x);
  PLOT("Motion:GyrometerData:data:y", getGyrometerData().data.y);
  PLOT("Motion:GyrometerData:data:z", getGyrometerData().data.z);

  PLOT("Motion:GyrometerData:rawData:x", getGyrometerData().rawData.x);
  PLOT("Motion:GyrometerData:rawData:y", getGyrometerData().rawData.y);
  PLOT("Motion:GyrometerData:rawData:z", getGyrometerData().rawData.z);
  PLOT("Motion:GyrometerData:ref", getGyrometerData().ref);

  PLOT("Motion:AccelerometerData:x", getAccelerometerData().data.x);
  PLOT("Motion:AccelerometerData:y", getAccelerometerData().data.y);
  PLOT("Motion:AccelerometerData:z", getAccelerometerData().data.z);

  PLOT("Motion:InertialSensorData:x", getInertialSensorData().data.x);
  PLOT("Motion:InertialSensorData:y", getInertialSensorData().data.y);

//  PLOT("Motion:InertialModel:x", getInertialModel().orientation.x);
//  PLOT("Motion:InertialModel:y", getInertialModel().orientation.y);

  PLOT("Motion:KinematicChain:oriantation:model:x",
    getKinematicChainMotor().theLinks[KinematicChain::Hip].R.getXAngle()
  );
  PLOT("Motion:KinematicChain:oriantation:model:y",
    getKinematicChainMotor().theLinks[KinematicChain::Hip].R.getYAngle()
  );


  // TODO: shouldn't this be part of kinematicChainProvider?
  DEBUG_REQUEST("Motion:KinematicChain:orientation_test",
    RotationMatrix calculatedRotation =
      Kinematics::ForwardKinematics::calcChestToFeetRotation(getKinematicChainSensor());

    // calculate expected acceleration sensor reading
    Vector2d inertialExpected(calculatedRotation.getXAngle(), calculatedRotation.getYAngle());

    PLOT("Motion:KinematicChain:oriantation:sensor:x", Math::toDegrees(inertialExpected.x) );
    PLOT("Motion:KinematicChain:oriantation:sensor:y", Math::toDegrees(inertialExpected.y) );
  );

  // add currentSum array to the ring buffer
  for(int i = 0; i < naoth::JointData::numOfJoint; i++){
      currentsRingBuffer[i].add(getBodyStatus().currentSum[i]);
  }

  // plot the filtered current (mean filter)
#define PLOT_CURRENT(name) \
  if(currentsRingBuffer[JointData::name].isFull()) { \
    PLOT("Motion:Current:"#name, (currentsRingBuffer[JointData::name].last() - currentsRingBuffer[JointData::name].first()) / currentsRingBuffer[JointData::name].getMaxEntries()); \
  }

  PLOT_CURRENT(HeadPitch);
  PLOT_CURRENT(HeadYaw);

  PLOT_CURRENT(RShoulderRoll);
  PLOT_CURRENT(LShoulderRoll);
  PLOT_CURRENT(RShoulderPitch);
  PLOT_CURRENT(LShoulderPitch);
  PLOT_CURRENT(RElbowRoll);
  PLOT_CURRENT(LElbowRoll);
  PLOT_CURRENT(RElbowYaw);
  PLOT_CURRENT(LElbowYaw);

  PLOT_CURRENT(RHipYawPitch);
  PLOT_CURRENT(LHipYawPitch);
  PLOT_CURRENT(RHipPitch);
  PLOT_CURRENT(LHipPitch);
  PLOT_CURRENT(RHipRoll);
  PLOT_CURRENT(LHipRoll);
  PLOT_CURRENT(RKneePitch);
  PLOT_CURRENT(LKneePitch);
  PLOT_CURRENT(RAnklePitch);
  PLOT_CURRENT(LAnklePitch);
  PLOT_CURRENT(RAnkleRoll);
  PLOT_CURRENT(LAnkleRoll);

  // plot the requested joint positions
#define PLOT_JOINT(name) \
  PLOT("Motion:MotorJointData:"#name, Math::toDegrees(getMotorJointData().position[JointData::name]))

  PLOT_JOINT(HeadPitch);
  PLOT_JOINT(HeadYaw);

  PLOT_JOINT(RShoulderRoll);
  PLOT_JOINT(LShoulderRoll);
  PLOT_JOINT(RShoulderPitch);
  PLOT_JOINT(LShoulderPitch);
  PLOT_JOINT(RElbowRoll);
  PLOT_JOINT(LElbowRoll);
  PLOT_JOINT(RElbowYaw);
  PLOT_JOINT(LElbowYaw);

  PLOT_JOINT(RHipYawPitch);
  PLOT_JOINT(LHipYawPitch);
  PLOT_JOINT(RHipPitch);
  PLOT_JOINT(LHipPitch);
  PLOT_JOINT(RHipRoll);
  PLOT_JOINT(LHipRoll);
  PLOT_JOINT(RKneePitch);
  PLOT_JOINT(LKneePitch);
  PLOT_JOINT(RAnklePitch);
  PLOT_JOINT(LAnklePitch);
  PLOT_JOINT(RAnkleRoll);
  PLOT_JOINT(LAnkleRoll);

  // buffer motor joint data to determine the error to the corresponding sensor joint data (will be read in 4 cycles)
  for(int i = 0; i < naoth::JointData::numOfJoint; i++){
      motorJointDataBuffer[i].add(getMotorJointData().position[i]);
  }

#define PLOT_JOINT_ERROR(name) \
  { if(motorJointDataBuffer[JointData::name].isFull()) { \
        double e = motorJointDataBuffer[JointData::name].first() - getSensorJointData().position[JointData::name];\
        PLOT("Motion:MotorJointError:"#name, Math::toDegrees(e)); \
    } \
  }

  PLOT_JOINT_ERROR(HeadPitch);
  PLOT_JOINT_ERROR(HeadYaw);

  PLOT_JOINT_ERROR(RShoulderRoll);
  PLOT_JOINT_ERROR(LShoulderRoll);
  PLOT_JOINT_ERROR(RShoulderPitch);
  PLOT_JOINT_ERROR(LShoulderPitch);
  PLOT_JOINT_ERROR(RElbowRoll);
  PLOT_JOINT_ERROR(LElbowRoll);
  PLOT_JOINT_ERROR(RElbowYaw);
  PLOT_JOINT_ERROR(LElbowYaw);

  PLOT_JOINT_ERROR(RHipYawPitch);
  PLOT_JOINT_ERROR(LHipYawPitch);
  PLOT_JOINT_ERROR(RHipPitch);
  PLOT_JOINT_ERROR(LHipPitch);
  PLOT_JOINT_ERROR(RHipRoll);
  PLOT_JOINT_ERROR(LHipRoll);
  PLOT_JOINT_ERROR(RKneePitch);
  PLOT_JOINT_ERROR(LKneePitch);
  PLOT_JOINT_ERROR(RAnklePitch);
  PLOT_JOINT_ERROR(LAnklePitch);
  PLOT_JOINT_ERROR(RAnkleRoll);
  PLOT_JOINT_ERROR(LAnkleRoll);

}//end debugPlots


void Motion::updateCameraMatrix()
{
  getCameraMatrix() = CameraGeometry::calculateCameraMatrixFromChestPose(
              getKinematicChainSensor().theLinks[KinematicChain::Torso].M,
              NaoInfo::robotDimensions.cameraTransform[naoth::CameraInfo::Bottom].offset,
              NaoInfo::robotDimensions.cameraTransform[naoth::CameraInfo::Bottom].rotationY,
              getCameraMatrixOffset().body_rot,
              getCameraMatrixOffset().head_rot,
              getCameraMatrixOffset().cam_rot[naoth::CameraInfo::Bottom],
              getSensorJointData().position[JointData::HeadYaw],
              getSensorJointData().position[JointData::HeadPitch],
              getInertialModel().orientation);

  getCameraMatrixTop() = CameraGeometry::calculateCameraMatrixFromChestPose(
              getKinematicChainSensor().theLinks[KinematicChain::Torso].M,
              NaoInfo::robotDimensions.cameraTransform[naoth::CameraInfo::Top].offset,
              NaoInfo::robotDimensions.cameraTransform[naoth::CameraInfo::Top].rotationY,
              getCameraMatrixOffset().body_rot,
              getCameraMatrixOffset().head_rot,
              getCameraMatrixOffset().cam_rot[naoth::CameraInfo::Top],
              getSensorJointData().position[JointData::HeadYaw],
              getSensorJointData().position[JointData::HeadPitch],
              getInertialModel().orientation);

  getCameraMatrix().timestamp = getSensorJointData().timestamp;
  getCameraMatrix().valid = true;

  getCameraMatrixTop().timestamp = getSensorJointData().timestamp;
  getCameraMatrixTop().valid = true;
}// end updateCameraMatrix

void Motion::drawRobot3D(const KinematicChain& kinematicChain)
{
  const Kinematics::Link* theLink = kinematicChain.theLinks;

  for (int i = 0; i < KinematicChain::numOfLinks; i++)
  {
    if ( i != KinematicChain::Neck
      && i != KinematicChain::LShoulder
      && i != KinematicChain::LElbow
      && i != KinematicChain::RShoulder
      && i != KinematicChain::RElbow
      && i != KinematicChain::Hip)
    {
      ENTITY(KinematicChain::getLinkName((KinematicChain::LinkID)i), theLink[i].M);
    }
  }//end for
}//end drawRobot3D
