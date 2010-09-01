/**
 * @file Cognition.cpp
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu Yuan</a>
 *
 */

#include "Cognition.h"
#include "Motion.h"

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
  perception();

  decide();
}//end call

Cognition::~Cognition()
{
}

void Cognition::perception()
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
  if (!isStandingUp)
  {
    if (theInertialSensorData.data[InertialSensorData::Y] > 0)
      Motion::request(Motion::stand_up_from_front);
    else
      Motion::request(Motion::stand_up_from_back);
  }
  else if ( theBall.wasSeen )
  {
    Motion::request(Motion::walk_forward);
  }
  else
  {
    Motion::request(Motion::turn_left);
  }
}
