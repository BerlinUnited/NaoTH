/**
* @file IKMotion.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* Implementation of class IKMotion
*/

#include "IKMotion.h"

using namespace InverseKinematic;

IKMotion::IKMotion(
  const InverseKinematicsMotionEngineService& theEngineService,
  motion::MotionID id,
  MotionLock& lock)
: AbstractMotion(id, lock),
  theEngineService(theEngineService)
  //theEngine(InverseKinematicsMotionEngine::getInstance()),
  //theParameters(theEngine.getParameters())
{
}

IKMotion::~IKMotion(){
}

CoMFeetPose IKMotion::getStandPose(double comHeight, bool standard) const
{
  if ( standard )
  {
    CoMFeetPose p;
    p.com.translation = Vector3<double>(getEngine().getParameters().hipOffsetX, 0, comHeight);
    p.com.rotation = RotationMatrix::getRotationY(Math::fromDegrees(getEngine().getParameters().bodyPitchOffset));
    double footY = NaoInfo::HipOffsetY + getEngine().getParameters().footOffsetY;
    p.feet.left.translation = Vector3<double>(0, footY, 0);
    p.feet.right.translation = Vector3<double>(0, -footY, 0);
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
    targetPose.com.rotation.rotateY(Math::fromDegrees(getEngine().getParameters().bodyPitchOffset));
    return targetPose;
  }
}//end getStandPose
