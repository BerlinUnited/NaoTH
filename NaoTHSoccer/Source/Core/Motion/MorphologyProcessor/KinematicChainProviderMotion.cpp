

#include "ForwardKinematics.h"

#include "KinematicChainProviderMotion.h"

KinematicChainProviderMotion::KinematicChainProviderMotion()
{
  // TODO: remove this dependency
  getKinematicChainSensor().init(getSensorJointData());
  getKinematicChainMotor().init(getMotorJointData());
}


void KinematicChainProviderMotion::execute()
{
  
  // calculate the KinematicChainSensor based on sensor data
  Kinematics::ForwardKinematics::calculateKinematicChainAll(
    //getAccelerometerData(),
    //getInertialSensorData.data,
    getInertialModel().orientation,
    getAccelerometerData().getAcceleration(),
    getKinematicChainSensor(),
    getFSRPositions().pos, // provides theFSRPos
    getRobotInfo().getBasicTimeStepInSecond());

  
  //
  Kinematics::ForwardKinematics::updateKinematicChainFrom(getKinematicChainMotor().theLinks);
  getKinematicChainMotor().updateCoM();
}//end execute
