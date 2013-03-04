/**
 * @file InverseKinematicsMotionEngine.h
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Mellmann, Heinrich</a>
 * Implementation of class Inverse Kinematics Motion Engine
 */


#include "InverseKinematicsMotionEngine.h"
#include "Motion/MorphologyProcessor/ForwardKinematics.h"
#include "Tools/Debug/DebugModify.h"
#include "Tools/Debug/DebugBufferedOutput.h"

using namespace InverseKinematic;
using namespace naoth;

InverseKinematicsMotionEngine::InverseKinematicsMotionEngine()
 :theBlackBoard(MotionBlackBoard::getInstance()),
  rotationStabilizeFactor(0)
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
  // TODO: calculate ZMP according to sensor, return CoM as ZMP at the moment
  ZMPFeetPose result;
  CoMFeetPose com = getCurrentCoMFeetPose();
  result.zmp = com.com;
  result.feet = com.feet;

  /*if ( thePreviewController.ready() )
  {
    result.zmp.translation = thePreviewController.back();
  }*/
  
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

HipFeetPose InverseKinematicsMotionEngine::controlCenterOfMass(const CoMFeetPose& p,bool& sloved, bool fix_height)
{
  // copy initial values
  HipFeetPose result;
  result.feet = p.feet;
  result.hip = p.com;
  result.localInHip();
  

  /* normally it should not affect the algorithm,
   * but when IK can not be solved, it affects the result, i.e., support foot is prefered.
   */
  bool leftFootSupport = (result.feet.left.translation.z < result.feet.right.translation.z);

  // requested com in the coordinates of the support foot
  Vector3<double> refCoM;
  // support foot
  Kinematics::Link* obsFoot;

  // transform all data in support foot local coordiantes
  if ( leftFootSupport )
  {
    obsFoot = &(theInverseKinematics.theKinematicChain.theLinks[KinematicChain::LFoot]);
    refCoM = p.feet.left.invert() * p.com.translation;
  }
  else
  {
    obsFoot = &(theInverseKinematics.theKinematicChain.theLinks[KinematicChain::RFoot]);
    refCoM = p.feet.right.invert() * p.com.translation;
  }

  // 
  obsFoot->R = RotationMatrix();
  obsFoot->p = Vector3<double>(0, 0, NaoInfo::FootHeight);
  
  // reuse results from last calculation for the starting value
  result.hip.translation = theCoMControlResult;


  // copy the requested values for the head and arm joints
  const double *sj = theBlackBoard.theMotorJointData.position;//theBlackBoard.theSensorJointData.position;
  double *j = theInverseKinematics.theJointData.position;
  for (int i = JointData::HeadPitch; i <= JointData::LElbowYaw; i++)
  {
    j[i] = sj[i];
  }



  double bestError = std::numeric_limits<double>::max();
  int i = 0; // iteration
  double max_iter = 15; // max number of iretations
  double max_error = 1e-8; // threshold

  // step control parameter
  double step = 1;
  double alpha = 0.5;
  double max_step = 1;
  
  for (; i < max_iter; i++)
  {
    // calculate the joints fulfilling the result
    solveHipFeetIK(result);

    // calculate the kinematic chain
    Kinematics::ForwardKinematics::updateKinematicChainFrom(obsFoot);
    // ... and the com
    theInverseKinematics.theKinematicChain.updateCoM();

    Vector3<double> obsCoM = theInverseKinematics.theKinematicChain.CoM;

    Vector3<double> e = refCoM - obsCoM;

    double error = e.x * e.x + e.y * e.y + e.z * e.z*(!fix_height);

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

    double maxAdjustment = 50;
    MODIFY("IK_COM_CTR_MAX", maxAdjustment);
    Vector3<double> u;
    if (abs(u.x) > maxAdjustment || abs(u.y) > maxAdjustment)
    {
      sloved = false;
    }
    else
    {
      sloved = true;
    }
    u.x = Math::clamp(e.x * step, -maxAdjustment, maxAdjustment);
    u.y = Math::clamp(e.y * step, -maxAdjustment, maxAdjustment);
    u.z = Math::clamp(e.z * step, -maxAdjustment, maxAdjustment)*(!fix_height);

    result.hip.translation += u;
  }//end for
  
  if ( bestError > 1 )
  {
    std::cerr<<"Warning: can not control CoM @ "<<bestError<<std::endl;
  }

  if( i == max_iter )
  {
    std::cerr<<"Warning: maximum iterations reached @ "<<bestError<<std::endl;
  }
  
  return result;
}//end controlCenterOfMass


