/**
* @file KeyFrameMotion.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Implementation of class KeyFrameMotion
*/

#include "KeyFrameMotion.h"

KeyFrameMotion::KeyFrameMotion(const MotionNet& currentMotionNet, motion::MotionID id)
  : 
  AbstractMotion(id),
  name(motion::getName(id)),
  currentMotionNet(currentMotionNet),
  t(0.0),
  stiffness(0.7)
{
}

KeyFrameMotion::KeyFrameMotion()
  :
  AbstractMotion(motion::num_of_motions),
  t(0.0),
  stiffness(0.7)
{
}

//KeyFrameMotion::KeyFrameMotion(const KeyFrameMotion& other)
//:AbstractMotion(MotionRequestID::numOfMotion)
//{
//  *this = other;
//}
//
//KeyFrameMotion& KeyFrameMotion::operator = (const KeyFrameMotion& src)
//{
//  theId = src.theId;
//  name = src.name;
//  currentMotionNet = src.currentMotionNet;
//  t = src.t;
//  currentKeyFrame = src.currentKeyFrame;
//  currentTransition = src.currentTransition;
//  return *this;
//}

KeyFrameMotion::~KeyFrameMotion(){}


void KeyFrameMotion::init()
{
  // ensure the morion net is not empty
  ASSERT(!currentMotionNet.isEmpty());
  currentKeyFrame = currentMotionNet.getKeyFrame(0);


  // the distance between the current state and the first key frame
  double distance = 0;
  stiffness = 0.9; //0.7;// only this value is tested!!!
  for(int joint = 0; joint < currentMotionNet.getNumOfJoints(); joint++)
  {
    JointData::JointID id = currentMotionNet.getJointID(joint);
    lastMotorJointData.position[id] = theBlackBoard.theSensorJointData.position[id];
    lastMotorJointData.stiffness[id] = stiffness;
    double joindDistance = fabs(Math::normalize(lastMotorJointData.position[id] - currentKeyFrame.jointData[joint]));
    distance = max(joindDistance, distance);
  }//end for


  double maxAngleSpeed = Math::pi_2; // radiant per second
  if(name == "fall_left" || name == "fall_right")
  {
    maxAngleSpeed = Math::pi2;
  }

  // set the initial transotion
  currentTransition.condition = "*";
  // ms needed for the translation to the first key frame
  currentTransition.duration = distance/maxAngleSpeed*1000.0;
  currentTransition.toKeyFrame = 0;
  currentTransition.fromKeyFrame = 0;

  t = currentTransition.duration;
}//end init

void KeyFrameMotion::execute(const MotionRequest& motionRequest, MotionStatus& /*motionStatus*/)
{
  std::string condition("stop");
  if(motion::getName(motionRequest.id) == name)
  {
    condition = "run";
  }

  double timeStep = theBlackBoard.theFrameInfo.basicTimeStep;

  if(currentState == motion::stopped)
  {
    init();
    currentState = motion::running;
  }//end if

  // skip frames if necessary
  while(t <= timeStep)
  {
    timeStep -= t;
    //get next transition
    getNextTransition(condition);

    if(currentState == motion::stopped)
    {
      return;
    }//end if
    //DOUT("Keyframe: " << fromKeyFrame.id << " <--- " << toKeyFrame.id << " " << condition << "|" << currentMotionNetName << "|" << transition.toMotionNetName << "\n");
  }//end while

  // here is allways timeStep < t
  double dt = timeStep/t;
  for(int joint = 0; joint < currentMotionNet.getNumOfJoints(); joint++)
  {
    JointData::JointID id = currentMotionNet.getJointID(joint);
    lastMotorJointData.position[id] = (1.0-dt)*lastMotorJointData.position[id] + dt*currentKeyFrame.jointData[joint];
    
    // set the joint data (the only place where theMotorJointData is set)
    theMotorJointData.stiffness[id] = stiffness;
    theMotorJointData.position[id] = lastMotorJointData.position[id];
  }//end for

  t -= timeStep;
}//end execute



void KeyFrameMotion::getNextTransition(std::string condition)
{
  bool transitionFound = false;

  for(size_t i = 0; i < currentMotionNet.getNumOfTransition(); i++)
  {
    if(currentMotionNet.getTransition(i).fromKeyFrame == currentKeyFrame.id)
    {
      if(currentMotionNet.getTransition(i).condition == condition)
      {
        currentTransition = currentMotionNet.getTransition(i);
        transitionFound = true;
        break;
      }
      else if(currentMotionNet.getTransition(i).condition == "*")
      {
        currentTransition = currentMotionNet.getTransition(i);
        transitionFound = true;
      }
    }//end if
  }//end for

  if(transitionFound)
  {
    currentKeyFrame = currentMotionNet.getKeyFrame(currentTransition.toKeyFrame);
    t = currentTransition.duration;
  }else
  {
    currentState = motion::stopped;
  }

  //printf("Transition[%s]: %d --> %d(%s)\n",condition,transition.fromKeyFrame,transition.toKeyFrame,transition.toMotionNetName);
}//end getNextTransition

void KeyFrameMotion::print(ostream& stream) const
{
  stream << "Name:               " << name << endl;
  stream << "#KeyFrame:          " << currentMotionNet.getNumOfKeyFrame() << endl;
  stream << "#Transitions:       " << currentMotionNet.getNumOfTransition() << endl;
  stream << "Current KeyFrame:   " << currentKeyFrame.id << endl;
  stream << "Current Transition: " << currentTransition << endl;
  stream << "Current Time:       " << t << endl;
}//end print


