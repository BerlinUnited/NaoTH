/**
* @file MotionNet.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Implementation of class MotionNet
*/

#include "MotionNet.h"
#include "Tools/Math/Common.h"

using namespace naoth;
using namespace std;

MotionNet::MotionNet() 
  : numberOfJoints(0)
{
}

MotionNet::~MotionNet(){}


void MotionNet::add(const MotionNet::KeyFrame& keyFrame)
{
  keyFrameArray.push_back(keyFrame);
}//add

void MotionNet::add(const MotionNet::Transition& transition)
{
  transitionArray.push_back(transition);
}//add

void MotionNet::add(const JointData::JointID& id)
{
  if(numberOfJoints < JointData::numOfJoint)
  {
    jointIndex[numberOfJoints++] = id;
  }//end if
}//add


const MotionNet::KeyFrame& MotionNet::getKeyFrame(int id) const
{
  for(size_t i = 0; i < keyFrameArray.size(); i++)
  {
    if(id == keyFrameArray[i].id) return keyFrameArray[i];
  }//end for

  throw "invalide KeyFrame";
}//end getKeyFrame

MotionNet::KeyFrame& MotionNet::getMutableKeyFrame(int id)
{
  for(size_t i = 0; i < keyFrameArray.size(); i++)
  {
    if(id == keyFrameArray[i].id) return keyFrameArray[i];
  }//end for

  throw "invalide KeyFrame";
}//end getKeyFrame

ostream& operator<<(ostream& stream, const MotionNet::KeyFrame& keyFrame)
{
  stream << "KEYFRAME: id: " << keyFrame.id;
  stream << "{" << keyFrame.jointData[0];
  for(int i = 1; i < JointData::numOfJoint; i++)
  {
    stream << "," << Math::toDegrees(keyFrame.jointData[i]);
  }
  stream << "}";

  return stream;
}

ostream& operator<<(ostream& stream, const MotionNet::Transition& transition)
{
  stream << "TRANSITION: " 
         << transition.fromKeyFrame
         <<" --" << transition.condition 
         <<"--> " << transition.toKeyFrame;

  return stream;
}

ostream& operator<<(ostream& stream, const MotionNet& motionNet)
{
  for(size_t i = 0; i < motionNet.getNumOfKeyFrame(); i++)
  {
    stream << motionNet.keyFrameArray[i] << endl;
  }//end for

  for(size_t i = 0; i < motionNet.getNumOfTransition(); i++)
  {
    stream << motionNet.transitionArray[i] << endl;
  }//end for

  return stream;
}

