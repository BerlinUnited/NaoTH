/**
* @file ForwardKinematics.h
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* Declaration of the class ForwardKinematics
*/

#ifndef _ForwardKinematics_H_
#define _ForwardKinematics_H_

#include <Tools/Math/RotationMatrix.h>
#include <Tools/Math/Vector3.h>

#include <Representations/Infrastructure/AccelerometerData.h>

#include "Representations/Modeling/KinematicChain.h"



namespace Kinematics 
{

class ForwardKinematics
{
public:

  /** Update the whole kinematic chain based on the provided data. */
  static void updateKinematicChainAll(
    const Vector2<double>& theBodyRotation,
    const Vector3<double>& theBodyAcceleration,
    const double deltaTime,
    KinematicChain& theKinematicChain,
    Vector3d* theFSRPos);

  /** 
  * update the position, velocity and acceleration of the links according to joint angles data
  */
  static void forwardAllKinematics(Link* theLink);
  static void forwardAllKinematics(Link& theLink);
  static void forwardAllKinematics(KinematicChain& theKinematicChain, KinematicChain::LinkID start);

  /** calculate the CoM for every link and the total CoM for the kinematic chain*/
  static void updateCoM(Link* l);
  static void updateCoM(KinematicChain& theKinematicChain);

  /** */
  static void updateKinematicChainFrom(Link* l);
  static void updateKinematicChainFrom(KinematicChain& theKinematicChain, KinematicChain::LinkID start);

  /** calculate rotation based on foot-torso transformation */
  static RotationMatrix calcChestFeetRotation(const KinematicChain& theKinematicChain);

  /** translate the whole kinematic chain by vector t */
  static void translate(KinematicChain& theKinematicChain, const Vector3d& t);
  
  /**
  * Set the chest position to (0,0,0) and its rotation to the provided theBodyRotation.
  * Update the speed and the acceleration.
  * NOTE: only the link 'Torso' is modified.
  */
  static void updateChest(
          KinematicChain& kinematicChain,
          const Vector2d& theBodyRotation,
          const Vector3d& theBodyAcceleration,
          const double deltaTime);

private:
  ForwardKinematics() {} // don't create an instance

  /**
  */
  static void setRotation(Link& link, const Vector2d& rotation);

  
  /** */
  static double estimateChestHeight(const Vector3d* fsrPos);


  /** */
  static void calcLinkUp(Link* l);

  /* update the position from leaf to root 
   * @return the root
   */
  static Link* updateToRoot(Link* l);

  /* update all children, except the given one
   * @parm theLink, all the children of theLink will be updated
   * @parm ept, this child will not be updated
   */
  static void forwardAllKinematicsExcept(Link* theLink, Link* ept);


  static void updateFSRPos(const KinematicChain& kinematicChain, Vector3d* theFSRPos);
  static int getLowestFSR(const Vector3d* theFSRPos);
};

} // namespace Kinematics
#endif  /* _ForwardKinematics_H_ */

