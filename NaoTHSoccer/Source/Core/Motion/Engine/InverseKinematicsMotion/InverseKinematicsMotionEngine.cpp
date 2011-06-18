/**
 * @file InverseKinematicsMotionEngine.h
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Mellmann, Heinrich</a>
 * Implementation of class Inverse Kinematics Motion Engine
 */


#include "InverseKinematicsMotionEngine.h"
#include "Motion/MorphologyProcessor/ForwardKinematics.h"

using namespace InverseKinematic;

InverseKinematicsMotionEngine::InverseKinematicsMotionEngine()
:theBlackBoard(MotionBlackBoard::getInstance())
{
  
}

Pose3D InverseKinematicsMotionEngine::getLeftFootFromKinematicChain(const KinematicChain& kc) const
{
  Pose3D p = kc.theLinks[KinematicChain::LFoot].M;
  p.translate(0, 0, -NaoInfo::FootHeight);
  return p;
}

Pose3D InverseKinematicsMotionEngine::getRightFootFromKinematicChain(const KinematicChain& kc) const
{
  Pose3D p = kc.theLinks[KinematicChain::RFoot].M;
  p.translate(0, 0, -NaoInfo::FootHeight);
  return p;
}


HipFeetPose InverseKinematicsMotionEngine::getHipFeetPoseFromKinematicChain(const KinematicChain& kc) const
{
  HipFeetPose p;
  p.hip = kc.theLinks[KinematicChain::Hip].M;
  p.feet.left = getLeftFootFromKinematicChain(kc);
  p.feet.right = getRightFootFromKinematicChain(kc);
  return p;
}

CoMFeetPose InverseKinematicsMotionEngine::getCoMFeetPoseFromKinematicChain(const KinematicChain& kc) const
{
  CoMFeetPose p;
  p.com.rotation = kc.theLinks[KinematicChain::Hip].R;
  p.com.translation = kc.CoM;
  p.feet.left = getLeftFootFromKinematicChain(kc);
  p.feet.right = getRightFootFromKinematicChain(kc);
  return p;
}

HipFeetPose InverseKinematicsMotionEngine::getHipFeetPoseBasedOnSensor() const
{
  return getHipFeetPoseFromKinematicChain(theBlackBoard.theKinematicChain);
}

CoMFeetPose InverseKinematicsMotionEngine::getCoMFeetPoseBasedOnSensor() const
{
  return getCoMFeetPoseFromKinematicChain(theBlackBoard.theKinematicChain);
}

HipFeetPose InverseKinematicsMotionEngine::getHipFeetPoseBasedOnModel() const
{
  return getHipFeetPoseFromKinematicChain(theBlackBoard.theKinematicChainModel);
}

CoMFeetPose InverseKinematicsMotionEngine::getCoMFeetPoseBasedOnModel() const
{
  return getCoMFeetPoseFromKinematicChain(theBlackBoard.theKinematicChainModel);
}

HipFeetPose InverseKinematicsMotionEngine::getCurrentHipFeetPose() const
{
  if (theBlackBoard.theSensorJointData.isLegStiffnessOn())
  {
    return getHipFeetPoseBasedOnModel();
  }

  return getHipFeetPoseBasedOnSensor();
}

CoMFeetPose InverseKinematicsMotionEngine::getCurrentCoMFeetPose() const
{
  if (theBlackBoard.theSensorJointData.isLegStiffnessOn())
  {
    return getCoMFeetPoseBasedOnModel();
  }

  return getCoMFeetPoseBasedOnSensor();
}

ZMPFeetPose InverseKinematicsMotionEngine::getPlannedZMPFeetPose() const
{
  if ( thePreviewController.ready() )
  {
    return theZMPFeetPoseBuffer.back();
  }
  
  // TODO: calculate ZMP according to sensor, return CoM as ZMP at the moment
  ZMPFeetPose result;
  CoMFeetPose com = getCurrentCoMFeetPose();
  result.zmp = com.com;
  result.feet = com.feet;
  return result;
}

Pose3D InverseKinematicsMotionEngine::interpolate(const Pose3D& sp, const Pose3D& tp, double t) const
{
  ASSERT(0 <= t);
  ASSERT(t <= 1);

  Vector3<double> perr = tp.translation - sp.translation;
  
  Pose3D p;
  p.translation = sp.translation + perr * t;
  p.rotation = RotationMatrix::interpolate(sp.rotation, tp.rotation, t);

  return p;
}//end interpolate

HipFeetPose InverseKinematicsMotionEngine::controlCenterOfMass(const CoMFeetPose& p)
{
  HipFeetPose result;
  result.feet = p.feet;
  result.hip = p.com;
  result.localInHip();
  
  /* normally it should not affect the algorithm,
   * but when IK can not be solved, it affects the result, i.e. support foot is prefered.
   */
  bool leftFootSupport = (result.feet.left.translation.abs2() < result.feet.right.translation.abs2());
  
  // transform all data in support foot local coordiantes
  Vector3<double> refCoM;
  if ( leftFootSupport )
    refCoM = p.feet.left.invert() * p.com.translation;
  else
    refCoM = p.feet.right.invert() * p.com.translation;
  
  // copy head joint and arm joint from sensor
  const double *sj = theBlackBoard.theSensorJointData.position;
  double *j = theInverseKinematics.theJointData.position;
  for (int i = JointData::HeadPitch; i <= JointData::LElbowYaw; i++)
  {
    j[i] = sj[i];
  }

  result.hip.translation = theCoMControlResult; // reuse results from last calculation
  
  // set the support foot as orginal
  Kinematics::Link* obsFoot;
  if ( leftFootSupport )
    obsFoot = &(theInverseKinematics.theKinematicChain.theLinks[KinematicChain::LFoot]);
  else
    obsFoot = &(theInverseKinematics.theKinematicChain.theLinks[KinematicChain::RFoot]);
    
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

    Vector3<double> e = refCoM - obsCoM;

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
  
  if ( bestError > 1 )
  {
    cerr<<"Warning: can not control CoM @ "<<bestError<<endl;
  }
  
  return result;
}

