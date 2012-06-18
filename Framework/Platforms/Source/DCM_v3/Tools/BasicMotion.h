/**
* @file InitialMotion.h
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* the initial motion for booting the robot
*/

#ifndef _BasicMotion_H_
#define _BasicMotion_H_

#include "Representations/Infrastructure/JointData.h"
#include "Representations/Infrastructure/InertialSensorData.h"

class BasicMotion
{
public:
  BasicMotion(
    naoth::MotorJointData& theMotorJointData,
    const naoth::JointData& theStartJointData,
    const naoth::InertialSensorData& theInertialSensorData);
  
  ~BasicMotion(){}

  void execute();
  
  bool isFinish() const { return initStatus == Finish; }

private:
  enum StatusID
  {
    Init,
    StiffnessReady,
    InitialPoseReady,
    Finish
  }  initStatus;

  void moveToInitialPose();

  bool setStiffness(double* stiffness, double delta);
  double getDistance(const naoth::JointData& one, const naoth::JointData& another);

  naoth::JointData theInitJoints;
  double maxStiffness[naoth::JointData::numOfJoint];
  double safeStiffness[naoth::JointData::numOfJoint];
  double freeStiffness[naoth::JointData::numOfJoint];

  double init_time;

  // target
  const double basicTimeStepInSecond;
  naoth::MotorJointData& theMotorJointData;
  const naoth::JointData theStartJointData;
};

#endif  /* _BasicMotion_H */