void InverseKinematicsMotionEngine::feetStabilize(double (&position)[naoth::JointData::numOfJoint])
{
  const Vector2d& inertial = theBlackBoard.theInertialModel.orientation;
  const Vector2d& gyro = theBlackBoard.theGyrometerData.data;

  

  // HACK: small filter...
  static Vector2<double> lastGyro = gyro;
  Vector2<double> filteredGyro = (lastGyro+gyro)*0.5;

  Vector2<double> weight;
  weight.x = 
      getParameters().walk.stabilizeFeetP.x * inertial.x
    + getParameters().walk.stabilizeFeetD.x * filteredGyro.x;

  weight.y = 
      getParameters().walk.stabilizeFeetP.y * inertial.y
    + getParameters().walk.stabilizeFeetD.y * filteredGyro.y;


  // X axis
  //position[JointData::RHipRoll] -= weightX;
  position[JointData::RAnkleRoll] += weight.x;
  //position[JointData::LHipRoll] -= weightX;
  position[JointData::LAnkleRoll] += weight.x;

  // Y axis
  position[JointData::RAnklePitch] += weight.y;
  position[JointData::LAnklePitch] += weight.y;

  lastGyro = gyro;
}//end feetStabilize


bool InverseKinematicsMotionEngine::rotationStabilize(Pose3D& hip, const Pose3D& leftFoot, const Pose3D& rightFoot)
{
  // disable stablization slowly when no foot is on the ground
  const double switchingTime = 3000; // ms
  const double switchingRate = theBlackBoard.theRobotInfo.basicTimeStep / switchingTime;
  //if (theBlackBoard.theSupportPolygon.mode == SupportPolygon::NONE)
  if(!theBlackBoard.theGroundContactModel.leftGroundContact &&
     !theBlackBoard.theGroundContactModel.rightGroundContact)
    rotationStabilizeFactor -= switchingRate;
  else
    rotationStabilizeFactor += switchingRate;

  rotationStabilizeFactor = Math::clamp(rotationStabilizeFactor, 0.0, 1.0);
  PLOT("rotationStabilizeFactor", rotationStabilizeFactor);

  Vector2d r;
  r.x = hip.rotation.getXAngle();
  r.y = hip.rotation.getYAngle();

  PLOT("rotationStabilize:hip:x", Math::toDegrees(hip.rotation.getXAngle()));
  PLOT("rotationStabilize:hip:y", Math::toDegrees(hip.rotation.getYAngle()));

  //const Vector2d& s = theBlackBoard.theInertialPercept.data;
  const Vector2d& s = theBlackBoard.theInertialSensorData.data;

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
      //chestRotationStabilizerValue[i] *= rotationStabilizeFactor;
      isWorking = true;
    }
  }

  PLOT("isWorking", isWorking);
  if ( isWorking )
  {
    double height = NaoInfo::ThighLength + NaoInfo::TibiaLength + NaoInfo::FootHeight;
        //theBlackBoard.theKinematicChain.theLinks[KinematicChain::Hip].p.z;
    hip.translate(0, 0, -height);
    hip.rotateX(chestRotationStabilizerValue.x);
    hip.rotateY(chestRotationStabilizerValue.y);
    hip.translate(0, 0, height);
  }

  PLOT("rotationStabilize:stabilizer:x", Math::toDegrees(hip.rotation.getXAngle()));
  PLOT("rotationStabilize:stabilizer:y", Math::toDegrees(hip.rotation.getYAngle()));

  return isWorking;
}//end rotationStabilize



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
  // TODO: this is not enough, currently a solution of BH is used from InverseKinematicsBH.h
  // use mean value of two hips
  double hipYawPitch = (position[JointData::LHipYawPitch] + position[JointData::RHipYawPitch]) * 0.5;
  position[JointData::LHipYawPitch] = hipYawPitch;
  position[JointData::RHipYawPitch] = hipYawPitch;
}


