/* 
 * File:   Cognition.cpp
 * Author: thomas
 * 
 * Created on 10. August 2010, 17:27
 */

#include "Cognition.h"

#include <iostream>
#include <Interface/PlatformInterface/PlatformInterface.h>
#include <map>

using namespace std;
using namespace naorunner;

Cognition::Cognition():
isStandingUp(true)
{
}

void Cognition::call()
{
  percetion();

  decide();
}//end call

Cognition::~Cognition()
{
}

void Cognition::percetion()
{
  // update ball percept
  std::map<std::string, Vector3<double> >::const_iterator ballData = theVirtualVision.data.find("B");
  if (ballData != theVirtualVision.data.end())
  {
    theBall.wasSeen = true;
    theBall.distance = ballData->second[0];
  }
  else
  {
    theBall.wasSeen = false;
  }

  // update if the robot is standing up
  if ( abs(theInertialSensorData.data[InertialSensorData::X]) < Math::fromDegrees(45)
    && abs(theInertialSensorData.data[InertialSensorData::Y]) < Math::fromDegrees(45) )
    isStandingUp = true;
  else
    isStandingUp = false;
}

void Cognition::decide()
{
}
