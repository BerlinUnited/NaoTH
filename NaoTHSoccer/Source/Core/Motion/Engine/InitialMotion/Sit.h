/**
* @file Sit.h
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
*/

#ifndef _SIT_H
#define	_SIT_H

#include "Motion/AbstractMotion.h"

class Sit : public AbstractMotion
{
public:
  Sit();
  
  virtual ~Sit(){}

  virtual void execute(const MotionRequest& motionRequest, MotionStatus& /*motionStatus*/);

private:
  enum StatusID
  {
    Move,
    SitPoseReady,
    Finish
  }  sitStatus;

  void moveToSitPose();
  void freeJoint(bool freely);

  double movedTime;
  naoth::JointData startJoints;
  naoth::JointData theSitJoints;
  double maxStiffness[naoth::JointData::numOfJoint];
  double safeStiffness[naoth::JointData::numOfJoint];
  double freeStiffness[naoth::JointData::numOfJoint];
};

#endif	/* SIT_H */