int InverseKinematicsMotionEngine::controlZMPstart(const ZMPFeetPose& start)
{
  // if it is not ready, it should be empty
  //ASSERT( thePreviewController.count() == 0 );
  // TODO: clear it because of the motion can be forced to finish immediately...
  // the idea of keep buffer is to switch zmp control between different motions,
  // such as walk and kick, then maybe we should check if zmp control is used every cycle and etc.
  thePreviewController.clear();

  CoMFeetPose currentCoMPose = getCurrentCoMFeetPose();
  currentCoMPose.localInLeftFoot();

  // here assume the foot movment can not jump
  // so we can keep them in the same coordinate
  const Pose3D& trans = start.feet.left;
  //currentCoMPose.feet.left *= trans;
  //currentCoMPose.feet.right *= trans;
  currentCoMPose.com *= trans;

  thePreviewControlCoM = currentCoMPose.com.translation;
  thePreviewControldCoM = Vector2<double>(0,0);
  thePreviewControlddCoM = Vector2<double>(0,0);
  thePreviewController.init(currentCoMPose.com.translation, thePreviewControldCoM, thePreviewControlddCoM);
  
  unsigned int previewSteps = thePreviewController.previewSteps() - 1;
  thePreviewController.clear();

  for (unsigned int i = 0; i < previewSteps; i++)
  {
    double t = static_cast<double>(i) / previewSteps;
    Pose3D p = interpolate(currentCoMPose.com, start.zmp, t);
    thePreviewController.push(p.translation);
  }
  return previewSteps;
}

void InverseKinematicsMotionEngine::controlZMPpush(const Vector3d& zmp)
{
  thePreviewController.push(zmp);
}

bool InverseKinematicsMotionEngine::controlZMPpop(Vector3d& com)
{
  if ( thePreviewController.ready() )
  {
    thePreviewController.control(thePreviewControlCoM, thePreviewControldCoM, thePreviewControlddCoM);
    com = thePreviewControlCoM;
    return true;
  }
  return false;
}

bool InverseKinematicsMotionEngine::controlZMPstop(const Vector3d& finalZmp)
{
  Vector3d diff = finalZmp - thePreviewControlCoM;
  bool stoppted = (diff.abs2() < 1) && (thePreviewControldCoM.abs2() < 1) && (thePreviewControlddCoM.abs2() < 1);
  if ( stoppted )
  {
    thePreviewController.clear();
  }
  else
  {
    controlZMPpush(finalZmp);
  }

  return stoppted;
}

Vector3d InverseKinematicsMotionEngine::controlZMPback() const
{
  return thePreviewController.back();
}

Vector3d InverseKinematicsMotionEngine::controlZMPfront() const
{
  return thePreviewController.front();
}

void InverseKinematicsMotionEngine::controlZMPclear()
{
  thePreviewController.clear();
}

