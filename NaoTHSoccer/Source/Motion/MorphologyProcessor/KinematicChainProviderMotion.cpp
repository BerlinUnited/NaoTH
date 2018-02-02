

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
  Kinematics::ForwardKinematics::updateKinematicChainAll(
    getIMUData().orientation_rotvec,
    RotationMatrix(getIMUData().rotation).invert() * getIMUData().acceleration, // gravitiy adjusted global acceleration in body frame
    getRobotInfo().getBasicTimeStepInSecond(),
    getKinematicChainSensor(),
    getFSRPositions().pos);
  
  //
  Kinematics::ForwardKinematics::updateKinematicChainFrom(getKinematicChainMotor().theLinks);
  getKinematicChainMotor().updateCoM();
}//end execute