bool InverseKinematicsMotionEngine::rotationStabilize(Pose3D& hip) const
{
  Vector2d r;
  r.x = hip.rotation.getXAngle();
  r.y = hip.rotation.getYAngle();

  const Vector2d& s = theBlackBoard.theInertialPercept.data;

  Vector2d e = r - s;

  bool isWorking = false;
  Vector2<double> chestRotationStabilizerValue;
  const double maxAngle = Math::fromDegrees(30);
  for( int i=0; i<2; i++ )
  {
    double threshold = Math::fromDegrees(getParameters().rotationStabilize.threshold[i]);
    if ( abs(e[i]) > threshold )
    {
      chestRotationStabilizerValue[i] = (e[i] - Math::sgn(e[i]) * threshold) * getParameters().rotationStabilize.k[i];
      chestRotationStabilizerValue[i] = Math::clamp(chestRotationStabilizerValue[i], -maxAngle, maxAngle);
      isWorking = true;
    }
  }

  if ( isWorking )
  {
    double height = theBlackBoard.theKinematicChain.theLinks[KinematicChain::Hip].p.z;
    hip.translate(0, 0, -height);
    hip.rotateX(chestRotationStabilizerValue.x);
    hip.rotateY(chestRotationStabilizerValue.y);
    hip.translate(0, 0, height);
  }
  return isWorking;
}

void InverseKinematicsMotionEngine::solveHipFeetIK(const InverseKinematic::HipFeetPose& p)
{
  Pose3D chest = p.hip;
  chest.translate(0, 0, NaoInfo::HipOffsetZ);
  const Vector3<double> footOffset(0,0,-NaoInfo::FootHeight);
  
  double err = theInverseKinematics.gotoLegs(chest, p.feet.left, p.feet.right, footOffset, footOffset);

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

void InverseKinematicsMotionEngine::startControlZMP(const ZMPFeetPose& target)
{
  ZMPFeetPose startZMPPose = getPlannedZMPFeetPose();
  startZMPPose.localInLeftFoot();

  // here assume the foot movment can not jump
  // so we can keep them in the same coordinate
  const Pose3D& trans = target.feet.left;
  startZMPPose.feet.left *= trans;
  startZMPPose.feet.right *= trans;
  startZMPPose.zmp *= trans;

  thePreviewControlCoM = Vector2<double>(startZMPPose.zmp.translation.x,startZMPPose.zmp.translation.y);
  thePreviewControldCoM = Vector2<double>(0,0);
  thePreviewControlddCoM = Vector2<double>(0,0);
  thePreviewController.init(thePreviewControlCoM, thePreviewControldCoM, thePreviewControlddCoM);
  
  unsigned int previewSteps = thePreviewController.previewSteps();
  theZMPFeetPoseBuffer.clear();
  thePreviewController.clear();

  ZMPFeetPose myTarget = target;
  myTarget.zmp.translation.x = startZMPPose.zmp.translation.x;
  myTarget.zmp.translation.y = startZMPPose.zmp.translation.y;
  for (unsigned int i = 0; i < previewSteps-1; i++)
  {
    double t = static_cast<double>(i) / previewSteps;
    ZMPFeetPose p = interpolate(startZMPPose, myTarget, t);
    thePreviewController.push(Vector2<double>(p.zmp.translation.x, p.zmp.translation.y));
    theZMPFeetPoseBuffer.push_back(p);
  }
}

CoMFeetPose InverseKinematicsMotionEngine::controlZMP(const ZMPFeetPose& p)
{
  theZMPFeetPoseBuffer.push_back(p);

  Vector2<double> zmp(p.zmp.translation.x, p.zmp.translation.y);
  thePreviewController.setParameters(theBlackBoard.theFrameInfo.basicTimeStep, p.zmp.translation.z);
  
  if ( !thePreviewController.ready() )
  {
    startControlZMP(p);
  }
  
  thePreviewController.control(zmp, thePreviewControlCoM, thePreviewControldCoM, thePreviewControlddCoM);
  
  const ZMPFeetPose& bP = theZMPFeetPoseBuffer.front();
  CoMFeetPose result;
  result.com = bP.zmp;
  result.feet = bP.feet;
  theZMPFeetPoseBuffer.pop_front();
  result.com.translation.x = thePreviewControlCoM.x;
  result.com.translation.y = thePreviewControlCoM.y;
  return result;
}

bool InverseKinematicsMotionEngine::stopControlZMP(const ZMPFeetPose& p, CoMFeetPose& result)
{
  result = controlZMP(p);
  
  // if ZMP == CoM
  Vector2<double> diff = Vector2<double>(result.com.translation.x-p.zmp.translation.x, result.com.translation.y-p.zmp.translation.y);
  bool stopped = diff.abs2() < 1 && thePreviewControldCoM.abs2() < 1 && thePreviewControlddCoM.abs2() < 1;
  
  if ( stopped )
  {
    thePreviewController.clear();
  }
  
  return stopped;
}
