/**
* @file AbstractMotion.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* Declaration of class AbstractMotion
*/

#ifndef __AbstractMotion_h_
#define __AbstractMotion_h_

#include "MotionBlackBoard.h"

class AbstractMotion
{
private:
  motion::MotionID theId;
  
protected:

  motion::State currentState;
  naoth::MotorJointData& theMotorJointData;
  const MotionBlackBoard& theBlackBoard;

public:

  AbstractMotion(motion::MotionID id);

  virtual ~AbstractMotion(){};

  virtual void init(){}
  virtual void execute(const MotionRequest& motionRequest, MotionStatus& moitonStatus) = 0;

  bool isStopped() const {return currentState == motion::stopped; }

  virtual bool isFinish() const { return isStopped(); }

  motion::MotionID getId() const { return theId; }
  
  std::string getName() const { return motion::getName(theId); } 

  motion::State state() const { return currentState; }

protected:
  /** set the stiffness with max changes */
  bool setStiffness(double* hardness, double delta,
                    naoth::JointData::JointID begin=naoth::JointData::HeadPitch,
                    naoth::JointData::JointID end=naoth::JointData::numOfJoint);
};

class EmptyMotion: public AbstractMotion
{
public:
  EmptyMotion():AbstractMotion(motion::empty)
  {
    currentState = motion::stopped;
  }

  virtual ~EmptyMotion(){}

  virtual void execute(const MotionRequest& /*motionRequest*/, MotionStatus& /*moitonStatus*/)
  {
    /** do nothing */
  }//end execute
};

#endif //__AbstractMotion_h_
