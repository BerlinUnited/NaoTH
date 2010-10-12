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
#include <Representations/Infrastructure/JointData.h>
#include <Representations/Infrastructure/FrameInfo.h>
#include <Representations/Infrastructure/InertialSensorData.h>
#include <Representations/Infrastructure/FSRData.h>
#include <Representations/Infrastructure/AccelerometerData.h>
#include <Representations/Infrastructure/GyrometerData.h>
#include <PlatformInterface/Callable.h>

class Motion : public naoth::Callable
{
public:
  enum Request
  {
    stand,
    walk_forward,
    turn_left,
    stand_up_from_front,
    stand_up_from_back,
    numOfRequet
  };

  Motion();
  virtual ~Motion();

  template<class PlatformType>
  void init(PlatformType& platformInterface)
  {
    std::cout << "Motion register start" << std::endl;
#define REGISTER_INPUT(R) platformInterface.registerMotionInput(the##R, #R)
    REGISTER_INPUT(SensorJointData);
    REGISTER_INPUT(FrameInfo);
    REGISTER_INPUT(InertialSensorData);
    REGISTER_INPUT(FSRData);
    REGISTER_INPUT(AccelerometerData);
    REGISTER_INPUT(GyrometerData);
#undef REGISTER_INPUT

    platformInterface.registerMotionOutput(theMotorJointData, "MotorJointData");
    std::cout << "Motion register end" << std::endl;
  }

  virtual void call();

  class KeyFrame
  {
  public:
    KeyFrame();
    
    double position[naoth::JointData::numOfJoint];
    double time;
  };

  std::list<KeyFrame> loadKeyFrames(const std::string& filename);

  static void request(Request r);

private:
  int theTimeStep;

  static Request theRequest;
  std::list<KeyFrame> theKeyFrame[numOfRequet];

  std::list<KeyFrame> activeKeyFrame;

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

