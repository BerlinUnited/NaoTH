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

Cognition::Cognition()
{
}

void Cognition::call()
{
  sense();

  think();
}//end call

Cognition::~Cognition()
{
}

void Cognition::sense()
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
}

void Cognition::think()
{

}
