/**
 * @file MotionBlackBoard.h
 *
  * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 *
 */

#ifndef _MOTIONBLACKBOARD_H
#define  _MOTIONBLACKBOARD_H


//#include "PlatformInterface/PlatformInterchangeable.h"

// representations
#include <Representations/Infrastructure/FrameInfo.h>
#include <Representations/Infrastructure/FSRData.h>
#include <Representations/Infrastructure/JointData.h>
#include <Representations/Infrastructure/AccelerometerData.h>
#include <Representations/Infrastructure/GyrometerData.h>
#include <Representations/Infrastructure/InertialSensorData.h>
#include <Representations/Infrastructure/BumperData.h>
#include <Representations/Perception/CameraMatrix.h>
#include <Representations/Infrastructure/LEDData.h>
#include <Representations/Infrastructure/RobotInfo.h>

#include "Representations/Perception/InertialPercept.h"
#include "Representations/Infrastructure/CalibrationData.h"

#include "Representations/Motion/Request/HeadMotionRequest.h"
#include "Representations/Motion/Request/MotionRequest.h"
#include "Representations/Motion/MotionStatus.h"

#include "Representations/Modeling/OdometryData.h"
#include "Representations/Modeling/KinematicChain.h"
#include "Representations/Modeling/SupportPolygon.h"


class AbstractMotion;

class MotionBlackBoard : public naoth::Singleton<MotionBlackBoard>
{
private:
  friend class naoth::Singleton<MotionBlackBoard>;

  MotionBlackBoard();

public:

  void init();

  ~MotionBlackBoard();

  naoth::RobotInfo theRobotInfo;

  // data used internally
  KinematicChain theKinematicChain; // data based on sensors
  KinematicChain theKinematicChainModel; // data based on joint command (motor joint data)
  SupportPolygon theSupportPolygon;

  Vector3<double> theFSRPos[naoth::FSRData::numOfFSR];
  naoth::FrameInfo theFrameInfo;
  naoth::AccelerometerData theAccelerometerData;
  naoth::GyrometerData theGyrometerData;
  naoth::InertialSensorData theInertialSensorData;
  naoth::FSRData theFSRData;
  naoth::MotorJointData theMotorJointData;
  naoth::MotorJointData theLastMotorJointData;
  naoth::SensorJointData theSensorJointData;
  naoth::LEDData theLEDData;

  CalibrationData theCalibrationData;
  
  // SerialSensorData theSerialSensorData;
  // SerialSensorDataRequest theSerialSensorDataRequest;
  // BumperData theBumperData;
  CameraMatrix theCameraMatrix;
  MotionStatus theMotionStatus;
  OdometryData theOdometryData;
  InertialPercept theInertialPercept;

  // data copied from cognition
  HeadMotionRequest theHeadMotionRequest;
  MotionRequest theMotionRequest;
  
  AbstractMotion* currentlyExecutedMotion;
};

#endif  /* _MOTIONBLACKBOARD_H */

