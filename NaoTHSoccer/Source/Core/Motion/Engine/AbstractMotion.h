/**
* @file AbstractMotion.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* Declaration of class AbstractMotion
*/

#ifndef _AbstractMotion_h_
#define _AbstractMotion_h_

#include "Representations/Motion/Request/MotionRequest.h"
#include <Representations/Infrastructure/JointData.h>

/**
*
*/
class MotionLock
{
public:
  MotionLock() 
    : 
    id(motion::num_of_motions), 
    state(motion::stopped)
  {}
  motion::MotionID id;
  motion::State state;

  bool isStopped() const {return state == motion::stopped; }
  void setState(motion::MotionID newId, motion::State newState) {
    assert(newId == id || state == motion::stopped);
    id = newId;
    state = newState;
  }
  void lock(motion::MotionID newId) {
    setState(newId, state = motion::running);
  }
  void unlock(motion::MotionID newId) {
    setState(newId, motion::stopped);
  }
  void forceUnlock() {
    state = motion::stopped;
  }
};


/**
*
*/
class AbstractMotion
{
private:
  motion::MotionID theId;
  MotionLock& lock;
  motion::State currentState;

protected:
  void setCurrentState(motion::State state)
  {
    // assure the lock is 'mine'
    assert(lock.id == theId);
    lock.state = state;
    currentState = state;
  }

  void setId(motion::MotionID id){ theId = id; lock.id = theId; };

public:

  AbstractMotion(motion::MotionID id, MotionLock& lock);

  virtual ~AbstractMotion(){}

  virtual void init(){}

  bool isStopped() const {return currentState == motion::stopped; }

  virtual bool isFinish() const { return isStopped(); }

  motion::MotionID getId() const { return theId; }
  
  std::string getName() const { return motion::getName(theId); } 

  motion::State state() const { return currentState; }

protected:
  /** set the stiffness with max changes */
  bool setStiffness(
    naoth::MotorJointData& theMotorJointData,
    const naoth::SensorJointData& theSensorJointData,
    double* hardness, double delta,
    naoth::JointData::JointID begin=naoth::JointData::HeadPitch,
    naoth::JointData::JointID end=naoth::JointData::numOfJoint);
};


#include <ModuleFramework/Module.h>


BEGIN_DECLARE_MODULE(EmptyMotion)
  PROVIDE(MotionLock)
END_DECLARE_MODULE(EmptyMotion)

class EmptyMotion: private EmptyMotionBase, public AbstractMotion
{
public:
  EmptyMotion()
    :
    AbstractMotion(motion::empty, getMotionLock())
  {
    setCurrentState(motion::stopped);
  }

  virtual ~EmptyMotion(){}

  void execute()
  {
    /** do nothing */
  }//end execute
};

#endif //_AbstractMotion_h_
