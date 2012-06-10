/**
 * @file Motion.cpp
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 *
 */

#include "Motion.h"
#include <glib.h>

#include <stdlib.h>

#include "MorphologyProcessor/ForwardKinematics.h"
#include "MorphologyProcessor/FootGroundContactDetector.h"

#include "CameraMatrixCalculator/CameraMatrixCalculator.h"
#include "Engine/InitialMotion/InitialMotionFactory.h"
#include "Engine/InverseKinematicsMotion/InverseKinematicsMotionFactory.h"
#include "Engine/ParallelKinematicMotionEngine/ParallelKinematicMotionFactory.h"
#include "Engine/KeyFrameMotion/KeyFrameMotionEngine.h"

#include "Tools/Debug/Stopwatch.h"
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugModify.h"

#include "SensorFilter/InertiaSensorFilter.h"

#include <DebugCommunication/DebugCommandManager.h>

#ifdef NAO_OLD
#include "Tools/Debug/DebugBufferedOutput.h"
#include "Tools/Debug/DebugDrawings.h"
#include "Tools/Debug/DebugDrawings3D.h"
#endif

using namespace naoth;

Motion::Motion()
  :
  theBlackBoard(MotionBlackBoard::getInstance()),
  theInertiaSensorCalibrator(theBlackBoard, theBlackBoard.theCalibrationData),
  theFootTouchCalibrator(theBlackBoard.theFSRData, theBlackBoard.theMotionStatus, theBlackBoard.theSupportPolygon, theBlackBoard.theKinematicChainModel),
  frameNumSinceLastMotionRequest(0),
  lastCognitionFrameNumber(0),
  state(initial),
  motionLogger("MotionLog"),
  oldMotionRequestTime(0)
{
  theSupportPolygonGenerator.init(theBlackBoard.theFSRData.force,
    theBlackBoard.theFSRPos,
    theBlackBoard.theKinematicChain.theLinks);
    
  theMotionFactories.push_back(new InitialMotionFactory());
  theMotionFactories.push_back(new InverseKinematicsMotionFactory());
  theMotionFactories.push_back(new KeyFrameMotionEngine());
  theMotionFactories.push_back(new ParallelKinematicMotionFactory());


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
}

Motion::~Motion()
{
  for (std::list<MotionFactory*>::iterator iter = theMotionFactories.begin();
    iter != theMotionFactories.end(); ++iter)
  {
    delete *iter;
  }
  theMotionFactories.clear();
}

