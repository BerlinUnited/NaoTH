/**
 * @file Motion.cpp
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu Yuan</a>
 *
 */

#include "Motion.h"

#include <iostream>
#include <fstream>
#include <PlatformInterface/Platform.h>

using namespace std;
using namespace naoth;

Motion::KeyFrame::KeyFrame()
{
  for (int i = 0; i < JointData::numOfJoint; i++)
  {
    position[i] = 0;
  }
  time = 0;
}

Motion::Motion():
  theTimeStep(20)
{
  string dir = "keyframes/";
  theKeyFrame[MotionRequest::stand] = loadKeyFrames(dir+"stand.txt");
  theKeyFrame[MotionRequest::walk_forward] = loadKeyFrames(dir+"walk_forward.txt");
  theKeyFrame[MotionRequest::turn_left] = loadKeyFrames(dir+"turn_left.txt");
  theKeyFrame[MotionRequest::stand_up_from_front] = loadKeyFrames(dir+"stand_up_from_front.txt");
  theKeyFrame[MotionRequest::stand_up_from_back] = loadKeyFrames(dir+"stand_up_from_back.txt");
  
  for (int i = 0; i < JointData::numOfJoint; i++)
  {
    getMotorJointData().stiffness[i] = 1.0;
  }
}

void Motion::init(naoth::ProcessInterface& platformInterface, const naoth::PlatformBase& platform)
{
  getMotionStatus().currentlyExecutedMotion = MotionRequest::stand;

  // init robot info
  getRobotInfo().platform = platform.getName();
  getRobotInfo().bodyNickName = platform.getBodyNickName();
  getRobotInfo().bodyID = platform.getBodyID();
  getRobotInfo().basicTimeStep = platform.getBasicTimeStep();

  g_message("Motion register begin");
#define REG_INPUT(R)                                                    \
  platformInterface.registerInput(get##R())

  REG_INPUT(SensorJointData);
  REG_INPUT(FrameInfo);
  REG_INPUT(InertialSensorData);
  REG_INPUT(FSRData);
  REG_INPUT(AccelerometerData);
  REG_INPUT(GyrometerData);

#define REG_OUTPUT(R)                                                   \
  platformInterface.registerOutput(get##R())

  REG_OUTPUT(MotorJointData);
  REG_OUTPUT(LEDData);

  platformInterface.registerInputChanel(getMotionRequest());
}//end init

void Motion::call()
{
  MotionRequest::MotionID theRequest = getMotionRequest().id;
  if ( activeKeyFrame.empty() )
  {
    activeKeyFrame = theKeyFrame[theRequest];
  }

  KeyFrame& frame = activeKeyFrame.front();

  double dt = theTimeStep/frame.time;
  for(int i=0; i<JointData::numOfJoint; i++)
  {
    getMotorJointData().position[i]
        = (1.0-dt)*getMotorJointData().position[i] + dt*frame.position[i];
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
  if(!ifile.good()) 
  {
    cout << "ERROR: couldn't load the file " << filename << endl;
    exit(-1);
  }

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
