/**
 * @file InverseKinematicsMotionEngine.h
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 * Declaration of class Inverse Kinematics Motion Engine
 */

#ifndef _INVERSE_KINEMATCS_MOTION_ENGINE_
#define _INVERSE_KINEMATCS_MOTION_ENGINE_

#include "Motion/MotionBlackBoard.h"
#include "Motions/IKPose.h"
#include "PreviewController.h"
#include "Motions/IKParameters.h"

class InverseKinematicsMotionEngine: public Singleton<InverseKinematicsMotionEngine>
{
private:
  friend class Singleton<InverseKinematicsMotionEngine>;
  
  InverseKinematicsMotionEngine();
  
  InverseKinematic::HipFeetPose getHipFeetPoseFromKinematicChain(const KinematicChain& kc) const;
  
  InverseKinematic::CoMFeetPose getCoMFeetPoseFromKinematicChain(const KinematicChain& kc) const;
  
  Pose3D getLeftFootFromKinematicChain(const KinematicChain& kc) const;
  
  Pose3D getRightFootFromKinematicChain(const KinematicChain& kc) const;
  
  Pose3D interpolate(const Pose3D& sp, const Pose3D& tp, double t) const;
public:

  virtual ~InverseKinematicsMotionEngine()
  {
  }
  
  InverseKinematic::HipFeetPose getHipFeetPoseBasedOnSensor() const;
  
  InverseKinematic::CoMFeetPose getCoMFeetPoseBasedOnSensor() const;

  InverseKinematic::HipFeetPose getHipFeetPoseBasedOnModel() const;

  InverseKinematic::CoMFeetPose getCoMFeetPoseBasedOnModel() const;

  InverseKinematic::HipFeetPose getCurrentHipFeetPose() const;

  InverseKinematic::CoMFeetPose getCurrentCoMFeetPose() const;
  
  InverseKinematic::ZMPFeetPose getPlannedZMPFeetPose() const;
  
  template<typename T>
  T interpolate(const T& sp, const T& tp, double t) const 
  {
    T p;
    p.body() = interpolate(sp.body(), tp.body(), t);
    p.feet.left = interpolate(sp.feet.left, tp.feet.left, t);
    p.feet.right = interpolate(sp.feet.right, tp.feet.right, t);
    return p;
  }

  InverseKinematic::HipFeetPose controlCenterOfMass(const InverseKinematic::CoMFeetPose& p);

  int controlZMPstart(const InverseKinematic::ZMPFeetPose& start);

  void controlZMPpush(const Vector3d& zmp);

  Vector3d controlZMPback() const;

  bool controlZMPstop();

  /**
   * @param com return the result
   */
  bool controlZMPpop(Vector3d& com);
  
  void solveHipFeetIK(const InverseKinematic::HipFeetPose& p);
  
  /**
   * @return if stabilizer is working
   */
  bool rotationStabilize(Pose3D& hip) const;
  
  void copyLegJoints(double (&position)[naoth::JointData::numOfJoint]) const;
  
  const IKParameters& getParameters() const { return theParameters; }
  
private:

  const MotionBlackBoard& theBlackBoard;
  
  IKParameters theParameters;

  Kinematics::InverseKinematics theInverseKinematics;
  
  Vector3<double> theCoMControlResult; // save CoM control result to be reused

  PreviewController thePreviewController;
  Vector3<double> thePreviewControlCoM;
  Vector2<double> thePreviewControldCoM;
  Vector2<double> thePreviewControlddCoM;
};

#endif // _INVERSE_KINEMATCS_MOTION_ENGINE_
