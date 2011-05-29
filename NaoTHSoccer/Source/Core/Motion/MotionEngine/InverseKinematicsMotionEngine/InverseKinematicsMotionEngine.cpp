/**
 * @file InverseKinematicsMotionEngine.h
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Mellmann, Heinrich</a>
 * Implementation of class Inverse Kinematics Motion Engine
 */


#include "InverseKinematicsMotionEngine.h"
#include "Core/Motion/MorphologyProcessor/ForwardKinematics.h"

using namespace InverseKinematic;

InverseKinematicsMotionEngine::InverseKinematicsMotionEngine()
:theBlackBoard(MotionBlackBoard::getInstance())
{
  
}

HipFeetPose InverseKinematicsMotionEngine::controlCenterOfMass(const CoMFeetPose& p)
{
  HipFeetPose result;
  result.lFoot = p.lFoot;
  result.rFoot = p.rFoot;
  result.hip = p.com;
  result.localInHip();
  
  // transform all data in left foot local coordiantes
  Vector3<double> refCoM = p.lFoot.invert() * p.com.translation;
  
  // copy head joint and arm joint from sensor
  const double *sj = theBlackBoard.theSensorJointData.position;
  double *j = theInverseKinematics.theJointData.position;
  for (int i = JointData::HeadPitch; i <= JointData::LElbowYaw; i++)
  {
    j[i] = sj[i];
  }

  result.hip.translation = theCoMControlResult; // reuse results from last calculation
  
  // set the support foot as orginal
  Kinematics::Link* obsFoot = &(theInverseKinematics.theKinematicChain.theLinks[KinematicChain::LFoot]);
  obsFoot->R = RotationMatrix();
  obsFoot->p = Vector3<double>(0, 0, NaoInfo::FootHeight);

  double bestError = std::numeric_limits<double>::max();
  int i = 0;
  double max_iter = 15;
  double max_error = 1e-8;

  // step control parameter
  double step = 1;
  double alpha = 0.5;
  double max_step = 1;
  
  for (; i < max_iter; i++)
  {
    solveHipFeetIK(result);
    Kinematics::ForwardKinematics::updateKinematicChainFrom(obsFoot);
    theInverseKinematics.theKinematicChain.updateCoM();
    Vector3<double> obsCoM = theInverseKinematics.theKinematicChain.CoM;

    Vector3<double> e = (refCoM - obsCoM);

    double error = e.x * e.x + e.y * e.y + e.z * e.z;

    if (bestError < error)
    {
      // the error becoms bigger, reset
      result.hip.translation = theCoMControlResult;
      step *= alpha; // make smaller steps
    } else
    {
      bestError = error;
      theCoMControlResult = result.hip.translation;
      step *= (1.0 + alpha); // make bigger steps
    }

    if (bestError < max_error /*&& i > 0*/)
    {
      // converge
      result.hip.translation = theCoMControlResult;
      break;
    }

    step = Math::clamp(step, max_error, max_step);

    Vector3<double> u;
    u.x = Math::clamp(e.x * step, -50.0, 50.0);
    u.y = Math::clamp(e.y * step, -50.0, 50.0);
    u.z = Math::clamp(e.z * step, -50.0, 50.0);

    result.hip.translation += u;
  }//end for

  return result;
}

void InverseKinematicsMotionEngine::rotationStabilize(Pose3D& hip) const
{
  
}

void InverseKinematicsMotionEngine::solveHipFeetIK(const InverseKinematic::HipFeetPose& p)
{
  Pose3D chest = p.hip;
  chest.translate(0, 0, NaoInfo::HipOffsetZ);
  const Vector3<double> footOffset(0,0,-NaoInfo::FootHeight);
  
  double err = theInverseKinematics.gotoLegs(chest, p.lFoot, p.rFoot, footOffset, footOffset);

  if (abs(err) > Math::fromDegrees(1))
  {
    THROW("IK failed!");
  }
}
  
void InverseKinematicsMotionEngine::copyLegJoints(double (&position)[naoth::JointData::numOfJoint]) const
{
  const double* l = theInverseKinematics.theJointData.position;
  for (int i = JointData::RHipYawPitch; i < JointData::numOfJoint; i++)
  {
    position[i] = l[i];
  }
  // use mean value of two hips
  double hipYawPitch = (position[JointData::LHipYawPitch] + position[JointData::RHipYawPitch]) * 0.5;
  position[JointData::LHipYawPitch] = hipYawPitch;
  position[JointData::RHipYawPitch] = hipYawPitch;
}