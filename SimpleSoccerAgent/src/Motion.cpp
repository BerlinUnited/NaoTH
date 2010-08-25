/* 
 * File:   Motion.cpp
 * Author: thomas
 * 
 * Created on 10. August 2010, 17:27
 */

#include "Motion.h"

#include <iostream>
#include <Interface/PlatformInterface/Platform.h>

using namespace std;
using namespace naorunner;

Motion::KeyFrame::KeyFrame()
{
  for (int i = 0; i < JointData::numOfJoint; i++)
  {
    position[i] = 0;
  }
  time = 0;
}

Motion::Motion():
theTimeStep(20),
  theKeyFrameTime(0)
{
  KeyFrame open;
  open.position[JointData::LShoulderRoll] = Math::fromDegrees(90);
  open.position[JointData::RShoulderRoll] = Math::fromDegrees(-90);
  open.time = 1000;

  KeyFrame close;
  close.time = 1000;

  testKeyFrame.push_back(open);
  testKeyFrame.push_back(close);

  for (int i = 0; i < JointData::numOfJoint; i++)
  {
    theMotorJointData.hardness[i] = 1.0;
  }
}

void Motion::call()
{
  if ( activeKeyFrame.empty() )
  {
    activeKeyFrame = testKeyFrame;
  }

  KeyFrame& frame = activeKeyFrame.front();

  double t = theKeyFrameTime / frame.time;
  for(int i=0; i<JointData::numOfJoint; i++)
  {
    theMotorJointData.position[i] = theLastKeyFrame.position[i] * (1-t) + frame.position[i] * t;
  }

  theKeyFrameTime += theTimeStep;
  if (frame.time <= theKeyFrameTime)
  {
    theLastKeyFrame = frame;
    theKeyFrameTime = 0;
    activeKeyFrame.pop_front();
  }

}//end call

Motion::~Motion()
{
}