void InverseKinematicsMotionEngine::autoArms(const HipFeetPose& pose, double (&position)[JointData::numOfJoint])
{
  double target[JointData::LElbowYaw + 1];
  target[JointData::RElbowYaw] = Math::fromDegrees(90);
  target[JointData::LElbowYaw] = Math::fromDegrees(-90);
  target[JointData::RShoulderRoll] = Math::fromDegrees(-10);
  target[JointData::LShoulderRoll] = Math::fromDegrees(10);
  target[JointData::RShoulderPitch] = Math::fromDegrees(100);
  target[JointData::LShoulderPitch] = Math::fromDegrees(100);
  target[JointData::RElbowRoll] = Math::fromDegrees(30);
  target[JointData::LElbowRoll] = Math::fromDegrees(-30);

  // move the arm according to motion ----------------
  /*double shouldPitchRate = 0.5;
  double shouldRollRate = 0.5;
  double elbowRollRate = 0.5;
  Pose3D lFoot = pose.hip.local(pose.feet.left);
  Pose3D rFoot = pose.hip.local(pose.feet.right);
  target[JointData::RShoulderPitch] -= (Math::fromDegrees(lFoot.translation.x) * shouldPitchRate);
  target[JointData::RShoulderRoll] -= (Math::fromDegrees(lFoot.translation.y - NaoInfo::HipOffsetY) * shouldRollRate);
  target[JointData::RElbowRoll] += (Math::fromDegrees(lFoot.translation.x) * elbowRollRate);
  target[JointData::LShoulderPitch] -= (Math::fromDegrees(rFoot.translation.x) * shouldPitchRate);
  target[JointData::LShoulderRoll] -= (Math::fromDegrees(rFoot.translation.y + NaoInfo::HipOffsetY) * shouldRollRate);
  target[JointData::LElbowRoll] -= (Math::fromDegrees(rFoot.translation.x) * elbowRollRate);*/
  //----------------------------------------------

  // move the arm accoring to interial sensor -------------
  /*
  if (getParameters().arm.alwaysEnabled
    || (theBlackBoard.theMotionStatus.currentMotion == MotionRequestID::walk && getParameters().walk.useArm))
  {
    const InertialPercept& isd = theBlackBoard.theInertialPercept;
    double shoulderPitch = isd.get(InertialSensorData::Y) * getParameters().arm.shoulderPitchInterialSensorRate;
    double shoulderRoll = isd.get(InertialSensorData::X) * getParameters().arm.shoulderRollInterialSensorRate;
    target[JointData::RShoulderPitch] += shoulderPitch;
    target[JointData::LShoulderPitch] += shoulderPitch;
    target[JointData::RShoulderRoll] += shoulderRoll;
    target[JointData::LShoulderRoll] += shoulderRoll;
  }*/
  //----------------------------------------------

  // make sure the arms do not collide legs --------------
  target[JointData::RShoulderRoll] = std::min(target[JointData::RShoulderRoll], position[JointData::RHipRoll]);
  target[JointData::LShoulderRoll] = std::max(target[JointData::LShoulderRoll], position[JointData::LHipRoll]);
  //---------------------------------------------

  // limit the joint range to avoid collision --------------

  //---------------------------------------------

  // limit the max speed -----------------------------
  double max_speed = Math::fromDegrees(getParameters().arm.maxSpeed) * theBlackBoard.theRobotInfo.getBasicTimeStepInSecond();
  for (int i = JointData::RShoulderRoll; i <= JointData::LElbowYaw; i++)
  {
    double s = target[i] - position[i];
    s = Math::clamp(s, -max_speed, max_speed);
    position[i] += s;
  }
  //----------------------------------------------
}//end autoArms

Vector3<double> InverseKinematicsMotionEngine::sensorCoMIn(KinematicChain::LinkID link) const
{
  Pose3D foot = theBlackBoard.theKinematicChain.theLinks[link].M;
  foot.translate(0, 0, -NaoInfo::FootHeight);
  foot.rotation = RotationMatrix(); // assume the foot is flat on the ground
  return foot.invert() * theBlackBoard.theKinematicChain.CoM;
}

// compares expected com and com from sensors
// @return adjust applyed to hip
Vector3<double> InverseKinematicsMotionEngine::balanceCoM(const Vector3d& lastReqCoM, KinematicChain::LinkID link) const
{
  static unsigned int frameNumber = theBlackBoard.theFrameInfo.getFrameNumber();
  static Vector3<double> uP, uI, uD;
  if ( theBlackBoard.theFrameInfo.getFrameNumber() > frameNumber + 1)
  {
    // reset
    uP = Vector3<double>();
    uI = Vector3<double>();
    uD = Vector3<double>();
  }

  ASSERT(link==KinematicChain::LFoot || link==KinematicChain::RFoot );
  Vector3d sensorCoM = sensorCoMIn(link);

  Vector3d e = lastReqCoM - sensorCoM;

  Vector3<double> u;
  for( int i=0; i<3; i++)
  {
    if (abs(e[i]) > theParameters.balanceCoM.threshold)
    {
      u[i] = e[i] - Math::sgn(e[i]) * theParameters.balanceCoM.threshold;
    }
  }
  uI += u;
  uD = u - uP;
  uP = u;
  frameNumber = theBlackBoard.theFrameInfo.getFrameNumber();
  u = uP * theParameters.balanceCoM.kP + uI * theParameters.balanceCoM.kI + uD * theParameters.balanceCoM.kD;
  for(int i=0; i<3; i++)
  {
    u[i] = Math::clamp(u[i], -30.0, 30.0);
  }
  return u;
}


