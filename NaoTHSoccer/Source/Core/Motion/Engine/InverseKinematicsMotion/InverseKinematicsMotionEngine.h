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

#include <ModuleFramework/Module.h>


BEGIN_DECLARE_MODULE(InverseKinematicsMotionEngine)
  REQUIRE(KinematicChainSensor)
  REQUIRE(KinematicChainMotor)
  REQUIRE(SensorJointData)
END_DECLARE_MODULE(InverseKinematicsMotionEngine)

class InverseKinematicsMotionEngine: private InverseKinematicsMotionEngineBase//: public naoth::Singleton<InverseKinematicsMotionEngine>
{
private:
  //friend class naoth::Singleton<InverseKinematicsMotionEngine>;
  

  InverseKinematic::HipFeetPose getHipFeetPoseFromKinematicChain(const KinematicChain& kc) const;
  
  InverseKinematic::CoMFeetPose getCoMFeetPoseFromKinematicChain(const KinematicChain& kc) const;
  
  Pose3D getLeftFootFromKinematicChain(const KinematicChain& kc) const;
  
  Pose3D getRightFootFromKinematicChain(const KinematicChain& kc) const;
  
  Pose3D interpolate(const Pose3D& sp, const Pose3D& tp, double t) const;
public:

  InverseKinematicsMotionEngine();

  virtual ~InverseKinematicsMotionEngine()
  {
  }

  void execute(){} // dummy

  
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



  InverseKinematic::HipFeetPose controlCenterOfMass(
    const naoth::MotorJointData& theMotorJointData,
    const InverseKinematic::CoMFeetPose& p, 
    bool& solved, 
    bool fix_height/*=false*/);


  unsigned int contorlZMPlength() const { return thePreviewController.previewSteps(); }

  int controlZMPstart(const InverseKinematic::ZMPFeetPose& start);

  void controlZMPpush(const Vector3d& zmp);

  Vector3d controlZMPback() const;

  Vector3d controlZMPfront() const;

  bool controlZMPstop(const Vector3d& finalZmp);

  void controlZMPclear();

  /**
   * @param com return the result
   */
  bool controlZMPpop(Vector3d& com);
  
  void solveHipFeetIK(const InverseKinematic::HipFeetPose& p);
  

  /**
   * PID stabilizer controlling the feet of the robot directly
   */
  void feetStabilize(
    const InertialModel& theInertialModel,
    const naoth::GyrometerData& theGyrometerData,
    double (&position)[naoth::JointData::numOfJoint]);

  /**
   * @return if stabilizer is working
   */
  bool rotationStabilize(
    const naoth::RobotInfo& theRobotInfo,
    const GroundContactModel& theGroundContactModel,
    const naoth::InertialSensorData& theInertialSensorData,
    Pose3D& hip, 
    const Pose3D& leftFoot, 
    const Pose3D& rightFoot);


  void copyLegJoints(double (&position)[naoth::JointData::numOfJoint]) const;
  
  const IKParameters& getParameters() const { return theParameters; }
  
  void autoArms(
    const naoth::RobotInfo& theRobotInfo,
    const InverseKinematic::HipFeetPose& pose, 
    double (&position)[naoth::JointData::numOfJoint]);

  Vector3<double> sensorCoMIn(
    const KinematicChainSensor& theKinematicChain,
    KinematicChain::LinkID link) const;

  Vector3<double> balanceCoM(
    const naoth::FrameInfo& theFrameInfo,
    const KinematicChainSensor& theKinematicChain,
    const Vector3d& lastReqCoM, KinematicChain::LinkID link) const;

  void gotoArms(
    const MotionStatus& theMotionStatus,
    const InertialModel& theInertialModel,
    const naoth::RobotInfo& theRobotInfo,
    const InverseKinematic::HipFeetPose& currentPose, 
    double (&position)[naoth::JointData::numOfJoint]);

private:

  //const MotionBlackBoard& theBlackBoard;
  
  IKParameters theParameters;

  Kinematics::InverseKinematics theInverseKinematics;
  
  Vector3<double> theCoMControlResult; // save CoM control result to be reused

  PreviewController thePreviewController;
  Vector3<double> thePreviewControlCoM;
  Vector2<double> thePreviewControldCoM;
  Vector2<double> thePreviewControlddCoM;

  double rotationStabilizeFactor; // [0, 1] disable ~ enable
};


class InverseKinematicsMotionEngineService
{
public:
  InverseKinematicsMotionEngineService() 
    : 
  theEngine(NULL) 
  {
  }

  virtual ~InverseKinematicsMotionEngineService()
  {
    delete theEngine;
  }

  InverseKinematicsMotionEngine* theEngine;
};//end InverseKinematicsMotionEngineService

#endif // _INVERSE_KINEMATCS_MOTION_ENGINE_
