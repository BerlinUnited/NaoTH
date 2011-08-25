/**
 * @file MotionBlackBoard.h
 *
  * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 *
 */

#ifndef _MOTIONBLACKBOARD_H
#define	_MOTIONBLACKBOARD_H


//#include "PlatformInterface/PlatformInterchangeable.h"

// representations
#include <Representations/Infrastructure/FrameInfo.h>
#include <Representations/Infrastructure/FSRData.h>
#include <Representations/Infrastructure/JointData.h>
#include <Representations/Infrastructure/AccelerometerData.h>
#include <Representations/Infrastructure/GyrometerData.h>
#include <Representations/Infrastructure/InertialSensorData.h>
#include <Representations/Infrastructure/BumperData.h>
#include <Representations/Infrastructure/LEDData.h>
#include <Representations/Infrastructure/RobotInfo.h>

#include "Representations/Motion/MotionRequest.h"

#include "Representations/Modeling/OdometryData.h"


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

  // SerialSensorData theSerialSensorData;
  // SerialSensorDataRequest theSerialSensorDataRequest;
  // BumperData theBumperData;
  OdometryData theOdometryData;

  // data copied from cognition
  MotionRequest theMotionRequest;
  
  MotionRequest::MotionID currentlyExecutedMotion;
};

#endif	/* _MOTIONBLACKBOARD_H */

