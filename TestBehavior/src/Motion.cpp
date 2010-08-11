/* 
 * File:   Motion.cpp
 * Author: thomas
 * 
 * Created on 10. August 2010, 17:27
 */

#include "Motion.h"

#include <iostream>

#include <naorunner/PlatformInterface/Platform.h>

Motion::Motion()
: countUp(false)
{
}

void Motion::init()
{
  std::cout << "Motion Initialize" << std::endl;
  naorunner::Platform& pl = naorunner::Platform::getInstance();
  pl.thePlatformInterface->registerMotionInput(&theSensorJointData, "SensorJointData");
  pl.thePlatformInterface->registerMotionOutput(&theMotorJointData, "MotorJointData");
}

void Motion::call()
{
  std::cout << "Motion was called" << std::endl;

  theMotorJointData.hardness[naorunner::JointData::LShoulderRoll] = 0.9;
  std::cout << "sensor pos=" << theSensorJointData.position[naorunner::JointData::LShoulderRoll] << std::endl;
  theMotorJointData.position[naorunner::JointData::LShoulderRoll]
   = countUp ? theSensorJointData.position[naorunner::JointData::LShoulderRoll] + 0.1 :
     theSensorJointData.position[naorunner::JointData::LShoulderRoll] - 0.1;
  // clip
  if(fabs(theMotorJointData.position[naorunner::JointData::LShoulderRoll]) > 0.5)
  {
    countUp = !countUp;
  }
}

Motion::~Motion()
{
}

