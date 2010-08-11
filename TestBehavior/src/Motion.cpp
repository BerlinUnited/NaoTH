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
: countUp(true),pos(0)
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
  theMotorJointData.hardness[naorunner::JointData::RShoulderRoll] = 0.9;

  pos = countUp ? pos + 0.01 : pos - 0.01;
  if(pos >= 0.9 || pos <= 0)
  {
    countUp = !countUp;
  }
  theMotorJointData.position[naorunner::JointData::LShoulderRoll] = pos;
  theMotorJointData.position[naorunner::JointData::RShoulderRoll] = -pos;

  std::cout << "sensor pos=" << pos << std::endl;

  naorunner::Platform::getInstance().thePlatformInterface->setMotionOutput();
}

Motion::~Motion()
{
}

