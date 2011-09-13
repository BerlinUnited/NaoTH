/**
* @file ForwardKinematics.h
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* Declaration of the class ForwardKinematics
*/

#ifndef _FORWARDKINEMATICS_H
#define  _FORWARDKINEMATICS_H

#include <Tools/Math/Matrix.h>
#include <Tools/Math/Vector3.h>

#include <Representations/Infrastructure/InertialSensorData.h>
#include <Representations/Infrastructure/AccelerometerData.h>

#include "Representations/Modeling/KinematicChain.h"



namespace Kinematics {

class ForwardKinematics
{
public:

  /** */
  static void calculateKinematicChainAll(
    const naoth::AccelerometerData& theAccelerometerData,
    const naoth::InertialSensorData& theInertialSensorData,
    KinematicChain& theKinematicChain,
    Vector3<double>* theFSRPos,
    const double deltaTime);

  /** update the position, velocity and acceleration of links according to joint angles data directly */
  static void forwardAllKinematics(Link* theLink);

  /** */
  static void calcCoMs(Link* l);

  /** */
  static void updateKinematicChainFrom(Link* l);


private:
  ForwardKinematics() {} // don't create an instance

  /** */
  static void calcChestRotation(Link* theLinks, const naoth::InertialSensorData& theInertialSensorData);

  /** */
  static void calcChestAll(Link* theLinks,
          const naoth::AccelerometerData& theAccelerometerData,
          const naoth::InertialSensorData& theInertialSensorData,
          const double deltaTime);
  
  /** */
  static double calcChestHeight(const Vector3<double>* fsrPos);

  /** */
  static void calcLinkUp(Link* l);

  /** */
  static void adjustKinematicChain(Link* l, Vector3<double>* fsrPos, double chestHeight);
  
  /** this function should be called after calcLink() */
  static void calcFSRPos(const Link* theLink, Vector3<double>* theFSRPos);

  /* update the position from leaf to root 
   * @return the root
   */
  static Link* updateToRoot(Link* &l);

  /* update all children, except the given one
   * @parm theLink, all the children of theLink will be updated
   * @parm ept, this child will not be updated
   */
  static void forwardAllKinematicsExcept(Link* theLink, Link* ept);
};

} // namespace Kinematics
#endif  /* _FORWARDKINEMATICS_H */

