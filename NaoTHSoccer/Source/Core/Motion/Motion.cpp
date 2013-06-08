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


#include "CameraMatrixCalculator/CameraMatrixCalculator.h"

#include "Tools/Debug/Stopwatch.h"
#include "Tools/Debug/DebugModify.h"

#include <DebugCommunication/DebugCommandManager.h>



using namespace naoth;

Motion::Motion()
  : ModuleManagerWithDebug("Motion"),
    motionLogger("MotionLog")
{

  REGISTER_DEBUG_COMMAND(motionLogger.getCommand(), motionLogger.getDescription(), &motionLogger);

  #define ADD_LOGGER(R) motionLogger.addRepresentation(&(get##R()), #R);

  ADD_LOGGER(FrameInfo);
  ADD_LOGGER(SensorJointData);
  ADD_LOGGER(MotorJointData);
  ADD_LOGGER(InertialSensorData);
  ADD_LOGGER(AccelerometerData);
  ADD_LOGGER(GyrometerData);
  ADD_LOGGER(FSRData);
  ADD_LOGGER(MotionRequest);

  DEBUG_REQUEST_REGISTER("Motion:KinematicChain:orientation_test", "", false);


  // register the modeules
  theInertiaSensorCalibrator = registerModule<InertiaSensorCalibrator>("InertiaSensorCalibrator", true);
  theInertiaSensorFilterBH = registerModule<InertiaSensorFilter>("InertiaSensorFilter", true);
  theFootGroundContactDetector = registerModule<FootGroundContactDetector>("FootGroundContactDetector", true);
  theSupportPolygonGenerator = registerModule<SupportPolygonGenerator>("SupportPolygonGenerator", true);
  theOdometryCalculator = registerModule<OdometryCalculator>("OdometryCalculator", true);
  theKinematicChainProvider = registerModule<KinematicChainProviderMotion>("KinematicChainProvider", true);

  theMotionEngine = registerModule<MotionEngine>("MotionEngine", true);
}

Motion::~Motion()
{

}

