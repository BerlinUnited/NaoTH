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
protected:
  enum State
  {
    running,
    stopped
  };

  State currentState;
  MotorJointData& theMotorJointData;
  const MotionBlackBoard& theBlackBoard;
  MotionID theId;
  bool finished;

public:

  AbstractMotion(MotionID id);

  virtual ~AbstractMotion(){};

  virtual void init(){}
  virtual void execute(const MotionRequest& motionRequest, MotionStatus& moitonStatus) = 0;

  bool isRunning() const {return currentState == running; }

  MotionID getId() const { return theId; }

  bool isFinished() const { return finished; }

protected:
  /** set the stiffness with max changes */
  bool setStiffness(double* hardness, double delta);
};

class EmptyMotion: public AbstractMotion
{
public:
  EmptyMotion():AbstractMotion(empty) {};

  virtual ~EmptyMotion(){}

  virtual void execute(const MotionRequest& motionRequest, MotionStatus& /*moitonStatus*/)
  {
    if(motionRequest.id != empty)
      currentState = stopped;
    else
      currentState = running;

    /** do nothing */
  }//end execute
};

#endif //__AbstractMotion_h_
