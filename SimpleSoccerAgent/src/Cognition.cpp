/* 
 * File:   Cognition.cpp
 * Author: thomas
 * 
 * Created on 10. August 2010, 17:27
 */

#include "Cognition.h"

#include <iostream>
#include <Interface/PlatformInterface/PlatformInterface.h>

using namespace std;
using namespace naorunner;

Cognition::Cognition()
{
}

void Cognition::call()
{
  cout<<"Game Mode = "<<theSimSparkGameInfo.thePlayMode<<endl;
//  std::cout << naorunner::JointData::getJointName(naorunner::JointData::LShoulderRoll)
//    << theSensorJointData.position[naorunner::JointData::LShoulderRoll] << std::endl;
}//end call

Cognition::~Cognition()
{
}

