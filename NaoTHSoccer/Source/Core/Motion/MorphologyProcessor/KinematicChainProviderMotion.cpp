

#include "ForwardKinematics.h"

#include "KinematicChainProviderMotion.h"
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
    getInertialModel().orientation,
    getAccelerometerData().getAcceleration(),
    getRobotInfo().getBasicTimeStepInSecond(),
    getKinematicChainSensor(),
    getFSRPositions().pos);

  
  //
  Kinematics::ForwardKinematics::updateKinematicChainFrom(getKinematicChainMotor().theLinks);
  getKinematicChainMotor().updateCoM();
}//end execute
