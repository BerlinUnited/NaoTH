/**
 * @file InverseKinematicsMotionEngine.h
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 * Declaration of class Inverse Kinematics Motion Engine
 */

#ifndef _INVERSE_KINEMATCS_MOTION_ENGINE_
#define _INVERSE_KINEMATCS_MOTION_ENGINE_

#include "Core/Motion/MotionBlackBoard.h"
#include "Motions/IKPose.h"

class InverseKinematicsMotionEngine: public Singleton<InverseKinematicsMotionEngine>
{
private:
  friend class Singleton<InverseKinematicsMotionEngine>;
  
  InverseKinematicsMotionEngine();
public:

  virtual ~InverseKinematicsMotionEngine()
  {
  }
  
  InverseKinematic::HipFeetPose controlCenterOfMass(const InverseKinematic::CoMFeetPose& p);
  
  void solveHipFeetIK(const InverseKinematic::HipFeetPose& p);
  
  void rotationStabilize(Pose3D& hip) const;
  
  void copyLegJoints(double (&position)[naoth::JointData::numOfJoint]) const;
  
private:
  const MotionBlackBoard& theBlackBoard;

  Kinematics::InverseKinematics theInverseKinematics;
  
  Vector3<double> theCoMControlResult;
};

#endif // _INVERSE_KINEMATCS_MOTION_ENGINE_
