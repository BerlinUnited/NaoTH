#include "KinematicChainProvider.h"
#include <Motion/MorphologyProcessor/ForwardKinematics.h>

KinematicChainProvider::KinematicChainProvider()
{
  udpateTime = getFrameInfo().getTime();

  // HACK
  if(!getKinematicChain().is_initialized())
  {
    getKinematicChain().init(getSensorJointData());
  }
}

void KinematicChainProvider::execute()
{
  double deltaTime = ( getFrameInfo().getTime() - udpateTime ) * 0.001;
  udpateTime = getFrameInfo().getTime();

  // calculate the KinematicChainSensor based on sensor data
  RotationMatrix orientation = getIMUData().orientation_rotvec; // get orientation (ignore any rotation around z axis) as rotation matrix

  // TODO: use sensor instead? getIMUData().acceleration_sensor - 9.81 * RotationMatrix(getIMUData().rotation).invert() * Vector3d(0,0,1);
  Vector3d local_acc_without_gravity = RotationMatrix(getIMUData().rotation).invert() * getIMUData().acceleration; // gravity adjusted global acceleration in body frame

  // calculate the kinematic chain
  Kinematics::ForwardKinematics::updateKinematicChainAll(
    orientation,
    local_acc_without_gravity,
    deltaTime,
    getKinematicChain(),
    theFSRPos);
}

KinematicChainProvider::~KinematicChainProvider()
{

}