void Motion::init(naoth::PlatformInterfaceBase& platformInterface)
{
  theBlackBoard.init();
  theBlackBoard.currentlyExecutedMotion = &theEmptyMotion;

  // init robot info
  theBlackBoard.theRobotInfo.platform = platformInterface.getName();
  theBlackBoard.theRobotInfo.bodyNickName = platformInterface.getBodyNickName();
  theBlackBoard.theRobotInfo.bodyID = platformInterface.getBodyID();
  theBlackBoard.theRobotInfo.basicTimeStep = platformInterface.getBasicTimeStep();
  
  g_message("Motion register begin");
#define REG_INPUT(R)                                                    \
  platformInterface.registerMotionInput(theBlackBoard.the##R)
  
  REG_INPUT(SensorJointData);
  REG_INPUT(FrameInfo);
  REG_INPUT(InertialSensorData);
  REG_INPUT(FSRData);
  REG_INPUT(AccelerometerData);
  REG_INPUT(GyrometerData);
  
#define REG_OUTPUT(R)                                                   \
  platformInterface.registerMotionOutput(theBlackBoard.the##R)

  REG_OUTPUT(MotorJointData);
  //REG_OUTPUT(LEDData);

  g_message("Motion register end");
  
  // messages from motion to cognition
  platformInterface.registerMotionOutputChanel<MotionStatus, Serializer<MotionStatus> >(theBlackBoard.theMotionStatus);
  platformInterface.registerMotionOutputChanel<OdometryData, Serializer<OdometryData> >(theBlackBoard.theOdometryData);
  //platformInterface.registerMotionOutputChanel<CalibrationData, Serializer<CalibrationData> >(theBlackBoard.theCalibrationData);
  platformInterface.registerMotionOutputChanel<InertialModel, Serializer<InertialModel> >(theBlackBoard.theInertialModel);

  // messages from cognition to motion
  platformInterface.registerMotionInputChanel<HeadMotionRequest, Serializer<HeadMotionRequest> >(theBlackBoard.theHeadMotionRequest);
  platformInterface.registerMotionInputChanel<MotionRequest, Serializer<MotionRequest> >(theBlackBoard.theMotionRequest);

  selectMotion();// create init motion
  state = initial;
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

  if(frameNumSinceLastMotionRequest > 500)
  {
    std::cerr << "+==================================+" << std::endl;
    std::cerr << "| NO MORE MESSAGES FROM COGNITION! |" << std::endl;
    std::cerr << "+==================================+" << std::endl;
    std::cerr << "dumping traces" << std::endl;
    Trace::getInstance().dump();
    Stopwatch::getInstance().dump("cognition");

    std::cerr << "syncing file system..." ;
    sync();
    std::cerr << " finished." << std::endl;

    ASSERT(frameNumSinceLastMotionRequest <= 500);
  }

  // ensure initialization
  switch (state)
  {
  case initial:
  {
    theBlackBoard.theHeadMotionRequest.id = HeadMotionRequest::numOfHeadMotion;
    theBlackBoard.theMotionRequest.time = theBlackBoard.theMotionStatus.time;
    theBlackBoard.theMotionRequest.id = motion::init;

    if ( theBlackBoard.theMotionStatus.currentMotion == motion::init
        && !theBlackBoard.currentlyExecutedMotion->isStopped()
        && theBlackBoard.currentlyExecutedMotion->isFinish() )
    {
      state = running;
    }

    break;
  }
  case running:
  {
    break;
  }
  case exiting:
  {
    theBlackBoard.theHeadMotionRequest.id = HeadMotionRequest::numOfHeadMotion;
    theBlackBoard.theMotionRequest.time = theBlackBoard.theMotionStatus.time;
    theBlackBoard.theMotionRequest.id = motion::init;
    break;
  }
  }//end switch

  // IMPORTANT: execute head motion firstly
  // stabilization of the walk depends on the head position
  // cf. InverseKinematicsMotionEngine::controlCenterOfMass(...)
  theHeadMotionEngine.execute();

  // motion engine execute
  selectMotion();
  ASSERT(NULL!=theBlackBoard.currentlyExecutedMotion);
  theBlackBoard.currentlyExecutedMotion->execute(theBlackBoard.theMotionRequest, theBlackBoard.theMotionStatus);
  theBlackBoard.theMotionStatus.currentMotionState = theBlackBoard.currentlyExecutedMotion->state();

  // calibrate the foot touch detector
  if(theBlackBoard.theMotionRequest.calibrateFootTouchDetector)
    theFootTouchCalibrator.execute();
  
  STOPWATCH_START("Motion:postProcess");
  postProcess();
  STOPWATCH_STOP("Motion:postProcess");

  STOPWATCH_STOP("MotionExecute");

#ifdef NAO_OLD
  theStopwatchSender.execute();
#endif
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
  theInertiaSensorCalibrator.update();

  // correct the sensors
  theBlackBoard.theInertialSensorData.data += theBlackBoard.theCalibrationData.inertialSensorOffset;
  theBlackBoard.theGyrometerData.data += theBlackBoard.theCalibrationData.gyroSensorOffset;
  theBlackBoard.theAccelerometerData.data += theBlackBoard.theCalibrationData.accSensorOffset;
  
  // 
  static InertiaSensorFilter theInertiaSensorFilterBH(theBlackBoard, theBlackBoard.theInertialModel);
  theInertiaSensorFilterBH.update();


  static FootGroundContactDetector theFootGroundContactDetector(theBlackBoard, theBlackBoard.theFSRData, theBlackBoard.theGroundContactModel);
  theFootGroundContactDetector.update();

  //
  Kinematics::ForwardKinematics::calculateKinematicChainAll(
    theBlackBoard.theAccelerometerData,
    //theBlackBoard.theInertialSensorData.data,
    theBlackBoard.theInertialModel.orientation,
    theBlackBoard.theKinematicChain,
    theBlackBoard.theFSRPos,
    theBlackBoard.theRobotInfo.getBasicTimeStepInSecond());

  theSupportPolygonGenerator.calcSupportPolygon(theBlackBoard.theSupportPolygon);
  
  CameraMatrixCalculator::calculateCameraMatrix(
    theBlackBoard.theCameraMatrix,
    theBlackBoard.theHeadMotionRequest.cameraID,
    theBlackBoard.theKinematicChain);
    
  theOdometryCalculator.calculateOdometry(
    theBlackBoard.theOdometryData,
    theBlackBoard.theKinematicChain,
    theBlackBoard.theFSRData);
    
  Kinematics::ForwardKinematics::updateKinematicChainFrom(theBlackBoard.theKinematicChainModel.theLinks);
  theBlackBoard.theKinematicChainModel.updateCoM();
  
  theBlackBoard.theLastMotorJointData = theBlackBoard.theMotorJointData;


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
  mjd.updateSpeed(theBlackBoard.theLastMotorJointData, basicStepInS);
  mjd.updateAcceleration(theBlackBoard.theLastMotorJointData, basicStepInS);

    
#ifdef NAO_OLD
  theDebugMessageOut.answers.clear();
  for(std::list<DebugMessageIn::Message>::const_iterator iter = theDebugMessageIn.messages.begin();
      iter != theDebugMessageIn.messages.end(); ++iter)
  {
    std::stringstream answer;
    DebugCommandManager::getInstance().handleCommand(iter->command, iter->arguments, answer);
    theDebugMessageOut.answers.push_back(answer.str());
  }

  STOPWATCH_START("Debug ~ Init");
  DebugBufferedOutput::getInstance().update();
  DebugDrawings::getInstance().update();
  DebugDrawings3D::getInstance().update();
  STOPWATCH_STOP("Debug ~ Init");
#endif
}//end postProcess

void Motion::selectMotion()
{
  ASSERT(theBlackBoard.currentlyExecutedMotion != NULL);

  // test if the current MotionStatus allready arrived in cognition
  if ( theBlackBoard.theMotionStatus.time != theBlackBoard.theMotionRequest.time )
    return;

  if (theBlackBoard.theMotionStatus.currentMotion == theBlackBoard.theMotionRequest.id
    && theBlackBoard.currentlyExecutedMotion->isStopped())
  {
    changeMotion(&theEmptyMotion);
  }

  if (theBlackBoard.theMotionStatus.currentMotion != theBlackBoard.theMotionRequest.id
    &&
    (theBlackBoard.currentlyExecutedMotion->isStopped() || theBlackBoard.theMotionRequest.forced))
  {
    AbstractMotion* newMotion = NULL;
    for ( std::list<MotionFactory*>::iterator iter=theMotionFactories.begin(); 
          NULL==newMotion && iter!=theMotionFactories.end(); ++iter)
    {
      newMotion = (*iter)->createMotion(theBlackBoard.theMotionRequest);
    }

    if (NULL != newMotion)
    {
      ASSERT(newMotion->getId() == theBlackBoard.theMotionRequest.id);
      changeMotion(newMotion);
    } else
    {
      changeMotion(&theEmptyMotion);
      cerr << "Warning: Request " << motion::getName(theBlackBoard.theMotionRequest.id)
        << " cannot be executed!" << endl;
    }
  }
}//end selectMotion

void Motion::changeMotion(AbstractMotion* m)
{
  theBlackBoard.currentlyExecutedMotion = m;
  theBlackBoard.theMotionStatus.lastMotion = theBlackBoard.theMotionStatus.currentMotion;
  theBlackBoard.theMotionStatus.currentMotion = theBlackBoard.currentlyExecutedMotion->getId();
  theBlackBoard.theMotionStatus.time = theBlackBoard.theFrameInfo.getTime();
}//end changeMotion

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