void InverseKinematicsMotionEngine::gotoArms(const InverseKinematic::HipFeetPose& currentPose, double (&position)[JointData::numOfJoint])
{
  double target[JointData::LElbowYaw + 1];
  //    target[JointData::RShoulderRoll] = 0;
  //    target[JointData::LShoulderRoll] = 0;
  //    target[JointData::RShoulderPitch] = Math::fromDegrees(120);
  //    target[JointData::LShoulderPitch] = Math::fromDegrees(120);
  //    target[JointData::RElbowRoll] = Math::fromDegrees(90);
  //    target[JointData::LElbowRoll] = Math::fromDegrees(-90);
  target[JointData::RElbowYaw] = Math::fromDegrees(90);
  target[JointData::LElbowYaw] = Math::fromDegrees(-90);

  // experiment //////////////////////////

  // move the arm according to motion ----------------
  target[JointData::RShoulderRoll] = Math::fromDegrees(-10);
  target[JointData::LShoulderRoll] = Math::fromDegrees(10);
  target[JointData::RShoulderPitch] = Math::fromDegrees(100);
  target[JointData::LShoulderPitch] = Math::fromDegrees(100);
  target[JointData::RElbowRoll] = Math::fromDegrees(30);
  target[JointData::LElbowRoll] = Math::fromDegrees(-30);

  if ( (getParameters().arm.kickEnabled && theBlackBoard.theMotionStatus.currentMotion == motion::kick)
    || (getParameters().arm.walkEnabled && theBlackBoard.theMotionStatus.currentMotion == motion::walk))
  {
  double shouldPitchRate = 0.5;
  double shouldRollRate = 0.5;
  double elbowRollRate = 0.5;
  MODIFY("Motion:ShoulderPitchRate", shouldPitchRate);
  MODIFY("Motion:ShoulderRollRate", shouldRollRate);
  MODIFY("Motion:ElbowRollRate", elbowRollRate);
  InverseKinematic::HipFeetPose localizedPose(currentPose);
  localizedPose.localInHip();

  Pose3D& lFoot = localizedPose.feet.left;
  Pose3D& rFoot = localizedPose.feet.right;
  target[JointData::RShoulderPitch] -= (Math::fromDegrees(lFoot.translation.x) * shouldPitchRate);
  target[JointData::RShoulderRoll] -= (Math::fromDegrees(lFoot.translation.y - NaoInfo::HipOffsetY) * shouldRollRate);
  target[JointData::RElbowRoll] += (Math::fromDegrees(lFoot.translation.x) * elbowRollRate);
  target[JointData::LShoulderPitch] -= (Math::fromDegrees(rFoot.translation.x) * shouldPitchRate);
  target[JointData::LShoulderRoll] -= (Math::fromDegrees(rFoot.translation.y + NaoInfo::HipOffsetY) * shouldRollRate);
  target[JointData::LElbowRoll] -= (Math::fromDegrees(rFoot.translation.x) * elbowRollRate);
  }
  //----------------------------------------------

  // move the arm accoring to interial sensor -------------
  if (getParameters().arm.alwaysEnabled
    || (theBlackBoard.theMotionStatus.currentMotion == motion::walk && getParameters().walk.useArm))
  {
    // TODO: InertialSensorData may be better
    const InertialModel& isd = theBlackBoard.theInertialModel;
    double shoulderPitch = isd.orientation.y * getParameters().arm.shoulderPitchInterialSensorRate;
    double shoulderRoll = isd.orientation.x * getParameters().arm.shoulderRollInterialSensorRate;
    target[JointData::RShoulderPitch] += shoulderPitch;
    target[JointData::LShoulderPitch] += shoulderPitch;
    target[JointData::RShoulderRoll] += shoulderRoll;
    target[JointData::LShoulderRoll] += shoulderRoll;
  }
  //----------------------------------------------

  // make sure the arms do not collide legs --------------
  target[JointData::RShoulderRoll] = std::min(target[JointData::RShoulderRoll], position[JointData::RHipRoll]);
  target[JointData::LShoulderRoll] = std::max(target[JointData::LShoulderRoll], position[JointData::LHipRoll]);
  //---------------------------------------------

  // limit the joint range to avoid collision --------------

  //---------------------------------------------

  // limit the max speed -----------------------------
  double max_speed = Math::fromDegrees(getParameters().arm.maxSpeed) * theBlackBoard.theRobotInfo.getBasicTimeStepInSecond();
  for (int i = JointData::RShoulderRoll; i <= JointData::LElbowYaw; i++)
  {
    double s = target[i] - position[i];
    s = Math::clamp(s, -max_speed, max_speed);
    position[i] += s;
  }
  //----------------------------------------------
}//end gotoArms

