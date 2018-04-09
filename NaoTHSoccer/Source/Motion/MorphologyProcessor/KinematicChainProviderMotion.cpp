

#include "KinematicChainProviderMotion.h"

#include "ForwardKinematics.h"
#include <Tools/NaoInfo.h>

KinematicChainProviderMotion::KinematicChainProviderMotion()
{
  // TODO: remove this dependency
  getKinematicChainSensor().init(getSensorJointData());
  getKinematicChainMotor().init(getMotorJointData());
}


void KinematicChainProviderMotion::execute()
{
  // calculate the KinematicChainSensor based on sensor data
  RotationMatrix orientation = getIMUData().orientation_rotvec; // get orientation (ignore any rotation around z axis) as rotation matrix

  // TODO: use sensor instead? getIMUData().acceleration_sensor - 9.81 * RotationMatrix(getIMUData().rotation).invert() * Vector3d(0,0,1);
  Vector3d local_acc_without_gravity = RotationMatrix(getIMUData().rotation).invert() * getIMUData().acceleration; // gravity adjusted global acceleration in body frame

  Kinematics::ForwardKinematics::updateKinematicChainAll(
    orientation,
    local_acc_without_gravity,
    getRobotInfo().getBasicTimeStepInSecond(),
    getKinematicChainSensor(),
    getFSRPositions().pos);
  
  //
  Kinematics::ForwardKinematics::updateKinematicChainFrom(getKinematicChainMotor().theLinks);
  getKinematicChainMotor().updateCoM();
}//end execute
