/**
* @file IKMotion.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* Implementation of class IKMotion
*/

#include "IKMotion.h"

using namespace InverseKinematic;

IKMotion::IKMotion(motion::MotionID id)
: AbstractMotion(id),
theEngine(InverseKinematicsMotionEngine::getInstance()),
theParameters(theEngine.getParameters())
{
}

IKMotion::~IKMotion(){
}

CoMFeetPose IKMotion::getStandPose(double comHeight) const
{
  CoMFeetPose p;
  p.com.translation = Vector3<double>(theParameters.hipOffsetX, 0, comHeight);
  p.com.rotation = RotationMatrix::getRotationY(Math::fromDegrees(theParameters.bodyPitchOffset));
  double footY = NaoInfo::HipOffsetY + theParameters.footOffsetY;
  p.feet.left.translation = Vector3<double>(0, footY, 0);
  p.feet.right.translation = Vector3<double>(0, -footY, 0);
  return p;
}//end getStandPose
