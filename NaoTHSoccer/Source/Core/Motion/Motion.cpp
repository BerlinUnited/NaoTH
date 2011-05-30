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
#include "Core/Tools/SwapSpace/SwapSpace.h"
#include "Engine/InitialMotion/InitialMotionFactory.h"
#include "Engine/InverseKinematicsMotion/InverseKinematicsMotionFactory.h"
#include "Engine/KeyFrameMotion/KeyFrameMotionEngine.h"

Motion::Motion():theBlackBoard(MotionBlackBoard::getInstance())
{
  theSupportPolygonGenerator.init(theBlackBoard.theFSRData.force,
    theBlackBoard.theFSRPos,
    theBlackBoard.theKinematicChain.theLinks);
    
  theMotionFactories.push_back(new InitialMotionFactory());
  theMotionFactories.push_back(new InverseKinematicsMotionFactory());
  theMotionFactories.push_back(new KeyFrameMotionEngine());
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

void Motion::init(naoth::PlatformDataInterface& platformInterface)
{
  theBlackBoard.init();
  theBlackBoard.currentlyExecutedMotion = &theEmptyMotion;
  
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
  g_message("Motion register end");
  
}//end init


void Motion::call()
{
  // TODO
  //STOPWATCH_START("MotionExecute");
  
  // process sensor data
  processSensorData();
  
  // get orders from cognition
  /*SwapSpace::getInstance().theCognitionCache.pull(
    theBlackBoard.theHeadMotionRequest,
    theBlackBoard.theMotionRequest
  );*/
    // FAKE Motion Request:
  theBlackBoard.theMotionRequest.time = theBlackBoard.theMotionStatus.time;
  if ( theBlackBoard.theMotionStatus.currentMotion == motion::EMPTY )
    theBlackBoard.theMotionRequest.id = motion::INIT;
  else if ( theBlackBoard.theMotionStatus.currentMotion == motion::INIT )
    theBlackBoard.theMotionRequest.id = motion::STAND;
  else if ( theBlackBoard.theMotionStatus.currentMotion == motion::STAND )
    theBlackBoard.theMotionRequest.id = motion::DANCE;

  // execute head motion firstly
  theHeadMotionEngine.execute();

  // motion engine execute
  selectMotion();
  ASSERT(NULL!=theBlackBoard.currentlyExecutedMotion);
  theBlackBoard.currentlyExecutedMotion->execute(theBlackBoard.theMotionRequest, theBlackBoard.theMotionStatus);
  //cout<<theBlackBoard.currentlyExecutedMotion->getName()<<endl;
  // TODO
  //STOPWATCH_STOP("MotionExecute");
  
  postProcess();
}//end call

void Motion::processSensorData()
{
  // check all joint stiffness
  theBlackBoard.theSensorJointData.checkStiffness();
  
  Kinematics::ForwardKinematics::calculateKinematicChainAll(
    theBlackBoard.theAccelerometerData,
    theBlackBoard.theInertialSensorData,
    theBlackBoard.theKinematicChain,
    theBlackBoard.theFSRPos,
    theBlackBoard.theFrameInfo.getBasicTimeStepInSecond());

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
}//end processSensorData

void Motion::postProcess()
{
  MotorJointData& mjd = theBlackBoard.theMotorJointData;
  double basicStepInS = theBlackBoard.theFrameInfo.getBasicTimeStepInSecond();

#ifdef DEBUG
  mjd.checkStiffness();
#endif

  mjd.clamp();
  mjd.updateSpeed(theBlackBoard.theLastMotorJointData, basicStepInS);
  mjd.updateAcceleration(theBlackBoard.theLastMotorJointData, basicStepInS);

  SwapSpace::getInstance().theMotionCache.push(
    theBlackBoard.theMotionStatus,
    theBlackBoard.theOdometryData
    );
    
#ifdef DEBUG
//TODO
  //MotionDebug::getInstance().execute();
#endif
}

void Motion::selectMotion()
{
  // test if the current MotionStatus allready arrived in cognition
  if ( theBlackBoard.theMotionStatus.time > theBlackBoard.theMotionRequest.time )
    return;

  if (theBlackBoard.theMotionStatus.currentMotion == theBlackBoard.theMotionRequest.id
    && !theBlackBoard.currentlyExecutedMotion->isRunning())
  {
    changeMotion(&theEmptyMotion);
  }

  if (theBlackBoard.theMotionStatus.currentMotion != theBlackBoard.theMotionRequest.id
    &&
    (!theBlackBoard.currentlyExecutedMotion->isRunning() || theBlackBoard.theMotionRequest.forced))
  {
    AbstractMotion* newMotion = NULL;
    for ( std::list<MotionFactory*>::iterator iter=theMotionFactories.begin(); 
          NULL==newMotion && iter!=theMotionFactories.end(); ++iter)
    {
      newMotion = (*iter)->createMotion(theBlackBoard.theMotionRequest);
    }

/*
    if (newMotion == NULL)
      newMotion = theInverseKinematicsMotionFactory.createMotion(theBlackBoard.theMotionRequest);

    if (newMotion == NULL)
      newMotion = theParallelKinematicMotionFactory.createMotion(theBlackBoard.theMotionRequest);

    if(newMotion == NULL)
      newMotion = theNeuralMotionFactory.createMotion(theBlackBoard.theMotionRequest);

    if (newMotion == NULL)
      newMotion = theKeyFrameMotionEngine.createMotion(theBlackBoard.theMotionRequest);

    if (newMotion == NULL)
      newMotion = theDebugMotionEngine.createMotion(theBlackBoard.theMotionRequest);
*/
    if (NULL != newMotion)
    {
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
  theBlackBoard.theMotionStatus.time = theBlackBoard.theFrameInfo.time;
}//end changeMotion
