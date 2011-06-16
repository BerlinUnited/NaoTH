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
#include "Engine/KeyFrameMotion/KeyFrameMotionEngine.h"

Motion::Motion():theBlackBoard(MotionBlackBoard::getInstance()),
theMotionStatusWriter(NULL),
theOdometryDataWriter(NULL),
theHeadMotionRequestReader(NULL),
theMotionRequestReader(NULL),
  frameNumSinceLastMotionRequest(0)
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
  
  if (theMotionStatusWriter != NULL)
    delete theMotionStatusWriter;
  if (theOdometryDataWriter != NULL)
    delete theOdometryDataWriter;
  if (theHeadMotionRequestReader != NULL)
    delete theHeadMotionRequestReader;
  if (theMotionRequestReader != NULL)
    delete theMotionRequestReader;
}

void Motion::init(naoth::PlatformInterfaceBase& platformInterface)
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
  REG_OUTPUT(LEDData);
  g_message("Motion register end");
  
  theMotionStatusWriter = new MessageWriter(platformInterface.getMessageQueue("MotionStatus"));
  theOdometryDataWriter = new MessageWriter(platformInterface.getMessageQueue("OdometryData"));
  
  theHeadMotionRequestReader = new MessageReader(platformInterface.getMessageQueue("HeadMotionRequest"));
  theMotionRequestReader = new MessageReader(platformInterface.getMessageQueue("MotionRequest"));
}//end init


void Motion::call()
{
  // TODO
  //STOPWATCH_START("MotionExecute");
  
  // process sensor data
  processSensorData();
  
  // get orders from cognition
  if ( !theHeadMotionRequestReader->empty() )
  {
    string msg = theHeadMotionRequestReader->read();
    stringstream ss(msg);
    Serializer<HeadMotionRequest>::deserialize(ss, theBlackBoard.theHeadMotionRequest);
  }
  
  if ( !theMotionRequestReader->empty() )
  {
    string msg = theMotionRequestReader->read();
    stringstream ss(msg);
    Serializer<MotionRequest>::deserialize(ss, theBlackBoard.theMotionRequest);
    frameNumSinceLastMotionRequest = 0;
  }
  else
  {
    frameNumSinceLastMotionRequest++;
  }

  checkWarningState();

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

  // send message to cognition
  stringstream msmsg;
  Serializer<MotionStatus>::serialize(theBlackBoard.theMotionStatus, msmsg);
  theMotionStatusWriter->write(msmsg.str());
  
  stringstream odmsg;
  Serializer<OdometryData>::serialize(theBlackBoard.theOdometryData, odmsg);
  theOdometryDataWriter->write(odmsg.str());
    
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
  theBlackBoard.theMotionStatus.time = theBlackBoard.theFrameInfo.time;
}//end changeMotion

bool Motion::exit()
{
  theBlackBoard.theMotionRequest.id = motion::init;
  theBlackBoard.theMotionRequest.time = theBlackBoard.theMotionStatus.time;
  
  return (theBlackBoard.currentlyExecutedMotion != NULL)
    && ( theBlackBoard.currentlyExecutedMotion->getId() == motion::init)
    && ( theBlackBoard.currentlyExecutedMotion->isFinished() );
}//end exit

void Motion::checkWarningState()
{
  // check if cognition is running
  if ( frameNumSinceLastMotionRequest*theBlackBoard.theFrameInfo.getBasicTimeStepInSecond() > 1 )
  {
    theBlackBoard.theMotionRequest.id = motion::init;
    theBlackBoard.theMotionRequest.time = theBlackBoard.theMotionStatus.time;
    theBlackBoard.theLEDData.change = true;

    LEDData& theLEDData = theBlackBoard.theLEDData;
    int begin = int(frameNumSinceLastMotionRequest*theBlackBoard.theFrameInfo.getBasicTimeStepInSecond()*10)%10;
    theLEDData.theMonoLED[LEDData::EarRight0 + begin] = 0;
    theLEDData.theMonoLED[LEDData::EarLeft0 + begin] = 0;
    int end = (begin+2)%10;
    theLEDData.theMonoLED[LEDData::EarRight0 + end] = 1;
    theLEDData.theMonoLED[LEDData::EarLeft0 + end] = 1;

    for(int i=0; i<LEDData::numOfMultiLED; i++)
    {
      theLEDData.theMultiLED[i][LEDData::RED] = 0;
      theLEDData.theMultiLED[i][LEDData::GREEN] = 0;
      theLEDData.theMultiLED[i][LEDData::BLUE] = 1;
    }
  }
  else
  {
    theBlackBoard.theLEDData.change = false;
  }
}
