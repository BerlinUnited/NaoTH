

#include "KinematicChainProviderMotion.h"

#include "ForwardKinematics.h"
#include <Tools/NaoInfo.h>

KinematicChainProviderMotion::KinematicChainProviderMotion()
{
  // TODO: remove this dependency
  getKinematicChainSensor().init(getSensorJointData());
  getKinematicChainMotor().init(getMotorJointData());

  getDebugParameterList().add(&parameter);
}


void KinematicChainProviderMotion::execute()
{
    RotationMatrix orientation;
    Vector3d local_acc_without_gravity;

    if(parameter.useIMUData){
        // calculate the KinematicChainSensor based on sensor data
        orientation = getIMUData().orientation_rotvec; // get orientation (ignore any rotation around z axis) as rotation matrix

        // TODO: use sensor instead? getIMUData().acceleration_sensor - 9.81 * RotationMatrix(getIMUData().rotation).invert() * Vector3d(0,0,1);
        local_acc_without_gravity = RotationMatrix(getIMUData().rotation).invert() * getIMUData().acceleration; // gravity adjusted global acceleration in body frame
    } else {
        // almost old behavior for testing...
        orientation = RotationMatrix::getRotationY(getInertialModel().orientation.y)*RotationMatrix::getRotationX(getInertialModel().orientation.x);
        local_acc_without_gravity = getAccelerometerData().getAcceleration();
    }

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
