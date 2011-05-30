/**
* @file InitialMotion.h
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* the initial motion for booting the robot
*/

#ifndef _INITIALMOTION_H
#define	_INITIALMOTION_H

#include "Motion/AbstractMotion.h"

class InitialMotion : public AbstractMotion
{
public:
  InitialMotion();
  
  virtual ~InitialMotion(){}

  virtual void execute(const MotionRequest& motionRequest, MotionStatus& /*motionStatus*/);
  
private:
  enum StatusID
  {
    Dead,
    Init,
    StiffnessReady,
    ExtendPoseReady,
    InitialPoseReady,
    Finish
  }  initStatus;

  void dead();
  void increaseStiffness();
  void moveToExtendPose();
  void moveToInitialPose();
  void freeJoint(bool freely);

  bool isSafe() const;

  bool isDanger() const;

  double movedTime;
  JointData startJoints;
  JointData extendJoints;
  JointData theInitJoints;
  double maxStiffness[JointData::numOfJoint];
  double safeStiffness[JointData::numOfJoint];
  double freeStiffness[JointData::numOfJoint];
};

#endif	/* _INITIALMOTION_H */

