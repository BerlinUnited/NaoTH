/**
* @file InitialMotion.h
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* the initial motion for booting the robot
*/

#ifndef _INITIALMOTION_H
#define _INITIALMOTION_H

#include "Motion/AbstractMotion.h"

#include <ModuleFramework/Module.h>

BEGIN_DECLARE_MODULE(InitialMotion)
  REQUIRE(RobotInfo)
  REQUIRE(SensorJointData)
  REQUIRE(MotionRequest)
  REQUIRE(InertialSensorData)
  
  PROVIDE(MotionLock)
  PROVIDE(MotorJointData)
END_DECLARE_MODULE(InitialMotion)

class InitialMotion : private InitialMotionBase, public AbstractMotion
{
public:
  InitialMotion();
  
  virtual ~InitialMotion(){}

  virtual void execute(const MotionRequest& motionRequest, MotionStatus& /*motionStatus*/) {};
  void execute();

  virtual bool isFinish() const { return initStatus == InitialPoseReady || initStatus == Finish; }

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

  double getDistance(const naoth::JointData& one, const naoth::JointData& another);

  double init_time;
  double movedTime;
  
  naoth::JointData startJoints;
  naoth::JointData extendJoints;
  naoth::JointData theInitJoints;

  double maxStiffness[naoth::JointData::numOfJoint];
  double safeStiffness[naoth::JointData::numOfJoint];
  double freeStiffness[naoth::JointData::numOfJoint];
};

#endif  /* _INITIALMOTION_H */

