/**
* @file InverseKinematics.h
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* @author <a href="mailto:welter@informatik.hu-berlin.de">Welter, Oliver</a>
* Implementation of Inverse Kinematics
*/

#ifndef _InverseKinematics_H_
#define _InverseKinematics_H_

#include <Tools/Math/Matrix_nxn.h>
#include <Tools/Math/Pose3D.h>
#include <list>
#include "Representations/Modeling/KinematicChain.h"

namespace Kinematics {

class InverseKinematics
{
public:
  /* In some cases when the chain is not redundant,
   * a 6 dimension Task is impossible to achieve.
   * However, in most cases only one position or orientation needs to be controlled
   * and the user doesn't care so much about the other dimension.
   * So, some Mask are defined to indicates which dimensions should be controlled.
   */
  enum Mask
  {
    MASK_X = 1,
    MASK_Y = 2,
    MASK_Z = 4,
    MASK_WX = 8,
    MASK_WY = 16,
    MASK_WZ = 32,
    MASK_POS = MASK_X + MASK_Y + MASK_Z,
    MASK_ROT = MASK_WX + MASK_WY + MASK_WZ,
    MASK_ALL = MASK_POS + MASK_ROT
  };

  enum Calculus
  {
    CCD,
    JacTrans,
    JacInv
  };

public:
  InverseKinematics(bool autoInit=true);
  ~InverseKinematics();

  /** */
  void init();


public:
  /** the results of the calculations are stored here */
  KinematicChain theKinematicChain;
  naoth::JointData theJointData;


private:
  /** lock some joints (used by the numeric methods) */
  bool lock[naoth::JointData::numOfJoint];

  /** the threshold to finish algorithm (used by the most methods) */
  const double maxError;


  /** (needed by gotoTargetJacobianInversion and gotoTargetJacobianTranspose) */
  void calcJacobi(const std::list<Link*>& linkList, const Vector3<double>& affector, Matrix_nxn<double, 6>& J) const;

  /** (needed by gotoTargetJacobianInversion and gotoTargetJacobianTranspose) */
  bool updateJoints(const std::list<Link*>& linkList,const Vector_n<double,6>& dq) const;


  /** solve IK by Jacobian inversion algorithm
   * @param linkList the link list of leg
   * @param target the target pose of foot
   * @param offset the offset of affector
   * @param mask which dimensions should be controlled
   * @return error
   */
  double gotoTargetJacobianInversion(const std::list<Link*>& linkList, const Pose3D& target, const Vector3<double>& offset, Mask mask) const;

  /** solve IK by Jacobian transpose algorithm
   * @param linkList the link list of leg
   * @param target the target pose of foot
   * @param offset the offset of affector
   * @param mask which dimensions should be controlled
   * @return error
   */
  double gotoTargetJacobianTranspose(const std::list<Link*>& linkList, const Pose3D& target, const Vector3<double>& offset, Mask mask) const;

  /** solve IK by cyclic-coordinate descent algorithm
   * @note only hip joinsts is calculated, other joints should be calculated by function analyzeLeg
   * @param linkList the link list of leg
   * @param target the target pose of foot
   * @param offset the offset of affector
   * @param mask which dimensions should be controlled
   * @return error
   */
  double gotoTargetCCD(const std::list<Link*>& linkList, const Pose3D& target, const Vector3<double>& offset, Mask mask) const;


  /** solve IK numricaly: choose between different algorithms
   * @param linkList the link list of leg
   * @param target the target pose of foot
   * @param offset the offset of affector
   * @param mask which dimensions should be controlled
   * @return error
   */
  double gotoTarget(const std::list<Link*>& linkList, const Pose3D& target, const Vector3<double>& offset, Mask mask, Calculus method = CCD) const
  {
    switch(method)
    {
      case JacInv:
        return gotoTargetJacobianInversion(linkList, target, offset, mask);
      case JacTrans:
        return gotoTargetJacobianTranspose(linkList,target, offset, mask);
      case CCD: // CCD is default method
      default:
        return gotoTargetCCD(linkList, target, offset, mask);
    }//end switch
  }//end gotoTarget



  /* 
  * solve IK analyticaly for KneePitch, AnklePitch, AnkleRoll
  * @note the target will be modified when it can not be reached
  * the result is stored to theJointData
  */
  double calculateLowerNaoLegJointsAnalyticaly(bool leftLeg, Pose3D& target);

  /* 
  * solve IK analyticaly for 6 joints of the Nao's Leg
  * @note the target will be modified when it can not be reached
  * the result is stored to theJointData
  */
  double calculateNaoLegJointsAnalyticaly(bool leftLeg, Pose3D& target);



public:

  //@return error
  double gotoLeg(bool leftLeg, Pose3D& target, const Vector3<double>& offset, Mask mask = MASK_ALL);

  // private?
  double gotoLegAnalytical(bool leftLeg, Pose3D& target, const Vector3<double>& offset);

  // TODO: make targetLeftFoot const Pose3D&
  double gotoLegs(
    const Pose3D& targetChest, 
    Pose3D targetLeftFoot, 
    Pose3D targetRightFoot,
    const Vector3<double>& leftOffset, 
    const Vector3<double>& rightOffset,
    Mask leftFootMask = MASK_ALL, 
    Mask rightFootMask = MASK_ALL);
  

  double gotoArm(bool leftArm, const Pose3D& target, const Vector3<double>& offset, Mask mask);

  /** calculate inverse kinematic for robot arms */
  double gotoArms(
    const Pose3D& targetChest, 
    const Pose3D& targetLeftHand, 
    const Pose3D& targetRightHand,
    const Vector3<double>& leftOffset, 
    const Vector3<double>& rightOffset,
    Mask leftHandMask = MASK_ALL, 
    Mask rightHandMask = MASK_ALL);



public:
  /** 
    get the route from the link given by the id to the root node
    (uses theKinematicChain)
  */
  // private?
  void getRoute(KinematicChain::LinkID actuatorID, std::list<Link*>& linkList);


  /**
   * some tests
   */
  void test();
  void testArms();


// Tools
public:
// Kinematic-Numeric Tools
  /** calculate the translation and rotation deviation between current and target poses */
  static void calcError(const Pose3D& current, const Pose3D& target, Vector_n<double,6>& result, Mask mask);

  /** calcualte the quantization of error */
  static double errlen(const Vector_n<double,6>& e);

// Kinematic-Link Tools
  /** normalizes the angles of a route */
  static void normalizeLinkAngles(const std::list<Link*>& linkList);

  /** get the route from the given link to the root node */
  static void getRoute(Link *actuatorLink, std::list<Link*>& linkList);

  /**  */
  static void linkListForwardKinematics(const std::list<Link*>& linkList);

  /**  */
  static void linkListForwardKinematics(const std::list<Link*>& linkList, Link* beginLink);

  /**  */
  static void calculateAffector(const std::list<Link*>& linkList, const Vector3<double>& offset, Pose3D& affector);

  /**  */
  static void calculateAffector(const std::list<Link*>& linkList, Link* beginLink, const Vector3<double>& offset, Pose3D& affector);

};

} // namespace Kinematics
#endif  /* _InverseKinematics_H_ */
