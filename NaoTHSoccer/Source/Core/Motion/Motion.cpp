/**
 * @file Motion.cpp
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Mellmann, Heinrich</a>
 *
 */

#include "Motion.h"
#include <glib.h>

#include <stdlib.h>

#ifndef WIN32
#include <unistd.h>
#endif


#include "MorphologyProcessor/ForwardKinematics.h"


#include "CameraMatrixCalculator/CameraMatrixCalculator.h"

#include "Tools/Debug/Stopwatch.h"
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugModify.h"
#include <Tools/CameraGeometry.h>

#include <DebugCommunication/DebugCommandManager.h>


using namespace naoth;

Motion::Motion()
  //theFootTouchCalibrator(getFSRData, getMotionStatus, getSupportPolygon, getKinematicChainModel),

  : motionLogger("MotionLog")
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

  //theMotionEngine = registerModule<MotionEngine>("MotionEngine", true);
}

Motion::~Motion()
{

}

void Motion::init(naoth::ProcessInterface& platformInterface, const naoth::PlatformBase& platform)
{
  //theBlackBoard.init();
  // TODO: need a better solution for this
  getSensorJointData().init();
  getKinematicChainSensor().init(getSensorJointData());
  getKinematicChainMotor().init(getMotorJointData()); // TODO: make it const

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

  g_message("Motion register end");

  // messages from motion to cognition
  platformInterface.registerOutputChanel<CameraMatrix, Serializer<CameraMatrix> >(getCameraMatrix());
  platformInterface.registerOutputChanel<MotionStatus, Serializer<MotionStatus> >(getMotionStatus());
  platformInterface.registerOutputChanel<OdometryData, Serializer<OdometryData> >(getOdometryData());
  //platformInterface.registerOutputChanel<CalibrationData, Serializer<CalibrationData> >(getCalibrationData);
  platformInterface.registerOutputChanel<InertialModel, Serializer<InertialModel> >(getInertialModel());

  // messages from cognition to motion
  platformInterface.registerInputChanel<CameraInfo, Serializer<CameraInfo> >(getCameraInfo());
  platformInterface.registerInputChanel<HeadMotionRequest, Serializer<HeadMotionRequest> >(getHeadMotionRequest());
  platformInterface.registerInputChanel<MotionRequest, Serializer<MotionRequest> >(getMotionRequest());

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
  theMotionEngine.execute();

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

  //
  Kinematics::ForwardKinematics::calculateKinematicChainAll(
    getAccelerometerData(),
    //getInertialSensorData.data,
    getInertialModel().orientation,
    getKinematicChainSensor(),
    getFSRPositions().pos, // provides theFSRPos
    getRobotInfo().getBasicTimeStepInSecond());

  //
  theSupportPolygonGenerator->execute();

  //
  updateCameraMatrix();

  //
  theOdometryCalculator->execute();
  /*
  theOdometryCalculator.calculateOdometry(
    getOdometryData,
    getKinematicChain,
    getFSRData);
    */

  //
  Kinematics::ForwardKinematics::updateKinematicChainFrom(getKinematicChainMotor().theLinks);
  getKinematicChainMotor().updateCoM();

  // store the MotorJointData
  theLastMotorJointData = getMotorJointData();




  // some basic plots
  // plotting sensor data
  PLOT("Motion:GyrometerData:data:x", getGyrometerData().data.x);
  PLOT("Motion:GyrometerData:data:y", getGyrometerData().data.y);

  PLOT("Motion:GyrometerData:rawData:x", getGyrometerData().rawData.x);
  PLOT("Motion:GyrometerData:rawData:y", getGyrometerData().rawData.y);
  PLOT("Motion:GyrometerData:rawZero:x", getGyrometerData().rawZero.x);
  PLOT("Motion:GyrometerData:rawZero:y", getGyrometerData().rawZero.y);
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

}//end processSensorData


void Motion::updateCameraMatrix()
{
  CameraMatrix& cameraMatrix = getCameraMatrix();
  CameraMatrixCalculator::calculateCameraMatrix(
    cameraMatrix,
    getCameraInfo(),
    getKinematicChainSensor());

  cameraMatrix.valid = true;

  MODIFY("CameraMatrix:translation:x", cameraMatrix.translation.x);
  MODIFY("CameraMatrix:translation:y", cameraMatrix.translation.y);
  MODIFY("CameraMatrix:translation:z", cameraMatrix.translation.z);

  double correctionAngleX = 0.0;
  double correctionAngleY = 0.0;
  double correctionAngleZ = 0.0;
  MODIFY("CameraMatrix:correctionAngle:x", correctionAngleX);
  MODIFY("CameraMatrix:correctionAngle:y", correctionAngleY);
  MODIFY("CameraMatrix:correctionAngle:z", correctionAngleZ);

  cameraMatrix.rotation.rotateX(correctionAngleX);
  cameraMatrix.rotation.rotateY(correctionAngleY);
  cameraMatrix.rotation.rotateZ(correctionAngleZ);

}// end updateCameraMatrix


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
    Stopwatch::getInstance().dump("cognition");

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

