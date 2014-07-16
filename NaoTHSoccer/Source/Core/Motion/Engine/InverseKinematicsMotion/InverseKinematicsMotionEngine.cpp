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
 :
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
  return getHipFeetPoseFromKinematicChain(getKinematicChainSensor());
}

CoMFeetPose InverseKinematicsMotionEngine::getCoMFeetPoseBasedOnSensor() const
{
  return getCoMFeetPoseFromKinematicChain(getKinematicChainSensor());
}

HipFeetPose InverseKinematicsMotionEngine::getHipFeetPoseBasedOnModel() const
{
  return getHipFeetPoseFromKinematicChain(getKinematicChainMotor());
}

CoMFeetPose InverseKinematicsMotionEngine::getCoMFeetPoseBasedOnModel() const
{
  return getCoMFeetPoseFromKinematicChain(getKinematicChainMotor());
}

HipFeetPose InverseKinematicsMotionEngine::getCurrentHipFeetPose() const
{
  if (getSensorJointData().isLegStiffnessOn())
  {
    return getHipFeetPoseBasedOnModel();
  }

  return getHipFeetPoseBasedOnSensor();
}

CoMFeetPose InverseKinematicsMotionEngine::getCurrentCoMFeetPose() const
{
  // if was controlled in the last frame
  if(getFrameInfo().getFrameNumber() == lastCoMFeetControlFrameInfo.getFrameNumber() + 1)
  {
    return getlastControlledCoMFeetPose();
  }
  else if (getSensorJointData().isLegStiffnessOn())
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

HipFeetPose InverseKinematicsMotionEngine::controlCenterOfMass(
  const MotorJointData& theMotorJointData,
  const CoMFeetPose& p,
  bool& sloved, 
  bool fix_height)
{
  // save in case another motion wants to know it
  lastCoMFeetControlPose = p;
  lastCoMFeetControlFrameInfo = getFrameInfo();

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
  Vector3d refCoM;
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

  // set the supporting foot as the origin
  obsFoot->R = RotationMatrix();
  obsFoot->p = Vector3d(0, 0, NaoInfo::FootHeight);
  
  // reuse results from last calculation for the starting value
  result.hip.translation = theCoMControlResult;
  //result.hip.translation.z -= 35;

  // copy the requested values for the head and arm joints
  const double *sj = theMotorJointData.position;
  double *j = theInverseKinematics.theJointData.position;
  for (int i = 0; i <= JointData::numOfJoint; i++)
  {
    j[i] = sj[i];
  }

  double bestError = std::numeric_limits<double>::max();
  int i = 0; // iteration
  double max_iter = 15; // max number of iretations
  double max_error = 1e-3; //1e-8; // threshold

  // step control parameter
  double step = 1;
  double alpha = 0.5;
  double max_step = 2;
  
  for (; i < max_iter; i++)
  {
    // calculate the joints fulfilling the result
    solveHipFeetIK(result);

    // calculate the kinematic chain and the com
    Kinematics::ForwardKinematics::updateKinematicChainFrom(obsFoot);
    theInverseKinematics.theKinematicChain.updateCoM();

    // calculate error
    const Vector3d& obsCoM = theInverseKinematics.theKinematicChain.CoM;
    Vector3d e = refCoM - obsCoM;
    double error = e.x * e.x + e.y * e.y + e.z * e.z*(!fix_height);

    // adjust step size
    if (bestError < error)
    {
      // the error becoms bigger, reset
      result.hip.translation = theCoMControlResult;
      step *= alpha; // make smaller steps
    } else {
      bestError = error;
      theCoMControlResult = result.hip.translation;
      step *= (1.0 + alpha); // make bigger steps
    }
    step = Math::clamp(step, max_error, max_step);

    // convergence
    if (bestError < max_error /*&& i > 0*/)
    {
      assert(result.hip.translation.x == theCoMControlResult.x && 
             result.hip.translation.y == theCoMControlResult.y &&
             result.hip.translation.z == theCoMControlResult.z);
	    sloved = true;
      break;
    }

    // calculate the update
    Vector3d u = e * step;

    // clampt the update
    double maxAdjustment = 50;
    MODIFY("IK_COM_CTR_MAX", maxAdjustment);
    if (fabs(u.x) > maxAdjustment || fabs(u.y) > maxAdjustment) {
      sloved = false;
    } else {
      sloved = true;
    }
    u.x = Math::clamp(e.x * step, -maxAdjustment, maxAdjustment);
    u.y = Math::clamp(e.y * step, -maxAdjustment, maxAdjustment);
    u.z = Math::clamp(e.z * step, -maxAdjustment, maxAdjustment)*(!fix_height);

    result.hip.translation += u;
  }//end for
  
  // use the last best result
  result.hip.translation = theCoMControlResult;

  //std::cout << "steps " << i << std::endl;

  if(!sloved) {
    std::cerr<<"Warning: control com was not solved @ "<<bestError<<std::endl;
  }

  if ( bestError > 1 ) {
    std::cerr<<"Warning: can not control CoM @ "<<bestError<<std::endl;
  }

  if( i >= max_iter ) {
    std::cerr<<"Warning: maximum iterations reached @ "<<bestError<<std::endl;
  }
  
  return result;
}//end controlCenterOfMass


// TODO: check the parameter sloved
void InverseKinematicsMotionEngine::controlCenterOfMassCool(
  const MotorJointData& theMotorJointData,
  const CoMFeetPose& target,
  HipFeetPose& result,
  bool leftFootSupport,
  bool& sloved,
  bool fix_height)
{
  // save in case another motion wants to know it
  lastCoMFeetControlPose = target;
  lastCoMFeetControlFrameInfo = getFrameInfo();

  // requested com in the coordinates of the support foot
  Vector3d refCoM;
  KinematicChain::LinkID baseLink;
  if ( leftFootSupport ) {
    baseLink = KinematicChain::LFoot;
    refCoM = target.feet.left.invert() * target.com.translation;
  } else {
    baseLink = KinematicChain::RFoot;
    refCoM = target.feet.right.invert() * target.com.translation;
  }

  // set the supporting foot as the origin
  theInverseKinematics.theKinematicChain.getLink(baseLink).M = Pose3D(0, 0, NaoInfo::FootHeight);

  // copy the current motor joints as a stating configuration
  for (int i = 0; i < JointData::numOfJoint; i++) {
    theInverseKinematics.theJointData.position[i] = theMotorJointData.position[i];
  }

  // initialize the result with the requested com-feet pose
  /*
  HipFeetPose result;
  result.feet = target.feet;
  result.hip = target.com;
  result.localInHip();
  */
  
  //lastCoMControlResult = result.hip.translation;
  // reuse results from last calculation for the starting value
  //result.hip.translation = theCoMControlResult;// + (refCoM - lastCoMControlTarget);
  //lastCoMControlTarget = refCoM;

  double bestError = std::numeric_limits<double>::max();
  int i = 0; // iteration
  double max_iter = 15; // max number of iretations
  double max_error = 1e-8; // threshold

  // step control parameter
  double step = 1;
  double alpha = 0.5;
  double max_step = 1;
  Vector3d tmpResult = result.hip.translation;
  
  sloved = false;
  for (; i < max_iter; i++)
  {
    // calculate the joints fulfilling the result
    solveHipFeetIK(result);

    // calculate the kinematic chain and the com
    Kinematics::ForwardKinematics::updateKinematicChainFrom(theInverseKinematics.theKinematicChain, baseLink);
    theInverseKinematics.theKinematicChain.updateCoM();
    const Vector3d& obsCoM = theInverseKinematics.theKinematicChain.CoM;

    // calculate error
    Vector3d e = refCoM - obsCoM;
    double error = e.x * e.x + e.y * e.y + e.z * e.z*(!fix_height);

    // adjust step size
    if (bestError < error)
    {
      // the error becoms bigger, reset
      result.hip.translation = tmpResult;
      step *= alpha; // make smaller steps
    } else {
      bestError = error;
      tmpResult = result.hip.translation;
      step *= (1.0 + alpha); // make bigger steps
    }
    step = Math::clamp(step, max_error, max_step);

    // convergence
    if (bestError < max_error /*&& i > 0*/)
    {
      assert(result.hip.translation.x == tmpResult.x && 
             result.hip.translation.y == tmpResult.y &&
             result.hip.translation.z == tmpResult.z);
      sloved = true;
      break;
    }

    // calculate the update
    Vector3d u = e * step;

    // clampt the update
    double maxAdjustment = 50;
    MODIFY("IK_COM_CTR_MAX", maxAdjustment);

    u.x = Math::clamp(e.x * step, -maxAdjustment, maxAdjustment);
    u.y = Math::clamp(e.y * step, -maxAdjustment, maxAdjustment);
    u.z = Math::clamp(e.z * step, -maxAdjustment, maxAdjustment)*(!fix_height);

    result.hip.translation += u;
  }//end for
  
  // use the last best result
  result.hip.translation = tmpResult;

  if(!sloved) {
    std::cerr<<"Warning: control com was not solved @ "<<bestError<<std::endl;
  }

  if ( bestError > 1 ) {
    std::cerr<<"Warning: can not control CoM @ "<<bestError<<std::endl;
  }

  if( i >= max_iter ) {
    std::cerr<<"Warning: maximum iterations reached @ "<<bestError<<std::endl;
  }
  
  PLOT("NeoWalk:CalculatedCoM:x", theInverseKinematics.theKinematicChain.CoM.x);
  PLOT("NeoWalk:CalculatedCoM:y", theInverseKinematics.theKinematicChain.CoM.y);
  PLOT("NeoWalk:CalculatedCoM:z", theInverseKinematics.theKinematicChain.CoM.z);

  PLOT("NeoWalk:error", bestError);
  //std::cout << i << std::endl;

}//end controlCenterOfMassCool


void InverseKinematicsMotionEngine::feetStabilize(
  const InertialModel& theInertialModel,
  const GyrometerData& theGyrometerData,
  double (&position)[naoth::JointData::numOfJoint])
{
  const Vector2d& inertial = theInertialModel.orientation;
  const Vector3d& gyro = theGyrometerData.data;

  

  // HACK: small filter...
  static Vector3d lastGyro = gyro;
  Vector3d filteredGyro = (lastGyro+gyro)*0.5;

  Vector2d weight;
  weight.x = 
      getParameters().walk.stabilization.stabilizeFeetP.x * inertial.x
    + getParameters().walk.stabilization.stabilizeFeetD.x * filteredGyro.x;

  weight.y = 
      getParameters().walk.stabilization.stabilizeFeetP.y * inertial.y
    + getParameters().walk.stabilization.stabilizeFeetD.y * filteredGyro.y;


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


bool InverseKinematicsMotionEngine::rotationStabilize(
  const GyrometerData& theGyrometerData,
  double timeDelta,
  InverseKinematic::HipFeetPose& p)
{
  const double alpha = 0.5;
  Vector2d gyro = Vector2d(theGyrometerData.data.x, theGyrometerData.data.y);
  static Vector2d filteredGyro = gyro;
  filteredGyro = filteredGyro * (1.0f - alpha) + gyro * alpha;

  const double observerMeasurementDelay = 40;
  const int frameDelay = static_cast<int>(observerMeasurementDelay / (timeDelta*1000));

  static RingBuffer<Vector2d, 10> buffer;
  static Vector2d lastGyroError;
  static RotationMatrix lastBodyRotationMatrix = p.hip.rotation;

  const RotationMatrix relativeRotation = p.hip.rotation.invert() * lastBodyRotationMatrix;
  lastBodyRotationMatrix = p.hip.rotation;

  const double rotationY = atan2(relativeRotation.c[2].x, relativeRotation.c[2].z);
  buffer.add(Vector2d(relativeRotation.getXAngle(), rotationY));

  if(buffer.isFull() && frameDelay > 0 && frameDelay < buffer.size())
  {
    const Vector2d requestedVelocity = (buffer[frameDelay-1] - buffer[frameDelay]) / timeDelta;
    const Vector2d error = requestedVelocity - filteredGyro;
    const Vector2d errorDerivative = (error - lastGyroError) / timeDelta;

    double correctionY = getParameters().walk.stabilization.rotationP.y * error.y + 
                         getParameters().walk.stabilization.rotationD.y * errorDerivative.y;

    double correctionX = getParameters().walk.stabilization.rotationP.x * error.x + 
                         getParameters().walk.stabilization.rotationD.x * errorDerivative.x;

    p.localInHip();
    p.hip.rotateX(correctionX);
    p.hip.rotateY(correctionY);

    lastGyroError = error;
  }

  return true;
}

bool InverseKinematicsMotionEngine::rotationStabilize(
  const InertialModel& theInertialModel,
  const GyrometerData& theGyrometerData,
  Pose3D& hip)
{
  Vector2d r;
  r.x = hip.rotation.getXAngle();
  r.y = hip.rotation.getYAngle();

  PLOT("rotationStabilize:hip:x", Math::toDegrees(hip.rotation.getXAngle()));
  PLOT("rotationStabilize:hip:y", Math::toDegrees(hip.rotation.getYAngle()));

  const Vector2d& inertial = theInertialModel.orientation;
  const Vector3d& gyro = theGyrometerData.data;

  // HACK: small filter...
  static Vector3d lastGyro = gyro;
  Vector3d filteredGyro = (lastGyro+gyro)*0.5;

  PLOT("rotationStabilize:gyro:x", Math::toDegrees(lastGyro.x));
  PLOT("rotationStabilize:gyro:y", Math::toDegrees(lastGyro.y));

  Vector2<double> weight;
  weight.x = 
      getParameters().walk.stabilization.rotationP.x * inertial.x
    + getParameters().walk.stabilization.rotationD.x * filteredGyro.x;

  weight.y = 
      getParameters().walk.stabilization.rotationP.y * inertial.y
    + getParameters().walk.stabilization.rotationD.y * filteredGyro.y;


  double height = NaoInfo::ThighLength + NaoInfo::TibiaLength + NaoInfo::FootHeight;
      //theBlackBoard.theKinematicChain.theLinks[KinematicChain::Hip].p.z;
  hip.translate(0, 0, -height);
  hip.rotateX(weight.x);
  hip.rotateY(weight.y);
  hip.translate(0, 0, height);

  PLOT("rotationStabilize:stabilizer:x", Math::toDegrees(hip.rotation.getXAngle()));
  PLOT("rotationStabilize:stabilizer:y", Math::toDegrees(hip.rotation.getYAngle()));

  lastGyro = gyro;
  return true;
}//end rotationStabilize


bool InverseKinematicsMotionEngine::rotationStabilize(
  const RobotInfo& theRobotInfo,
  const GroundContactModel& theGroundContactModel,
  const naoth::InertialSensorData& theInertialSensorData,
  Pose3D& hip)
{
  // disable stablization slowly when no foot is on the ground
  const double switchingTime = 3000; // ms
  const double switchingRate = theRobotInfo.basicTimeStep / switchingTime;
  //if (theBlackBoard.theSupportPolygon.mode == SupportPolygon::NONE)
  if(!theGroundContactModel.leftGroundContact &&
     !theGroundContactModel.rightGroundContact) 
  {
    rotationStabilizeFactor -= switchingRate;
  } else {
    rotationStabilizeFactor += switchingRate;
  }

  rotationStabilizeFactor = Math::clamp(rotationStabilizeFactor, 0.0, 1.0);
  PLOT("rotationStabilizeFactor", rotationStabilizeFactor);

  Vector2d r;
  r.x = hip.rotation.getXAngle();
  r.y = hip.rotation.getYAngle();

  PLOT("rotationStabilize:hip:x", Math::toDegrees(hip.rotation.getXAngle()));
  PLOT("rotationStabilize:hip:y", Math::toDegrees(hip.rotation.getYAngle()));

  //const Vector2d& s = theBlackBoard.theInertialPercept.data;
  const Vector2d& s = theInertialSensorData.data;

  Vector2d e = r - s;

  bool isWorking = false;
  Vector2d chestRotationStabilizerValue;
  const double maxAngle = Math::fromDegrees(30);
  for( int i=0; i<2; i++ )
  {
    double threshold = Math::fromDegrees(getParameters().rotationStabilize.threshold[i]);
    if ( fabs(e[i]) > threshold )
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
  static const Vector3d footOffset(0,0,-NaoInfo::FootHeight);
  
  double err = theInverseKinematics.gotoLegs(chest, p.feet.left, p.feet.right, footOffset, footOffset);

  if (fabs(err) > Math::fromDegrees(1))
  {
    THROW("IK failed!");
  }
}//end solveHipFeetIK
  
void InverseKinematicsMotionEngine::copyLegJoints(double (&position)[naoth::JointData::numOfJoint]) const
{
  const double* l = theInverseKinematics.theJointData.position;
  for (int i = JointData::RHipYawPitch; i <= JointData::LAnkleRoll; i++)
  {
    position[i] = l[i];
  }
  // TODO: this is not enough, currently a solution of BH is used from InverseKinematicsBH.h
  // use mean value of two hips
  double hipYawPitch = (position[JointData::LHipYawPitch] + position[JointData::RHipYawPitch]) * 0.5;
  position[JointData::LHipYawPitch] = hipYawPitch;
  position[JointData::RHipYawPitch] = hipYawPitch;
}//end copyLegJoints


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
  thePreviewControldCoM = Vector2d(0,0);
  thePreviewControlddCoM = Vector2d(0,0);
  thePreviewController.init(currentCoMPose.com.translation, thePreviewControldCoM, thePreviewControlddCoM);
  
  unsigned int previewSteps = static_cast<unsigned int> (thePreviewController.previewSteps() - 1);
  thePreviewController.clear();

  for (unsigned int i = 0; i < previewSteps; i++)
  {
    double t = static_cast<double>(i) / previewSteps;
    Pose3D p = Pose3D::interpolate(currentCoMPose.com, start.zmp, t);
    thePreviewController.push(p.translation);
  }
  return previewSteps;
}//end controlZMPstart


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

double InverseKinematicsMotionEngine::solveHandsIK(
  const Pose3D& chest,
  const Pose3D& leftHand,
  const Pose3D& rightHand,
  double (&position)[naoth::JointData::numOfJoint])
{
  // TODO: is it a good place for it?
  static const Vector3<double> lHandOffset(NaoInfo::LowerArmLength+NaoInfo::HandOffsetX,0,0);
  static const Vector3<double> rHandOffset(NaoInfo::LowerArmLength+NaoInfo::HandOffsetX,0,0);

  static const Kinematics::InverseKinematics::Mask lHandMask(Kinematics::InverseKinematics::MASK_POS);
  static const Kinematics::InverseKinematics::Mask rHandMask(Kinematics::InverseKinematics::MASK_POS);

  // STEP 1: transform the whole chain into torso (for symmetrie reasons)
  //         otherwise, there is a risk of getting a assymetric solution
  Kinematics::Link& torsoLink = theInverseKinematics.theKinematicChain.theLinks[KinematicChain::Torso];
  torsoLink.R = RotationMatrix();
  torsoLink.p = Vector3<double>(0, 0, 0);
  Kinematics::ForwardKinematics::updateKinematicChainFrom(&torsoLink);

  // STEP 2: solve the inverse kinematic for arms
  double error = theInverseKinematics.gotoArms(
    chest,
    leftHand,
    rightHand,
    lHandOffset,
    rHandOffset,
    lHandMask,
    rHandMask);

  // STEP 3: copy the calculated joint angles of the arms
  const JointData& jointData = theInverseKinematics.theJointData;
  for (int i = JointData::RShoulderRoll; i < JointData::RHipYawPitch; i++)
  {
    position[i] = jointData.position[i];
  }

  return error;
}//end solveHandsIK


void InverseKinematicsMotionEngine::autoArms(
  const RobotInfo& theRobotInfo,
  const HipFeetPose& /* pose */,
  double (&position)[JointData::numOfJoint])
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
  double max_speed = Math::fromDegrees(getParameters().arm.maxSpeed) * theRobotInfo.getBasicTimeStepInSecond();
  for (int i = JointData::RShoulderRoll; i <= JointData::LElbowYaw; i++)
  {
    double s = target[i] - position[i];
    s = Math::clamp(s, -max_speed, max_speed);
    position[i] += s;
  }
  //----------------------------------------------
}//end autoArms

void InverseKinematicsMotionEngine::armsOnBack(
  const RobotInfo& theRobotInfo,
  const HipFeetPose& /* pose */,
  double (&position)[JointData::numOfJoint])
{
  double target[JointData::LElbowYaw + 1];
  target[JointData::RShoulderRoll] = Math::fromDegrees(-90);
  target[JointData::LShoulderRoll] = Math::fromDegrees(90);
  target[JointData::RShoulderPitch] = Math::fromDegrees(119);
  target[JointData::LShoulderPitch] = Math::fromDegrees(119);
  target[JointData::RElbowRoll] = Math::fromDegrees(30);
  target[JointData::LElbowRoll] = Math::fromDegrees(-30);
  target[JointData::RElbowYaw] = Math::fromDegrees(-25);
  target[JointData::LElbowYaw] = Math::fromDegrees(25);

  // make sure the arms do not collide legs --------------
  double diffR = target[JointData::RShoulderPitch] - position[JointData::RShoulderPitch];
  double diffL = target[JointData::LShoulderPitch] - position[JointData::LShoulderPitch];

  if( (diffR + diffL) / 2 <= 0.02)
  {
    // limit the max speed -----------------------------
    double max_speed = Math::fromDegrees(getParameters().arm.maxSpeed) * theRobotInfo.getBasicTimeStepInSecond();
    for (int i = JointData::RElbowRoll; i <= JointData::LElbowYaw; i++)
    {
      double s = target[i] - position[i];
      s = Math::clamp(s, -max_speed, max_speed);
      position[i] += s;
    }
    diffR = target[JointData::RElbowRoll] - position[JointData::RElbowRoll];
    diffL = target[JointData::LElbowRoll] - position[JointData::LElbowRoll];
    if( (diffR + diffL) / 2 <= 0.02)
    {
      target[JointData::RShoulderRoll] = 0.0;
      target[JointData::LShoulderRoll] = 0.0;
      for (int i = JointData::RShoulderRoll; i <= JointData::LShoulderRoll; i++)
      {
        double s = target[i] - position[i];
        s = Math::clamp(s, -max_speed, max_speed);
        position[i] += s;
      }
    }
  }
  else
  {
    target[JointData::RElbowRoll] = 0.0;
    target[JointData::RElbowRoll] = 0.0;
    // limit the max speed -----------------------------
    double max_speed = Math::fromDegrees(getParameters().arm.maxSpeed) * theRobotInfo.getBasicTimeStepInSecond();
    for (int i = JointData::RShoulderRoll; i <= JointData::LElbowYaw; i++)
    {
      double s = target[i] - position[i];
      s = Math::clamp(s, -max_speed, max_speed);
      position[i] += s;
    }
  }
  //---------------------------------------------
}//end armsOnBack


// calculates the com of theKinematicChain in the coordinates of the link 
Vector3d InverseKinematicsMotionEngine::sensorCoMIn(
  const KinematicChainSensor& theKinematicChain,
  KinematicChain::LinkID link) const
{
  Pose3D foot = theKinematicChain.theLinks[link].M;
  foot.translate(0, 0, -NaoInfo::FootHeight);
  foot.rotation = RotationMatrix(); // assume the foot is flat on the ground
  return foot.invert() * theKinematicChain.CoM;
}

// compares expected com and com from sensors
// @return adjust applyed to hip
Vector3d InverseKinematicsMotionEngine::balanceCoM(
  const FrameInfo& theFrameInfo,
  const KinematicChainSensor& theKinematicChain,
  const Vector3d& lastReqCoM, 
  KinematicChain::LinkID link) const
{
  static unsigned int frameNumber = theFrameInfo.getFrameNumber();
  static Vector3<double> uP, uI, uD;
  if ( theFrameInfo.getFrameNumber() > frameNumber + 1)
  {
    // reset
    uP = Vector3d::zero;
    uI = Vector3d::zero;
    uD = Vector3d::zero;
  }

  ASSERT(link==KinematicChain::LFoot || link==KinematicChain::RFoot );
  Vector3d sensorCoM = sensorCoMIn(theKinematicChain, link);

  Vector3d e = lastReqCoM - sensorCoM;

  Vector3<double> u;
  for( int i=0; i<3; i++)
  {
    if (fabs(e[i]) > theParameters.balanceCoM.threshold)
    {
      u[i] = e[i] - Math::sgn(e[i]) * theParameters.balanceCoM.threshold;
    }
  }
  uI += u;
  uD = u - uP;
  uP = u;
  frameNumber = theFrameInfo.getFrameNumber();
  u = uP * theParameters.balanceCoM.kP + uI * theParameters.balanceCoM.kI + uD * theParameters.balanceCoM.kD;
  for(int i=0; i<3; i++)
  {
    u[i] = Math::clamp(u[i], -30.0, 30.0);
  }
  return u;
}//end balanceCoM


void InverseKinematicsMotionEngine::gotoArms(
  const MotionStatus& theMotionStatus,
  const InertialModel& theInertialModel,
  const RobotInfo& theRobotInfo,
  const InverseKinematic::HipFeetPose& currentPose, 
  double (&position)[JointData::numOfJoint])
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

  if ( (getParameters().arm.kickEnabled && theMotionStatus.currentMotion == motion::kick)
    || (getParameters().arm.walkEnabled && theMotionStatus.currentMotion == motion::walk))
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
    || (theMotionStatus.currentMotion == motion::walk && getParameters().walk.general.useArm))
  {
    // TODO: InertialSensorData may be better
    const InertialModel& isd = theInertialModel;
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
  double max_speed = Math::fromDegrees(getParameters().arm.maxSpeed) * theRobotInfo.getBasicTimeStepInSecond();
  for (int i = JointData::RShoulderRoll; i <= JointData::LElbowYaw; i++)
  {
    double s = target[i] - position[i];
    s = Math::clamp(s, -max_speed, max_speed);
    position[i] += s;
  }
  //----------------------------------------------
}//end gotoArms

