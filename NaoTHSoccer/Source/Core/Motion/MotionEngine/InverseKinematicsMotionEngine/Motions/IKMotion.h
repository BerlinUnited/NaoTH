/**
* @file IKMotion.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* Declaration of class IKMotion
*/

#ifndef __IKMotion_h_
#define __IKMotion_h_

#include "Core/Motion/AbstractMotion.h"
#include "IKPose.h"
#include "IKParameters.h"
#include "Core/Motion/MotionEngine/InverseKinematicsMotionEngine/InverseKinematicsMotionEngine.h"

class IKMotion: public AbstractMotion 
{
private:
  InverseKinematic::HipFeetPose getHipFeetPoseFromKinematicChain(const KinematicChain& kc) const;
  
  InverseKinematic::CoMFeetPose getCoMFeetPoseFromKinematicChain(const KinematicChain& kc) const;
  
  Pose3D interpolate(const Pose3D& sp, const Pose3D& tp, double t) const;
protected:
  //InverseKinematicsMotionEngine::Trajectory<InverseKinematicsMotionEngine>& trajectory;

  InverseKinematic::CoMFeetPose interpolate(const InverseKinematic::CoMFeetPose& sp, const InverseKinematic::CoMFeetPose& tp, double t) const;

  InverseKinematic::CoMFeetPose getStandPose(double comHeight) const;

  InverseKinematic::HipFeetPose getHipFeetPoseBasedOnSensor() const;
  
  InverseKinematic::CoMFeetPose getCoMFeetPoseBasedOnSensor() const;

  InverseKinematic::HipFeetPose getHipFeetPoseBasedOnModel() const;

  InverseKinematic::CoMFeetPose getCoMFeetPoseBasedOnModel() const;

  InverseKinematic::HipFeetPose getCurrentHipFeetPose() const;

  InverseKinematic::CoMFeetPose getCurrentCoMFeetPose() const;

protected:
  IKParameters theParameters;

  InverseKinematicsMotionEngine& theEngine;
public:

  IKMotion(motion::MotionID id);
  
  virtual ~IKMotion();

  virtual void execute(const MotionRequest& motionRequest, MotionStatus& /*motionStatus*/) = 0;

};

#endif //__IKMotion_h_
