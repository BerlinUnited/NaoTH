/**
 * @file Motion.cpp
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu Yuan</a>
 *
 */

#include "Motion.h"

#include <iostream>
#include <fstream>
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

Motion::Request Motion::theRequest = Motion::stand;

Motion::Motion():
theTimeStep(20)
{
  string dir = "keyframes/";
  theKeyFrame[stand] = loadKeyFrames(dir+"stand.txt");
  theKeyFrame[walk_forward] = loadKeyFrames(dir+"walk_forward.txt");
  theKeyFrame[turn_left] = loadKeyFrames(dir+"turn_left.txt");
  theKeyFrame[stand_up_from_front] = loadKeyFrames(dir+"stand_up_from_front.txt");
  theKeyFrame[stand_up_from_back] = loadKeyFrames(dir+"stand_up_from_back.txt");
  
  for (int i = 0; i < JointData::numOfJoint; i++)
  {
    theMotorJointData.hardness[i] = 1.0;
  }
}

void Motion::call()
{
  if ( activeKeyFrame.empty() )
  {
    activeKeyFrame = theKeyFrame[theRequest];
  }

  KeyFrame& frame = activeKeyFrame.front();

  double dt = theTimeStep/frame.time;
  for(int i=0; i<JointData::numOfJoint; i++)
  {
    theMotorJointData.position[i] = (1.0-dt)*theMotorJointData.position[i] + dt*frame.position[i];
  }

  frame.time -= theTimeStep;
  if (frame.time <= theTimeStep)
  {
    activeKeyFrame.pop_front();
  }

}//end call

Motion::~Motion()
{
}

std::list<Motion::KeyFrame> Motion::loadKeyFrames(const std::string& filename)
{
  std::list<Motion::KeyFrame> keyFrameList;
  ifstream ifile(filename.c_str());
  while ( !ifile.eof() )
  {
    KeyFrame k;
    ifile>>k;
    if ( k.time > 0 ) keyFrameList.push_back(k);
  }

  return keyFrameList;
}

std::istream& operator>>(std::istream& in, Motion::KeyFrame& kf)
{
  in >> kf.time;
  for(int i=0; i<JointData::numOfJoint; i++)
  {
    in >> kf.position[i];
    kf.position[i] = Math::fromDegrees(kf.position[i]);
  }
  
  return in;
}

void Motion::request(Request r)
{
  theRequest = r;
}
