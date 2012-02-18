/**
 * @file Motion.cpp
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 *
 */

#include "Motion.h"
#include <glib.h>

#include "MorphologyProcessor/ForwardKinematics.h"
#include "CameraMatrixCalculator/CameraMatrixCalculator.h"
#include "Engine/InitialMotion/InitialMotionFactory.h"
#include "Engine/InverseKinematicsMotion/InverseKinematicsMotionFactory.h"
#include "Engine/ParallelKinematicMotionEngine/ParallelKinematicMotionFactory.h"
#include "Engine/KeyFrameMotion/KeyFrameMotionEngine.h"

#include "Tools/Debug/Stopwatch.h"
#include "Tools/Debug/DebugRequest.h"

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
  theInertialFilter(theBlackBoard, theBlackBoard.theCalibrationData.inertialSensorOffset),
  theFootTouchCalibrator(theBlackBoard.theFSRData, theBlackBoard.theMotionStatus, theBlackBoard.theSupportPolygon, theBlackBoard.theKinematicChainModel),
  theMotionStatusWriter(NULL),
  theOdometryDataWriter(NULL),
  theHeadMotionRequestReader(NULL),
  theMotionRequestReader(NULL),
  frameNumSinceLastMotionRequest(0),
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


  DEBUG_REQUEST_REGISTER("Motion:kc_sensor_test", "", false);
}

Motion::~Motion()
{
  for (std::list<MotionFactory*>::iterator iter = theMotionFactories.begin();
    iter != theMotionFactories.end(); ++iter)
  {
    delete *iter;
  }
  theMotionFactories.clear();
  
  delete theMotionStatusWriter;
  delete theOdometryDataWriter;
  delete theHeadMotionRequestReader;
  delete theMotionRequestReader;
  delete theCalibrationDataWriter;
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

#ifdef NAO_OLD
  platformInterface.registerMotionInput(theDebugMessageIn);
  platformInterface.registerMotionOutput(theDebugMessageOut);
#endif
  g_message("Motion register end");
  

  platformInterface.registerMotionOutputChanel<MotionStatus, Serializer<MotionStatus> >(theBlackBoard.theMotionStatus);
  platformInterface.registerMotionOutputChanel<OdometryData, Serializer<OdometryData> >(theBlackBoard.theOdometryData);
  platformInterface.registerMotionOutputChanel<CalibrationData, Serializer<CalibrationData> >(theBlackBoard.theCalibrationData);

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
    frameNumSinceLastMotionRequest = 0;
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

  // execute head motion firstly
  theHeadMotionEngine.execute();

  // motion engine execute
  selectMotion();
  ASSERT(NULL!=theBlackBoard.currentlyExecutedMotion);
  theBlackBoard.currentlyExecutedMotion->execute(theBlackBoard.theMotionRequest, theBlackBoard.theMotionStatus);
  theBlackBoard.theMotionStatus.currentMotionState = theBlackBoard.currentlyExecutedMotion->state();

  // calibrate the foot touch detector
  if(theBlackBoard.theMotionRequest.calibrateFootTouchDetector)
    theFootTouchCalibrator.execute();

  STOPWATCH_STOP("MotionExecute");
  
  STOPWATCH_START("Motion:postProcess");
  postProcess();
  STOPWATCH_STOP("Motion:postProcess");

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

  theBlackBoard.theInertialPercept = theInertialFilter.filter();
  
  Kinematics::ForwardKinematics::calculateKinematicChainAll(
    theBlackBoard.theAccelerometerData,
    theBlackBoard.theInertialPercept,
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
  PLOT("gyro:x", theBlackBoard.theGyrometerData.data.x);
  PLOT("gyro:y", theBlackBoard.theGyrometerData.data.y);

  PLOT("acc:x", theBlackBoard.theAccelerometerData.data.x);
  PLOT("acc:y", theBlackBoard.theAccelerometerData.data.y);
  PLOT("acc:z", theBlackBoard.theAccelerometerData.data.z);

  PLOT("inertial:x", sin(theBlackBoard.theInertialSensorData.data.x));
  PLOT("inertial:y", (theBlackBoard.theInertialSensorData.data.y));

  PLOT("kinematik_chain:model:x",
    theBlackBoard.theKinematicChainModel.theLinks[KinematicChain::Hip].R.getXAngle()
  );
  PLOT("kinematik_chain:model:y",
    theBlackBoard.theKinematicChainModel.theLinks[KinematicChain::Hip].R.getYAngle()
  );

  DEBUG_REQUEST("Motion:kc_sensor_test",
    RotationMatrix calculatedRotation = 
      Kinematics::ForwardKinematics::calcChestFeetRotation(theBlackBoard.theKinematicChain);

    // calculate expected acceleration sensor reading
    Vector2d inertialExpected(calculatedRotation.getXAngle(), calculatedRotation.getYAngle());

    PLOT("kinematik_chain:sensor:x",
      Math::toDegrees(inertialExpected.x)
    );
    
    PLOT("kinematik_chain:sensor:y",
      Math::toDegrees(inertialExpected.y)
    );
  );
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