void Motion::init(naoth::ProcessInterface& platformInterface, const naoth::PlatformBase& platform)
{
  // TODO: need a better solution for this

  // load the joint limits from the config 
  JointData::loadJointLimitsFromConfig();


  // init robot info
  getRobotInfo().platform = platform.getName();
  getRobotInfo().bodyNickName = platform.getBodyNickName();
  getRobotInfo().bodyID = platform.getBodyID();
  getRobotInfo().basicTimeStep = platform.getBasicTimeStep();

  g_message("Motion register begin");
#define REG_INPUT(R)                                                    \
  platformInterface.registerInput(get##R())

  REG_INPUT(SensorJointData);
  REG_INPUT(FrameInfo);
  REG_INPUT(InertialSensorData);
  REG_INPUT(FSRData);
  REG_INPUT(AccelerometerData);
  REG_INPUT(GyrometerData);

#define REG_OUTPUT(R)                                                   \
  platformInterface.registerOutput(get##R())

  REG_OUTPUT(MotorJointData);
  //REG_OUTPUT(LEDData);

  // messages from motion to cognition
  platformInterface.registerOutputChanel(getCameraMatrix());
  platformInterface.registerOutputChanel(getCameraMatrixTop());
  platformInterface.registerOutputChanel(getMotionStatus());
  platformInterface.registerOutputChanel(getOdometryData());
  //platformInterface.registerOutputChanel(getCalibrationData);
  platformInterface.registerOutputChanel(getInertialModel());

  // messages from cognition to motion
  platformInterface.registerInputChanel(getCameraInfo());
  platformInterface.registerInputChanel(getCameraInfoTop());
  platformInterface.registerInputChanel(getHeadMotionRequest());
  platformInterface.registerInputChanel(getMotionRequest());

  g_message("Motion register end");
}//end init



void Motion::call()
{

  STOPWATCH_START("MotionExecute");
  
  //TODO: move it to platform
  guard_cognition();

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

  STOPWATCH_START("Motion:postProcess");
  postProcess();
  STOPWATCH_STOP("Motion:postProcess");

  STOPWATCH_STOP("MotionExecute");

}//end call


void Motion::processSensorData()
{
  // check all joint stiffness
  int i = getSensorJointData().checkStiffness();
  if(i != -1)
  {
    THROW("Get ILLEGAL Stiffness: "<<JointData::getJointName(JointData::JointID(i))<<" = "<<getSensorJointData().stiffness[i]);
  }

  // calibrate inertia sensors
  theInertiaSensorCalibrator->execute();

  //TODO: introduce calibrated versions of the data
  // correct the sensors
  getInertialSensorData().data += getCalibrationData().inertialSensorOffset;
  getGyrometerData().data += getCalibrationData().gyroSensorOffset;
  getAccelerometerData().data += getCalibrationData().accSensorOffset;

  //
  theInertiaSensorFilterBH->execute();

  //
  theFootGroundContactDetector->execute();



  theKinematicChainProvider->execute();
  //
  /*
  Kinematics::ForwardKinematics::calculateKinematicChainAll(
    getAccelerometerData(),
    //getInertialSensorData.data,
    getInertialModel().orientation,
    getKinematicChainSensor(),
    getFSRPositions().pos, // provides theFSRPos
    getRobotInfo().getBasicTimeStepInSecond());
  */
  //
  //Kinematics::ForwardKinematics::updateKinematicChainFrom(getKinematicChainMotor().theLinks);
  //getKinematicChainMotor().updateCoM();


  //
  theSupportPolygonGenerator->execute();

  //
  updateCameraMatrix("CameraMatrix", getCameraMatrix(), getCameraInfo());
  updateCameraMatrix("CameraMatrixTop", getCameraMatrixTop(), getCameraInfoTop());

  //
  theOdometryCalculator->execute();
  /*
  theOdometryCalculator.calculateOdometry(
    getOdometryData,
    getKinematicChain,
    getFSRData);
    */


  // store the MotorJointData
  theLastMotorJointData = getMotorJointData();

  debugPlots();
}//end processSensorData


void Motion::postProcess()
{
  motionLogger.log(getFrameInfo().getFrameNumber());

  MotorJointData& mjd = getMotorJointData();
  double basicStepInS = getRobotInfo().getBasicTimeStepInSecond();

#ifdef DEBUG
  int i = mjd.checkStiffness();
  if(i != -1)
  {
    THROW("Get ILLEGAL Stiffness: "<<JointData::getJointName(JointData::JointID(i))<<" = "<<mjd.stiffness[i]);
  }
#endif

  mjd.clamp();
  mjd.updateSpeed(theLastMotorJointData, basicStepInS);
  mjd.updateAcceleration(theLastMotorJointData, basicStepInS);
}//end postProcess


void Motion::debugPlots()
{

  // some basic plots
  // plotting sensor data
  PLOT("Motion:GyrometerData:data:x", getGyrometerData().data.x);
  PLOT("Motion:GyrometerData:data:y", getGyrometerData().data.y);

  PLOT("Motion:GyrometerData:rawData:x", getGyrometerData().rawData.x);
  PLOT("Motion:GyrometerData:rawData:y", getGyrometerData().rawData.y);
  PLOT("Motion:GyrometerData:ref", getGyrometerData().ref);

  PLOT("Motion:AccelerometerData:x", getAccelerometerData().data.x);
  PLOT("Motion:AccelerometerData:y", getAccelerometerData().data.y);
  PLOT("Motion:AccelerometerData:z", getAccelerometerData().data.z);

  PLOT("Motion:InertialSensorData:x", sin(getInertialSensorData().data.x));
  PLOT("Motion:InertialSensorData:y", (getInertialSensorData().data.y));

  PLOT("Motion:KinematicChain:oriantation:model:x",
    getKinematicChainMotor().theLinks[KinematicChain::Hip].R.getXAngle()
  );
  PLOT("Motion:KinematicChain:oriantation:model:y",
    getKinematicChainMotor().theLinks[KinematicChain::Hip].R.getYAngle()
  );

  DEBUG_REQUEST("Motion:KinematicChain:orientation_test",
    RotationMatrix calculatedRotation =
      Kinematics::ForwardKinematics::calcChestFeetRotation(getKinematicChainSensor());

    // calculate expected acceleration sensor reading
    Vector2d inertialExpected(calculatedRotation.getXAngle(), calculatedRotation.getYAngle());

    PLOT("Motion:KinematicChain:oriantation:sensor:x", Math::toDegrees(inertialExpected.x) );
    PLOT("Motion:KinematicChain:oriantation:sensor:y", Math::toDegrees(inertialExpected.y) );
  );


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


  
#define PLOT_JOINT_ERROR(name) \
  {double e = getMotorJointData().position[JointData::name] - getSensorJointData().position[JointData::name];\
  PLOT("Motion:MotorJointError:"#name, Math::toDegrees(e));}

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


void Motion::updateCameraMatrix(
                                std::string name,
                                CameraMatrix& cameraMatrix,
                                const CameraInfo& cameraInfo)
{
  CameraMatrixCalculator::calculateCameraMatrix(
    cameraMatrix,
    cameraInfo,
    getKinematicChainSensor());
  cameraMatrix.timestamp = getSensorJointData().timestamp;
  cameraMatrix.valid = true;

  MODIFY(name + ":translation:x", cameraMatrix.translation.x);
  MODIFY(name + ":translation:y", cameraMatrix.translation.y);
  MODIFY(name + ":translation:z", cameraMatrix.translation.z);

  double correctionAngleX = 0.0;
  double correctionAngleY = 0.0;
  double correctionAngleZ = 0.0;
  MODIFY(name + ":correctionAngle:x", correctionAngleX);
  MODIFY(name + ":correctionAngle:y", correctionAngleY);
  MODIFY(name + ":correctionAngle:z", correctionAngleZ);

  cameraMatrix.rotation.rotateX(correctionAngleX);
  cameraMatrix.rotation.rotateY(correctionAngleY);
  cameraMatrix.rotation.rotateZ(correctionAngleZ);


}// end updateCameraMatrix



void Motion::guard_cognition()
{
  static unsigned int frameNumSinceLastMotionRequest(0);
  static unsigned int lastCognitionFrameNumber(0);

  // TODO: put this in the platform
  // check if cognition is still alive
  if(lastCognitionFrameNumber == getMotionRequest().cognitionFrameNumber)
  {
    frameNumSinceLastMotionRequest++;
  }
  else
  {
    lastCognitionFrameNumber = getMotionRequest().cognitionFrameNumber;
    frameNumSinceLastMotionRequest = 0;
  }

  if(frameNumSinceLastMotionRequest > 1500)
  {
    std::cerr << "+==================================+" << std::endl;
    std::cerr << "| NO MORE MESSAGES FROM COGNITION! |" << std::endl;
    std::cerr << "+==================================+" << std::endl;
    std::cerr << "dumping traces" << std::endl;
    Trace::getInstance().dump();
    StopwatchManager::getInstance().dump("cognition");

    //TODO: Maybe better put it into Platform?
    #ifndef WIN32
    std::cerr << "syncing file system..." ;
    sync();
    std::cerr << " finished." << std::endl;
    #endif

    ASSERT(frameNumSinceLastMotionRequest <= 500);
  }//end if
}//end guard_cognition


/*
// todo is it needed?
bool Motion::exit()
{
  state = exiting;
  if ( theBlackBoard.currentlyExecutedMotion->getId() == motion::init
      && theBlackBoard.currentlyExecutedMotion->isFinish() )
  {
    return true;
  }
  return false;
}//end exit
*/

