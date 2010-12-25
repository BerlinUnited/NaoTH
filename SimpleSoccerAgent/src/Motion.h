/**
 * @file Motion.h
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu Yuan</a>
 *
 */

#ifndef MOTION_H
#define	MOTION_H

#include <iostream>
#include <list>

//
#include <PlatformInterface/PlatformInterface.h>
#include <PlatformInterface/Callable.h>

//
#include <Representations/Infrastructure/JointData.h>
#include <Representations/Infrastructure/FrameInfo.h>
#include <Representations/Infrastructure/InertialSensorData.h>
#include <Representations/Infrastructure/FSRData.h>
#include <Representations/Infrastructure/AccelerometerData.h>
#include <Representations/Infrastructure/GyrometerData.h>

#include "BlackBoard.h"

class Motion : public naoth::Callable
{
public:

  Motion();
  virtual ~Motion();

  /** */
  void init(naoth::PlatformDataInterface& platformInterface)
  {
    std::cout << "Motion register start" << std::endl;
    
    //
    platformInterface.registerMotionInput(theSensorJointData);
    platformInterface.registerMotionInput(theFrameInfo);
    platformInterface.registerMotionInput(theInertialSensorData);
    platformInterface.registerMotionInput(theFSRData);
    platformInterface.registerMotionInput(theAccelerometerData);
    platformInterface.registerMotionInput(theGyrometerData);

    //
    platformInterface.registerMotionOutput(theMotorJointData);
    
    std::cout << "Motion register end" << std::endl;
  }//end init

  virtual void call();

  class KeyFrame
  {
  public:
    KeyFrame();
    
    double position[naoth::JointData::numOfJoint];
    double time;
  };

  std::list<KeyFrame> loadKeyFrames(const std::string& filename);

private:
  int theTimeStep;

  std::list<KeyFrame> theKeyFrame[MotionRequest::numOfRequestID];

  std::list<KeyFrame> activeKeyFrame;

  //
  naoth::SensorJointData theSensorJointData;
  naoth::MotorJointData theMotorJointData;
  naoth::FrameInfo theFrameInfo;
  naoth::InertialSensorData theInertialSensorData;
  naoth::FSRData theFSRData;
  naoth::AccelerometerData theAccelerometerData;
  naoth::GyrometerData theGyrometerData;
};

std::istream& operator>>(std::istream& in, Motion::KeyFrame& kf);

#endif	/* MOTION_H */

