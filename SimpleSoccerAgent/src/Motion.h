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
    platformInterface.registerMotionInput(theSensorJointData, "SensorJointData");

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
};

std::istream& operator>>(std::istream& in, Motion::KeyFrame& kf);

#endif	/* MOTION_H */

