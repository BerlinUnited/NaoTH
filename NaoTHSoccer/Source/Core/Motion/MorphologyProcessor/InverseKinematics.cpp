/**
* @file InverseKinematics.cpp
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* @author <a href="mailto:welter@informatik.hu-berlin.de">Welter, Oliver</a>
* Implementation of Inverse Kinematics
*/

#include "PlatformInterface/Platform.h"

#include "Tools/Debug/Stopwatch.h"

// Tools
#include "Tools/Math/Common.h"
#include "Tools/Math/RotationMatrix.h"
//#include "Tools/TemplateUtility.h"
#include "Tools/NaoInfo.h"

#include "InverseKinematics.h"


namespace Kinematics {

  using namespace naoth;
  using namespace std;
  
InverseKinematics::InverseKinematics(bool autoInit)
  : maxError(1.0)
{
  if (autoInit){
    init();
  }
}

void InverseKinematics::init()
{
  theKinematicChain.init(theJointData);
}

InverseKinematics::~InverseKinematics()
{
}

void InverseKinematics::calcJacobi(const list<Link*>& linkList, const Vector3<double>& affector, Matrix_nxn<double, 6>& J) const
{
  ASSERT(linkList.size()==6);
  Vector3<double> a;
  Vector3<double> n;
  
  int i=0;
  for(list<Link*>::const_iterator it = linkList.begin(); it != linkList.end(); ++it)
  {
    Link* link = *(it);
    Vector3<double> dp = affector - link->p;
    a = link->R * link->a;
    n = a^dp;

    J[0][i] = n.x;
    J[1][i] = n.y;
    J[2][i] = n.z;
    J[3][i] = a.x;
    J[4][i] = a.y;
    J[5][i] = a.z;
    i++;
  }
}//end calcJacobi

bool InverseKinematics::updateJoints(const list<Link*>& linkList, const Vector_n<double,6>& dq) const
{
  ASSERT(linkList.size()==6);
  int i = 0;
  bool updated = false;

  for(list<Link*>::const_iterator it = linkList.begin(); it != linkList.end(); ++it)
  {
    Link* j = *it;
    if (!lock[j->jointID])
    {
      *(j->q) += dq[i];
      // TODO: do we need the magic number 0.001 ?
      if (!updated && fabs(dq[i]) > Math::fromDegrees(0.001)) updated = true;
    }
    i++;
  }
  return updated;
}//end updateJoints

double InverseKinematics::gotoTargetJacobianInversion(const list<Link*>& linkList, const Pose3D& target, const Vector3<double>& offset, Mask mask) const
{
  const double lambda = 1;
  const int max_iterations = 100;
  Pose3D affector;

  Vector_n<double,6> dq;
  Vector_n<double,6> err;
  static Matrix_nxn<double, 6 > J;
  double errlen2 = 0;
  int i = 0;

  while (true)
  {
    calculateAffector(linkList, offset, affector);
    calcError(affector, target, err, mask);
    errlen2 = errlen(err);
    if (errlen2 < maxError || i++ > max_iterations)
    {
      break;
    }
    
    calcJacobi(linkList, affector.translation, J);
    try
    {
      dq = J.solve(err) * lambda;
    } catch (MVException &e)
    {
      cerr << "IK(" << i << ") " << e.getDescription() << endl; //TODO handle singular matrix
      break;
    }
    if ( !updateJoints(linkList, dq) ) break;
  }//end while

  normalizeLinkAngles(linkList);
  return errlen2;
}//end gotoTargetJacobianInversion


double InverseKinematics::gotoTargetJacobianTranspose(const list<Link*>& linkList, const Pose3D& target, const Vector3<double>& offset, Mask mask) const
{
  const double lambda = 0.00001;
  const int max_iterations = 100;
  Pose3D affector;

  Vector_n<double,6> dq;
  Vector_n<double,6> err;
  static Matrix_nxn<double, 6 > J;
  double errlen2 = 0;

  for(int i=0;i<max_iterations;i++)
  {
    calculateAffector(linkList, offset, affector);
    calcError(affector, target, err, mask);
    errlen2 = errlen(err);
    if (errlen2 < maxError )
    {
      normalizeLinkAngles(linkList);
      return errlen2;
    }
    calcJacobi(linkList, affector.translation, J);

    dq = lambda * ( J.transpose() * err );

    updateJoints(linkList, dq);
  }//end for

  normalizeLinkAngles(linkList);
  return errlen2;
}//end gotoTargetJacobianTranspose


double InverseKinematics::gotoTargetCCD(const list<Link*>& linkList, const Pose3D& target, const Vector3<double>& offset, Mask mask) const
{
  // TODO: parameter?  
  const double alpha = 0.00001; 
  const double max_error = Math::fromDegrees(1);
  const int max_iterations = 1000; //was 100

  const double wo = 1;
  const double wi = 1;
  const double wo0 = (mask&MASK_WX) ? wo : 0;
  const double wo1 = (mask&MASK_WY) ? wo : 0;
  const double wo2 = (mask&MASK_WZ) ? wo : 0;
  
  Pose3D affector;
  const RotationMatrix& uc = affector.rotation;
  const RotationMatrix& ud = target.rotation;

  calculateAffector(linkList, offset, affector);
  for (int i = 0; i < max_iterations; i++)
  {
    double delta = 0;
 
    // iterate over all joints
    for (list<Link*>::const_reverse_iterator iter = linkList.rbegin(); iter != linkList.rend(); ++iter)
    {
      Link* link = *iter;
      if (lock[link->jointID] || link->q == NULL ) continue;

      Vector3<double> Pid = target.translation - link->p;
      const double Lid = Pid.abs();
      Vector3<double> axis = link->R * link->a;
      Vector3<double> Pic = affector.translation - link->p;
      if (0 == (mask & MASK_X)) Pic[0] = 0;
      if (0 == (mask & MASK_Y)) Pic[1] = 0;
      if (0 == (mask & MASK_Z)) Pic[2] = 0;
      const double Lic = Pic.abs();
      const double ruo = std::min(Lid, Lic) / std::max(Lid, Lic);

      ASSERT(!Math::isNan(ruo));

      const double wp = alpha * (1 + ruo);

      double k1 = wp * (Pid * axis)*(Pic * axis) + (wo0 * (ud[0] * axis)*(uc[0] * axis) + wo1 * (ud[1] * axis)*(uc[1] * axis) + wo2 * (ud[2] * axis)*(uc[2] * axis));
      double k2 = wp * (Pid * Pic) + (ud[0] * uc[0] * wo0 + ud[1] * uc[1] * wo1 + ud[2] * uc[2] * wo2);
      double k3 = axis * ((Pic^Pid) * wp + (uc[0]^ud[0]) * wo0 + (uc[1]^ud[1]) * wo1 + (uc[2]^ud[2]) * wo2);
      double theta = atan2(k3, k2 - k1) * wi;

      (*(link->q)) += theta;
      delta += fabs(theta);
      calculateAffector(linkList, link, offset, affector);
    }
    if (delta < max_error)
    {
      //            cout << "break @ " << i << endl;
      break;
    }
  }//end for

  // calcluate the error of result
  Vector_n<double,6> err;
  calcError(affector, target, err, mask);
  return errlen(err);
}//end gotoTargetCCD



double InverseKinematics::calculateLowerNaoLegJointsAnalyticaly(bool leftLeg, Pose3D& target)
{
  const Link& pelvis = theKinematicChain.theLinks[leftLeg?KinematicChain::LPelvis:KinematicChain::RPelvis];

  JointData::JointID kneePitch = leftLeg ? JointData::LKneePitch : JointData::RKneePitch;
  JointData::JointID anklePitch = leftLeg ? JointData::LAnklePitch : JointData::RAnklePitch;
  JointData::JointID ankleRoll = leftLeg ? JointData::LAnkleRoll : JointData::RAnkleRoll;

  // TODO: transform to the targets koordinates?
  Vector3<double> r = target.invert() * pelvis.b;
  double C = r.abs();
  
  const double maxlen = NaoInfo::ThighLength + NaoInfo::TibiaLength;
  double kneeAng = 0;
  //double error = 0;
  if (C >= maxlen )
  {
    //cerr << "InverseKinematics can not reach this pose! length=" << C << endl;
//    error = C - maxlen;
    C = maxlen;
    r.normalize(C);
    target.translation = pelvis.b - target.rotation * r;
  }
  else
  {
    double c3 = (Math::sqr(NaoInfo::ThighLength) + Math::sqr(NaoInfo::TibiaLength) - Math::sqr(C))
      / (2.0 * NaoInfo::ThighLength * NaoInfo::TibiaLength);
    kneeAng = Math::normalizeAngle(Math::pi - acos(c3));
  }
    
  //
  theJointData.position[kneePitch] = kneeAng;

  //
  double alpha = asin((NaoInfo::ThighLength / C) * sin(Math::pi - kneeAng));
  theJointData.position[anklePitch] = -(atan2(r.x, (Math::sgn(r.z) * sqrt(Math::sqr(r.y) + Math::sqr(r.z)))) + alpha);

  theJointData.position[ankleRoll] = atan2(r.y, r.z);
  
  return 0;//error;
}//end calculateLowerLegJointsAnalyticaly


double InverseKinematics::calculateNaoLegJointsAnalyticaly(bool leftLeg, Pose3D& target)
{
  // calculate the last three joints
  double error = calculateLowerNaoLegJointsAnalyticaly(leftLeg, target);

  JointData::JointID hipYawPitch = leftLeg ? JointData::LHipYawPitch : JointData::RHipYawPitch;
  JointData::JointID hipRoll     = leftLeg ? JointData::LHipRoll     : JointData::RHipRoll;
  JointData::JointID hipPitch    = leftLeg ? JointData::LHipPitch    : JointData::RHipPitch;
  JointData::JointID kneePitch   = leftLeg ? JointData::LKneePitch   : JointData::RKneePitch;
  JointData::JointID anklePitch  = leftLeg ? JointData::LAnklePitch  : JointData::RAnklePitch;
  JointData::JointID ankleRoll   = leftLeg ? JointData::LAnkleRoll   : JointData::RAnkleRoll;

  RotationMatrix Foot2Thigh =
    RotationMatrix::getRotationX(-theJointData.position[ankleRoll])
    .rotateY(-theJointData.position[kneePitch]-theJointData.position[anklePitch]);

  double rotX = leftLeg?-Math::pi_4:Math::pi_4; //Math::fromDegrees(leftLeg?-45.0:45.0);
  RotationMatrix HipO2Foot = RotationMatrix::getRotationX(rotX) * target.rotation;

  RotationMatrix HipO2Thigh = HipO2Foot * Foot2Thigh;

  theJointData.position[hipRoll]     = asin(HipO2Thigh[1][2]) - rotX;
  theJointData.position[hipYawPitch] = atan2(-HipO2Thigh[1][0] , HipO2Thigh[1][1] ) * (leftLeg?-1.0:1.0);
  theJointData.position[hipPitch]    = atan2(-HipO2Thigh[0][2] , HipO2Thigh[2][2] );
  
  return error;
}//end calculateNaoLegJointsAnalyticaly

double InverseKinematics::gotoLeg(bool leftLeg, Pose3D& target, const Vector3<double>& offset, Mask mask)
{
  // solve analyticaly (for Nao only)
  if ( mask == MASK_ALL ) return gotoLegAnalytical(leftLeg, target, offset); 

  // solve general inverse kinematics with CCD
  list<Link*> linkList;
  getRoute(leftLeg ? KinematicChain::LFoot : KinematicChain::RFoot, linkList);
  return gotoTarget(linkList, target, offset, mask);
}// end gotoLeg


double InverseKinematics::gotoLegAnalytical(bool leftLeg, Pose3D& target, const Vector3<double>& offset)
{
  target.translation -= target.rotation*offset;
  double error = calculateNaoLegJointsAnalyticaly(leftLeg, target);
  target.translation += target.rotation*offset;

  //TODO: calculate error for the position?
  return error;
}//end gotoLeg


double InverseKinematics::gotoLegs(
  const Pose3D& tc, 
  Pose3D tlf, 
  Pose3D trf,
  const Vector3<double>& leftOffset, 
  const Vector3<double>& rightOffset,
  Mask leftFootMask, 
  Mask rightFootMask)
{
  STOPWATCH_START("InverseKinematics");

  tlf = tc.local(tlf);
  trf = tc.local(trf);

  // unlock all joints
  for (int i = 0; i < JointData::numOfJoint; i++)
  {
    lock[i] = false;
  }

  // calculate the right and the left foot
  double lerr = gotoLeg(true, tlf, leftOffset);
  double rerr = gotoLeg(false, trf, rightOffset);
  
  // calculate the hip-yaw-pitch deviation
  // NOTE: they have to be references
  double& lhyp = theJointData.position[JointData::LHipYawPitch];
  double& rhyp = theJointData.position[JointData::RHipYawPitch];
  double err = lhyp - rhyp;


  // try to adjust the legs (max 5 times) ...
  const double max_hip_error = Math::fromDegrees(0.5);
  const int max_trials = 5;
  int trials = 0;
  while ( abs(err) > max_hip_error ||  lerr > maxError || rerr > maxError )
  {
    // TODO: fixme!!!
    if (true || trials < 2 || ( (leftFootMask == MASK_ALL) && (rightFootMask == MASK_ALL)) )
    {
      double derr = err * 0.25;
      RotationMatrix rot = RotationMatrix::getRotationZ(derr);
      tlf.rotation *= rot;
      tlf.translation = rot * tlf.translation;
      trf.rotation *= rot;
      trf.translation = rot * trf.translation;
      lerr = gotoLeg(true, tlf, leftOffset);
      rerr = gotoLeg(false, trf, rightOffset);
    }
    else
    {
      if (leftFootMask == MASK_ALL)
      {
        if ( lerr > maxError ) lerr =gotoLeg(true, tlf, leftOffset, leftFootMask);
        rhyp = lhyp;
        lock[JointData::RHipYawPitch] = true;
        rerr = gotoLeg(false, trf, rightOffset);
        rerr = gotoLeg(false, trf, rightOffset, rightFootMask);
      } else // ASSERT(rightFootMask == MASK_ALL)
      {
        if ( rerr > maxError )
        {
          rerr = gotoLeg(false, trf, rightOffset, rightFootMask);
        }
        lhyp = rhyp;
        lock[JointData::LHipYawPitch] = true;
        lerr = gotoLeg(true, tlf, leftOffset);
        lerr = gotoLeg(true, tlf, leftOffset, leftFootMask);
      }
    }

    err = lhyp - rhyp;

    if ( trials++ > max_trials)
    {
      cerr<<"InverseKinematics:Warning lhyp-rhyp=" << Math::toDegrees(err) << " lerr="<<lerr<<" rerr="<<rerr<<endl;
      break;
    }
  }//end while

    
  STOPWATCH_STOP("InverseKinematics");
  return lerr + rerr;
}//end gotoLegs



double InverseKinematics::gotoArms(
  const Pose3D& tc, 
  const Pose3D& targetLeftHand, 
  const Pose3D& targetRightHand,
  const Vector3<double>& leftOffset, 
  const Vector3<double>& rightOffset,
  Mask leftHandMask, 
  Mask rightHandMask)
{
  STOPWATCH_START("InverseKinematics --- ARMS");

  // transform to the chest coordinates
  Pose3D tlh = tc.local(targetLeftHand);
  Pose3D trh = tc.local(targetRightHand);

  // unlock all joints
  for (int i = 0; i < JointData::numOfJoint; i++)
  {
    lock[i] = false;
  }

  //fix the ElbowYaw joint
  lock[JointData::LElbowYaw] = true;
  lock[JointData::RElbowYaw] = true;
  theJointData.position[JointData::LElbowYaw] = 0.0;
  theJointData.position[JointData::RElbowYaw] = -0.0;

  // calculate the right and the left arm
  double lerr = gotoArm(true, tlh, leftOffset, leftHandMask);
  double rerr = gotoArm(false, trh, rightOffset, rightHandMask);

  // TODO: handle error

  STOPWATCH_STOP("InverseKinematics --- ARMS");
  return lerr + rerr;
}//end gotoArms


double InverseKinematics::gotoArm(bool leftArm, const Pose3D& target, const Vector3<double>& offset, Mask mask)
{
  list<Link*> linkList;
  getRoute(leftArm?KinematicChain::LForeArm:KinematicChain::RForeArm, linkList);
  return gotoTarget(linkList, target, offset, mask, CCD);
}//end gotoArm


void InverseKinematics::calcError(const Pose3D& current, const Pose3D& target, Vector_n<double,6>& result, Mask mask)
{
  Vector3<double> perr = target.translation - current.translation;
  RotationMatrix rerr = current.rotation.invert() * target.rotation;
  Vector3<double> n = rerr.toQuaternion();
  Vector3<double> werr = current.rotation * n;
  result[0] = (mask&MASK_X) ? perr[0] : 0;
  result[1] = (mask&MASK_Y) ? perr[1] : 0;
  result[2] = (mask&MASK_Z) ? perr[2] : 0;
  result[3] = (mask&MASK_WX) ? werr[0] : 0;
  result[4] = (mask&MASK_WY) ? werr[1] : 0;
  result[5] = (mask&MASK_WZ) ? werr[2] : 0;
}//end calcError

double InverseKinematics::errlen(const Vector_n<double,6>& e)
{
  double error = Math::sqr(e[0]) + Math::sqr(e[1]) + Math::sqr(e[2])
    + Math::toDegrees(Math::sqr(e[3]) + Math::sqr(e[4]) + Math::sqr(e[5]));
  return error;
}//end errlen

void InverseKinematics::normalizeLinkAngles(const list<Link*>& linkList)
{
  // normalize angles
  for(list<Link*>::const_iterator it = linkList.begin(); it != linkList.end(); ++it)
  {
    (*it)->normalizeJointAngle();
  }
}//end normalizeLinkAngles

void InverseKinematics::getRoute(Link* actuatorLink, list<Link*>& linkList)
{
  Link *m = actuatorLink->mother;
  if(NULL != m)
  {
    getRoute(m, linkList);
  }
  linkList.push_back(actuatorLink);
}//end getRoute

void InverseKinematics::linkListForwardKinematics(const list<Link*>& linkList)
{
  for (list<Link*>::const_iterator iter = linkList.begin(); iter != linkList.end(); ++iter)
  {
    (*iter)->updateFromMother();
  }
}//end linkListForwardKinematics

void InverseKinematics::linkListForwardKinematics(const list<Link*>& linkList, Link* beginLink)
{
  bool start = false;
  for ( list<Link*>::const_iterator iter = linkList.begin(); iter != linkList.end(); ++iter )
  {
    Link* l = *iter;
    if ( l == beginLink ) start = true;

    if (start)
    {
      l->updateFromMother();
    }
  }//end for
}//end linkListForwardKinematics

void InverseKinematics::calculateAffector(const list<Link*>& linkList, const Vector3<double>& offset, Pose3D& affector)
{
  linkListForwardKinematics(linkList);
  Link* end = *linkList.rbegin();
  affector.rotation = end->R;
  affector.translation = end->R * offset + end->p;
}//end calculateAffector

void InverseKinematics::calculateAffector(const list<Link*>& linkList, Link* beginLink, const Vector3<double>& offset, Pose3D& affector)
{
  linkListForwardKinematics(linkList, beginLink);
  Link* end = *linkList.rbegin();
  affector.rotation = end->R;
  affector.translation = end->R * offset + end->p;
}//end calculateAffector



void InverseKinematics::getRoute(KinematicChain::LinkID actuatorID, list<Link*>& linkList)
{
  getRoute(&theKinematicChain.theLinks[actuatorID], linkList);
}//end getRoute





void InverseKinematics::test()
{
    cout << "************** Test LEGS Inverse Kinematics ******************\n";
    double refAng[JointData::numOfJoint];
    for (unsigned int i = 0; i < JointData::numOfJoint; i++)
    {
        refAng[i] = Math::random(theJointData.min[i], theJointData.max[i]);
        theJointData.position[i] = refAng[i];
    }
    refAng[JointData::LHipYawPitch] = refAng[JointData::RHipYawPitch];
    theJointData.position[JointData::LHipYawPitch] = theJointData.position[JointData::RHipYawPitch];

    cout<<"joints: ";
    for (int i = 10; i < JointData::numOfJoint; i++)
    {
        cout<<JointData::getJointName((JointData::JointID)i)<<"="<<Math::toDegrees(theJointData.position[i])<<' ';
    }
    cout<<endl<<endl;

  list<Link*> leftList, rightList;
  getRoute(KinematicChain::LFoot, leftList);
  getRoute(KinematicChain::RFoot, rightList);
  theKinematicChain.theLinks[KinematicChain::Torso].p = Vector3<double>(0, 0, 0);
  theKinematicChain.theLinks[KinematicChain::Torso].R = RotationMatrix::getRotationZ(0);
  Vector3<double> footOffset(0, 0, -NaoInfo::FootHeight);
  Pose3D leftTarget, rightTarget;
  calculateAffector(leftList, footOffset, leftTarget);
  calculateAffector(rightList, footOffset, rightTarget);
  Pose3D chestTarget(theKinematicChain.theLinks[KinematicChain::Torso].R, theKinematicChain.theLinks[KinematicChain::Torso].p);
  //leftTarget.rotateX(Math::fromDegrees(90));
  cout << "left target foot: " << leftTarget.translation <<" ("<< leftTarget.translation.abs() << ")"<< endl;

  int count = 0;
  while (count++ < 1)
  {
    for (unsigned int i = 0; i < JointData::numOfJoint; i++)
    {
      theJointData.position[i] = Math::random(theJointData.min[i], theJointData.max[i]);
    }
    theJointData.position[JointData::LHipYawPitch] = theJointData.position[JointData::RHipYawPitch];
    linkListForwardKinematics(leftList);
    linkListForwardKinematics(rightList);

    double err = gotoLegs(chestTarget, leftTarget, rightTarget, footOffset, footOffset, MASK_POS);

    Pose3D lf, rf;
    calculateAffector(leftList, footOffset, lf);
    calculateAffector(rightList, footOffset, rf);

    cout << "err (" << err << ") : \n";
    for (int i = 10; i < JointData::numOfJoint; i++)
    {
      cout << JointData::getJointName((JointData::JointID)i) << "=" << Math::toDegrees(theJointData.position[i] - refAng[i]) << ' ';
    }
    cout << endl<<endl;
    lf.rotation -= leftTarget.rotation;
    rf.rotation -= rightTarget.rotation;
    lf.translation -= leftTarget.translation;
    rf.translation -= rightTarget.translation;
    //cout << "left : " <<  lf << endl;
    //cout << "right : " << rf << endl;
    cout << "left hand translation: " <<  lf.translation << endl;
    cout << "left hand rotation: " <<  lf.rotation << endl << endl;
    cout << "right hand translation: " << rf.translation << endl;
    cout << "right hand rotation: " << rf.rotation << endl << endl;
  }
}


void InverseKinematics::testArms()
{
    cout << "******/\\****** Test ARMS Inverse Kinematics *********/\\*******\n";
    double refAng[JointData::numOfJoint];
    for (unsigned int i = 0; i < JointData::numOfJoint; i++)
    {
        refAng[i] = Math::random(theJointData.min[i], theJointData.max[i]);
        theJointData.position[i] = refAng[i];
    }
    refAng[JointData::LHipYawPitch] = refAng[JointData::RHipYawPitch];
    theJointData.position[JointData::LHipYawPitch] = theJointData.position[JointData::RHipYawPitch];

    cout<<"joints: ";
    for (int i = JointData::RShoulderRoll; i < JointData::RHipYawPitch; i++)
    {
        cout<<JointData::getJointName((JointData::JointID)i)<<"="<<Math::toDegrees(theJointData.position[i]/*theKinematicChain.theLinks[i].q*/)<<' ';
    }
    cout<<endl<<endl;

  list<Link*> leftList, rightList;
  getRoute(KinematicChain::LForeArm, leftList);

  cout << "KINEMATIC CHAIN >LEFT<:" << endl;
    for(list<Link*>::const_reverse_iterator iter = leftList.rbegin(); iter != leftList.rend(); ++iter)
    {
        cout << KinematicChain::getLinkName(*iter) << endl;
    }

  getRoute(KinematicChain::RForeArm, rightList);

  cout << "KINEMATIC CHAIN >RIGHT<:" << endl;
    for(list<Link*>::const_reverse_iterator iter = rightList.rbegin(); iter != rightList.rend(); ++iter)
    {
        cout << KinematicChain::getLinkName(*iter) << endl;
    }

  theKinematicChain.theLinks[KinematicChain::Torso].p = Vector3<double>(0, 0, 0);
  theKinematicChain.theLinks[KinematicChain::Torso].R = RotationMatrix::getRotationZ(0);
  Vector3<double> handOffset(0, 0, 0);
  Pose3D leftTarget, rightTarget;
  calculateAffector(leftList, handOffset, leftTarget);
  calculateAffector(rightList, handOffset, rightTarget);
  Pose3D chestTarget(theKinematicChain.theLinks[KinematicChain::Torso].R, theKinematicChain.theLinks[KinematicChain::Torso].p);
  //leftTarget.rotateX(Math::fromDegrees(90));
  //leftTarget.translate(50,0,0);
  cout << "left target hand: " << leftTarget.translation <<" ("<< leftTarget.translation.abs() << ")"<< endl<<endl;
  //rightTarget.translate(100,0,0);
  //cout << "right target hand: " << rightTarget.translation <<" ("<< rightTarget.translation.abs() << ")"<< endl;

  int count = 0;
  while (count++ < 1)
  {
    for (unsigned int i = 0; i < JointData::numOfJoint; i++)
    {
      theJointData.position[i] = Math::random(theJointData.min[i], theJointData.max[i]);
    }
    theJointData.position[JointData::LHipYawPitch] = theJointData.position[JointData::RHipYawPitch];
    linkListForwardKinematics(leftList);
    linkListForwardKinematics(rightList);

    cout << " > MASK POS < " << endl;
    double err = gotoArms(chestTarget, leftTarget, rightTarget, handOffset, handOffset, MASK_POS);  //was POS

    Pose3D lf, rf;
    calculateAffector(leftList, handOffset, lf);
    calculateAffector(rightList, handOffset, rf);

    cout << "err (" << err << ") : \n";
//    for (int i = JointData::RShoulderRoll; i < JointData::RHipYawPitch; i++)
//    {
//      cout << JointData::getJointName((JointData::JointID)i) << "=" << Math::toDegrees(theJointData.position[i] /*theKinematicChain.theLinks[i].q*/  - refAng[i]) << ' ';
//    }
//    cout << endl<<endl;
    lf.rotation -= leftTarget.rotation;
    rf.rotation -= rightTarget.rotation;
    lf.translation -= leftTarget.translation;
    rf.translation -= rightTarget.translation;
    cout << "left hand translation: " <<  lf.translation << endl;
    cout << "left hand rotation: " <<  lf.rotation << endl << endl;
    cout << "right hand translation: " << rf.translation << endl;
    cout << "right hand rotation: " << rf.rotation << endl << endl;
  }
}

} // namespace kinematics
