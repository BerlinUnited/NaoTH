/**
* @file IKMotion.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* Implementation of class IKMotion
*/

#include "IKMotion.h"
#include "IKPose.h"

using namespace InverseKinematic;

IKMotion::IKMotion(
  const InverseKinematicsMotionEngineService& theEngineService,
  motion::MotionID id,
  MotionLock& lock)
: AbstractMotion(id, lock),
  theEngineService(theEngineService)
{
}

IKMotion::~IKMotion(){
}

CoMFeetPose IKMotion::getStandPose(double comHeight, double hipOffsetX, double bodyPitchOffset, bool standard) const
{
  if ( standard )
  {
    CoMFeetPose p;
    p.com.translation = Vector3d(hipOffsetX, 0, comHeight);
    p.com.rotation = RotationMatrix::getRotationY(Math::fromDegrees(bodyPitchOffset));
    double footY = NaoInfo::HipOffsetY + getEngine().getParameters().footOffsetY;
    p.feet.left.translation = Vector3d(0, footY, 0);
    p.feet.right.translation = Vector3d(0, -footY, 0);
    return p;
  }
  else
  {
    CoMFeetPose targetPose = getEngine().getCurrentCoMFeetPose();
    targetPose.localInLeftFoot();
    double angZ = targetPose.feet.right.rotation.getZAngle();
    targetPose.feet.right.rotation = RotationMatrix::getRotationZ(angZ);
    targetPose.com.translation.z = comHeight;
    targetPose.com.rotation = RotationMatrix::getRotationZ(angZ*0.5);
    targetPose.com.rotation.rotateY(Math::fromDegrees(bodyPitchOffset));
    return targetPose;
  }
}//end getStandPose
