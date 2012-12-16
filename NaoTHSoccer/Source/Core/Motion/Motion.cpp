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
  :
  theBlackBoard(MotionBlackBoard::getInstance()),
  
  //theFootTouchCalibrator(theBlackBoard.theFSRData, theBlackBoard.theMotionStatus, theBlackBoard.theSupportPolygon, theBlackBoard.theKinematicChainModel),
  
  frameNumSinceLastMotionRequest(0),
  lastCognitionFrameNumber(0),
  motionLogger("MotionLog")
{

  REGISTER_DEBUG_COMMAND(motionLogger.getCommand(), motionLogger.getDescription(), &motionLogger);

  #define ADD_LOGGER(R) motionLogger.addRepresentation(&(theBlackBoard.the##R), #R);

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
  theBlackBoard.theSensorJointData.init();
  theBlackBoard.theKinematicChain.init(theBlackBoard.theSensorJointData);
  theBlackBoard.theKinematicChainModel.init(theBlackBoard.theMotorJointData);

  // init robot info
  theBlackBoard.theRobotInfo.platform = platform.getName();
  theBlackBoard.theRobotInfo.bodyNickName = platform.getBodyNickName();
  theBlackBoard.theRobotInfo.bodyID = platform.getBodyID();
  theBlackBoard.theRobotInfo.basicTimeStep = platform.getBasicTimeStep();

  g_message("Motion register begin");
#define REG_INPUT(R)                                                    \
  platformInterface.registerInput(theBlackBoard.the##R)

  REG_INPUT(SensorJointData);
  REG_INPUT(FrameInfo);
  REG_INPUT(InertialSensorData);
  REG_INPUT(FSRData);
  REG_INPUT(AccelerometerData);
  REG_INPUT(GyrometerData);

#define REG_OUTPUT(R)                                                   \
  platformInterface.registerOutput(theBlackBoard.the##R)

  REG_OUTPUT(MotorJointData);
  //REG_OUTPUT(LEDData);

  g_message("Motion register end");

  // messages from motion to cognition
  platformInterface.registerOutputChanel<CameraMatrix, Serializer<CameraMatrix> >(theBlackBoard.theCameraMatrix);
  platformInterface.registerOutputChanel<MotionStatus, Serializer<MotionStatus> >(theBlackBoard.theMotionStatus);
  platformInterface.registerOutputChanel<OdometryData, Serializer<OdometryData> >(theBlackBoard.theOdometryData);
  //platformInterface.registerOutputChanel<CalibrationData, Serializer<CalibrationData> >(theBlackBoard.theCalibrationData);
  platformInterface.registerOutputChanel<InertialModel, Serializer<InertialModel> >(theBlackBoard.theInertialModel);

  // messages from cognition to motion
  platformInterface.registerInputChanel<CameraInfo, Serializer<CameraInfo> >(theBlackBoard.theCameraInfo);
  platformInterface.registerInputChanel<HeadMotionRequest, Serializer<HeadMotionRequest> >(theBlackBoard.theHeadMotionRequest);
  platformInterface.registerInputChanel<MotionRequest, Serializer<MotionRequest> >(theBlackBoard.theMotionRequest);

}//end init


void Motion::call()
{
  STOPWATCH_START("MotionExecute");

  // process sensor data
  STOPWATCH_START("Motion:processSensorData");
  processSensorData();
  STOPWATCH_STOP("Motion:processSensorData");

  // check if cognition is still alive

  if(lastCognitionFrameNumber == theBlackBoard.theMotionRequest.cognitionFrameNumber)
  {
    frameNumSinceLastMotionRequest++;
  }
  else
  {
    lastCognitionFrameNumber = theBlackBoard.theMotionRequest.cognitionFrameNumber;
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
  }

  /**
  * run the motion engine
  */
  theMotionEngine.execute();

  // TODO: do we need it, is was never used so far
  // calibrate the foot touch detector
  /*
  if(theBlackBoard.theMotionRequest.calibrateFootTouchDetector)
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
  int i = theBlackBoard.theSensorJointData.checkStiffness();
  if(i != -1)
  {
    THROW("Get ILLEGAL Stiffness: "<<JointData::getJointName(JointData::JointID(i))<<" = "<<theBlackBoard.theSensorJointData.stiffness[i]);
  }

  // calibrate inrtia sensors
  theInertiaSensorCalibrator->execute();

  // correct the sensors
  theBlackBoard.theInertialSensorData.data += theBlackBoard.theCalibrationData.inertialSensorOffset;
  theBlackBoard.theGyrometerData.data += theBlackBoard.theCalibrationData.gyroSensorOffset;
  theBlackBoard.theAccelerometerData.data += theBlackBoard.theCalibrationData.accSensorOffset;

  //
  theInertiaSensorFilterBH->execute();

  //
  theFootGroundContactDetector->execute();

  //
  Kinematics::ForwardKinematics::calculateKinematicChainAll(
    theBlackBoard.theAccelerometerData,
    //theBlackBoard.theInertialSensorData.data,
    theBlackBoard.theInertialModel.orientation,
    theBlackBoard.theKinematicChain,
    theBlackBoard.theFSRPos.pos, // provides theFSRPos
    theBlackBoard.theRobotInfo.getBasicTimeStepInSecond());

  //
  theSupportPolygonGenerator->execute();

  //
  updateCameraMatrix();

  //
  theOdometryCalculator->execute();
  /*
  theOdometryCalculator.calculateOdometry(
    theBlackBoard.theOdometryData,
    theBlackBoard.theKinematicChain,
    theBlackBoard.theFSRData);
    */

  Kinematics::ForwardKinematics::updateKinematicChainFrom(theBlackBoard.theKinematicChainModel.theLinks);
  theBlackBoard.theKinematicChainModel.updateCoM();

  theLastMotorJointData = theBlackBoard.theMotorJointData;


  // some basic plots
  // plotting sensor data
  PLOT("Motion:GyrometerData:data:x", theBlackBoard.theGyrometerData.data.x);
  PLOT("Motion:GyrometerData:data:y", theBlackBoard.theGyrometerData.data.y);

  PLOT("Motion:GyrometerData:rawData:x", theBlackBoard.theGyrometerData.rawData.x);
  PLOT("Motion:GyrometerData:rawData:y", theBlackBoard.theGyrometerData.rawData.y);
  PLOT("Motion:GyrometerData:rawZero:x", theBlackBoard.theGyrometerData.rawZero.x);
  PLOT("Motion:GyrometerData:rawZero:y", theBlackBoard.theGyrometerData.rawZero.y);
  PLOT("Motion:GyrometerData:ref", theBlackBoard.theGyrometerData.ref);

  PLOT("Motion:AccelerometerData:x", theBlackBoard.theAccelerometerData.data.x);
  PLOT("Motion:AccelerometerData:y", theBlackBoard.theAccelerometerData.data.y);
  PLOT("Motion:AccelerometerData:z", theBlackBoard.theAccelerometerData.data.z);

  PLOT("Motion:InertialSensorData:x", sin(theBlackBoard.theInertialSensorData.data.x));
  PLOT("Motion:InertialSensorData:y", (theBlackBoard.theInertialSensorData.data.y));

  PLOT("Motion:KinematicChain:oriantation:model:x",
    theBlackBoard.theKinematicChainModel.theLinks[KinematicChain::Hip].R.getXAngle()
  );
  PLOT("Motion:KinematicChain:oriantation:model:y",
    theBlackBoard.theKinematicChainModel.theLinks[KinematicChain::Hip].R.getYAngle()
  );

  DEBUG_REQUEST("Motion:KinematicChain:orientation_test",
    RotationMatrix calculatedRotation =
      Kinematics::ForwardKinematics::calcChestFeetRotation(theBlackBoard.theKinematicChain);

    // calculate expected acceleration sensor reading
    Vector2d inertialExpected(calculatedRotation.getXAngle(), calculatedRotation.getYAngle());

    PLOT("Motion:KinematicChain:oriantation:sensor:x", Math::toDegrees(inertialExpected.x) );
    PLOT("Motion:KinematicChain:oriantation:sensor:y", Math::toDegrees(inertialExpected.y) );
  );


  // plot the requested joint positions
#define PLOT_JOINT(name) \
  PLOT("Motion:MotorJointData:"#name, Math::toDegrees(theBlackBoard.theMotorJointData.position[JointData::name]))

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
  CameraMatrix& cameraMatrix = theBlackBoard.theCameraMatrix;
  CameraMatrixCalculator::calculateCameraMatrix(
    cameraMatrix,
    theBlackBoard.theCameraInfo,
    theBlackBoard.theKinematicChain);

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

} // end updateCameraMatrix

void Motion::postProcess()
{
  motionLogger.log(theBlackBoard.theFrameInfo.getFrameNumber());

  MotorJointData& mjd = theBlackBoard.theMotorJointData;
  double basicStepInS = theBlackBoard.theRobotInfo.getBasicTimeStepInSecond();

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

