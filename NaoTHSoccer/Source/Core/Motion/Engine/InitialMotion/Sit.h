/**
* @file Sit.h
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
*/

#ifndef _SIT_H
#define _SIT_H

#include "Motion/AbstractMotion.h"

#include <ModuleFramework/Module.h>

BEGIN_DECLARE_MODULE(Sit)
  REQUIRE(RobotInfo)
  REQUIRE(SensorJointData)
  REQUIRE(MotionRequest)
  
  PROVIDE(MotionLock)
  PROVIDE(MotorJointData)
END_DECLARE_MODULE(Sit)

class Sit : private SitBase, public AbstractMotion
{
public:
  Sit();
  
  virtual ~Sit(){}

  virtual void execute(const MotionRequest& motionRequest, MotionStatus& /*motionStatus*/){};
  void execute();

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

#endif  /* SIT_H */

