/* 
 * File:   Motion.h
 * Author: thomas
 *
 * Created on 10. August 2010, 17:27
 */

#ifndef MOTION_H
#define	MOTION_H

#include <iostream>
#include <list>
#include <Interface/Representations/Infrastructure/JointData.h>
#include <Interface/PlatformInterface/Callable.h>

class Motion : public naorunner::Callable
{
public:
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
    
    double position[naorunner::JointData::numOfJoint];
    double time;
  };

  std::list<KeyFrame> loadKeyFrames(const std::string& filename);

private:
  int theTimeStep;

  std::list<KeyFrame> testKeyFrame;

  std::list<KeyFrame> activeKeyFrame;
  KeyFrame theLastKeyFrame;
  int theKeyFrameTime;

  naorunner::SensorJointData theSensorJointData;
  naorunner::MotorJointData theMotorJointData;
};

std::istream& operator>>(std::istream& in, Motion::KeyFrame& kf);

#endif	/* MOTION_H */

