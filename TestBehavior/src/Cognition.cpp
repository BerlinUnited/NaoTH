/* 
 * File:   Cognition.cpp
 * Author: thomas
 * 
 * Created on 10. August 2010, 17:27
 */

#include "Cognition.h"

#include <iostream>
#include <Interface/PlatformInterface/PlatformInterface.h>

using namespace naorunner;

Cognition::Cognition()
{
}

void Cognition::init(PlatformInterface& platformInterface)
{
  std::cout << "Cognition register start" << std::endl;
  platformInterface.registerCognitionInput(theSensorJointData, "SensorJointData");

  platformInterface.registerCognitionOutput(theLEDData, "LEDData"); 
  std::cout << "Cognition register end" << std::endl;
}

void Cognition::call()
{
  std::cout << "Cognition was called" << std::endl;
  std::cout << naorunner::JointData::getJointName(naorunner::JointData::LShoulderRoll)
    << theSensorJointData.position[naorunner::JointData::LShoulderRoll] << std::endl;

  static int idx = 0;
  idx = (idx+1)%naorunner::LEDData::numOfMonoLED;
  theLEDData.theMonoLED[idx] = 2.0 - theLEDData.theMonoLED[idx];
}//end call

Cognition::~Cognition()
{
}

