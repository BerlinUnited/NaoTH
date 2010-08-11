/* 
 * File:   Motion.h
 * Author: thomas
 *
 * Created on 10. August 2010, 17:27
 */

#ifndef MOTION_H
#define	MOTION_H

#include <naorunner/Representations/Infrastructure/JointData.h>
#include <naorunner/PlatformInterface/Callable.h>

class Motion : public naorunner::Callable
{
public:
  Motion();
  virtual ~Motion();

  virtual void init();

  virtual void call();

private:

  bool countUp;
  naorunner::SensorJointData theSensorJointData;
  naorunner::MotorJointData theMotorJointData;
};

#endif	/* MOTION_H */

