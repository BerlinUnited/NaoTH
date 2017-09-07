#include "IMUModel.h"

//TODO: remove pragma, problem with eigens optimization stuff "because conversion sequence for the argument is better"
#if defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wconversion"
#endif

IMUModel::IMUModel():
    integrated(1,0,0,0)
{
    DEBUG_REQUEST_REGISTER("IMUModel:reset_filter", "reset filter", false);
    DEBUG_REQUEST_REGISTER("IMUModel:reloadParameters", "", false);
    DEBUG_REQUEST_REGISTER("IMUModel:enableFilterWhileWalking", "enables filter update while walking", false);

    ukf_acc_global.P = Eigen::Matrix<double,3,3>::Identity(); // covariance matrix of current state
    ukf_rot.P        = Eigen::Matrix<double,6,6>::Identity(); // covariance matrix of current state

    getDebugParameterList().add(&imuParameters);

    reloadParameters();
}

IMUModel::~IMUModel()
{
    getDebugParameterList().remove(&imuParameters);
}

void IMUModel::execute(){
    DEBUG_REQUEST("IMUModel:reset_filter",
                  ukf_rot.reset();
                  ukf_acc_global.reset();
                  integrated=Eigen::Quaterniond(1,0,0,0);
    );

    DEBUG_REQUEST("IMUModel:reloadParameters",
        reloadParameters();
    );

    /* handle ukf filter for rotation */
    if(getMotionStatus().currentMotion == motion::walk){
        ukf_rot.Q = Q_rotation_walk;
    } else {
        ukf_rot.Q = Q_rotation;
    }

    ukf_rot.generateSigmaPoints();

    Eigen::Vector3d u_rot(0,0,0); // TODO: use generated angular acceleration as control vector?
    ukf_rot.predict(u_rot, getRobotInfo().getBasicTimeStepInSecond());

    // don't generate sigma points again because the process noise would be applied a second time
    // ukf.generateSigmaPoints();

    Eigen::Vector3d gyro;
    // gyro z axis seems to measure in opposite direction (turning left measures negative angular velocity, should be positive)
    gyro << getGyrometerData().data.x, getGyrometerData().data.y, -getGyrometerData().data.z;
    Eigen::Vector3d acceleration = Eigen::Vector3d(getAccelerometerData().data.x, getAccelerometerData().data.y, getAccelerometerData().data.z);

    IMU_RotationMeasurement z;
    z << acceleration.normalized(), gyro;

    if(getMotionStatus().currentMotion == motion::walk){
        if(imuParameters.enableWhileWalking){
            ukf_rot.update(z, R_rotation_walk);
        }
    } else {
        ukf_rot.update(z, R_rotation);
    }
    /* rotation ukf end */

    /* handle ukf filter for global acceleration */
    // transform acceleration measurement into global reference frame
    IMU_AccMeasurementGlobal z_acc = ukf_rot.state.getRotationAsQuaternion()._transformVector(acceleration);

    if(getMotionStatus().currentMotion == motion::walk){
        ukf_acc_global.Q = Q_acc_walk;
    } else {
        ukf_acc_global.Q = Q_acc;
    }

    ukf_acc_global.generateSigmaPoints();

    Eigen::Vector3d u_acc(0,0,0); // TODO: use generated jerk as control vector?
    ukf_acc_global.predict(u_acc, getRobotInfo().getBasicTimeStepInSecond());

    if(getMotionStatus().currentMotion == motion::walk){
        if(imuParameters.enableWhileWalking){
            ukf_acc_global.update(z_acc, R_acc_walk);
        }
    } else {
        ukf_acc_global.update(z_acc, R_acc);
    }
    /* acc ukf end */

    writeIMUData();

    plots();

    lastFrameInfo = getFrameInfo();
}

void IMUModel::writeIMUData(){
    // raw sensor values
    getIMUData().rotational_velocity_sensor = getGyrometerData().data;
    getIMUData().acceleration_sensor = getAccelerometerData().data;

    // global position data
    getIMUData().location += getIMUData().velocity * getRobotInfo().getBasicTimeStepInSecond();
    getIMUData().velocity += getIMUData().acceleration * getRobotInfo().getBasicTimeStepInSecond();
    
    getIMUData().acceleration.x = ukf_acc_global.state.acceleration()(0,0);
    getIMUData().acceleration.y = ukf_acc_global.state.acceleration()(1,0);
    getIMUData().acceleration.z = ukf_acc_global.state.acceleration()(2,0) + 9.81; //Math::g

    // convert to framework compliant x,y,z angles
    Eigen::Vector3d temp2 = ukf_rot.state.rotation();
    Vector3d rot_vec(temp2(0),temp2(1),temp2(2));
    RotationMatrix rot(rot_vec);
    getIMUData().rotation.x = rot.getXAngle();
    getIMUData().rotation.y = rot.getYAngle();
    getIMUData().rotation.z = rot.getZAngle();

    getIMUData().orientation = Vector2d(atan2( rot[1].z, rot[2].z),
                                        atan2(-rot[0].z, rot[2].z));

    // only to enable transparent switching with InertiaSensorFilter
    getInertialModel().orientation = getIMUData().orientation;
}

void IMUModel::plots(){
    // --- for testing integration
        Eigen::Matrix3d rot_vel_mat;
        // getGyrometerData().data.z is inverted!
        rot_vel_mat << 1                             , getGyrometerData().data.z*0.01,  getGyrometerData().data.y*0.01,
                      -getGyrometerData().data.z*0.01,                              1, -getGyrometerData().data.x*0.01,
                      -getGyrometerData().data.y*0.01, getGyrometerData().data.x*0.01,                               1;

        // continue rotation assuming constant velocity
        integrated = integrated * Eigen::Quaterniond(rot_vel_mat);

        Vector3d translation(0,250,250);
        LINE_3D(ColorClasses::red, translation, translation + quaternionToVector3D(integrated) * 100.0);
        LINE_3D(ColorClasses::blue, translation, translation + quaternionToVector3D(ukf_rot.state.getRotationAsQuaternion()) * 100.0);

        PLOT("IMUModel:Error:relative_to_pure_integration[°]", Math::toDegrees(Eigen::AngleAxisd(integrated*ukf_rot.state.getRotationAsQuaternion().inverse()).angle()));
    // --- end

    Eigen::Vector3d temp2 = ukf_rot.state.rotation();
    Vector3d rot_vec(temp2(0),temp2(1),temp2(2));
    RotationMatrix rot(rot_vec);
    Pose3D pose(rot);
    pose.translation = Vector3d(0,0,250);

    // plot gravity
    Eigen::Vector3d acceleration;
    acceleration << getAccelerometerData().data.x, getAccelerometerData().data.y, getAccelerometerData().data.z;
    Eigen::Vector3d temp3 = ukf_rot.state.getRotationAsQuaternion()._transformVector(acceleration);
    Vector3d gravity(6*temp3(0),6*temp3(1),6*temp3(2));

    COLOR_CUBE(30, pose);
    LINE_3D(ColorClasses::black, pose.translation, pose.translation + gravity*10.0);

    PLOT("IMUModel:Measurement:rotational_velocity:x", getGyrometerData().data.x);
    PLOT("IMUModel:Measurement:rotational_velocity:y", getGyrometerData().data.y);
    PLOT("IMUModel:Measurement:rotational_velocity:z", getGyrometerData().data.z);
}

void IMUModel::reloadParameters()
{   /* parameters for the acceleration filter */
    // process noise
    Q_acc << imuParameters.acceleration.stand.processNoiseQ00,                                0,                                0,
                                                       0, imuParameters.acceleration.stand.processNoiseQ11,                                0,
                                                       0,                                0, imuParameters.acceleration.stand.processNoiseQ22;

    Q_acc_walk << imuParameters.acceleration.walk.processNoiseQ00,                                0,                                0,
                                                       0, imuParameters.acceleration.walk.processNoiseQ11,                                0,
                                                       0,                                0, imuParameters.acceleration.walk.processNoiseQ22;
    
    // measurement covariance matrix
    R_acc << imuParameters.acceleration.stand.measurementNoiseR00, imuParameters.acceleration.stand.measurementNoiseR01, imuParameters.acceleration.stand.measurementNoiseR02,
             imuParameters.acceleration.stand.measurementNoiseR01, imuParameters.acceleration.stand.measurementNoiseR11, imuParameters.acceleration.stand.measurementNoiseR12,
             imuParameters.acceleration.stand.measurementNoiseR02, imuParameters.acceleration.stand.measurementNoiseR12, imuParameters.acceleration.stand.measurementNoiseR22;

    R_acc_walk << imuParameters.acceleration.walk.measurementNoiseR00, imuParameters.acceleration.walk.measurementNoiseR01, imuParameters.acceleration.walk.measurementNoiseR02,
                  imuParameters.acceleration.walk.measurementNoiseR01, imuParameters.acceleration.walk.measurementNoiseR11, imuParameters.acceleration.walk.measurementNoiseR12,
                  imuParameters.acceleration.walk.measurementNoiseR02, imuParameters.acceleration.walk.measurementNoiseR12, imuParameters.acceleration.walk.measurementNoiseR22;

    /* parameters for the rotation filter */
    // process noise
    Q_rotation << Eigen::Matrix<double,6,6>::Zero();
    Q_rotation(0,0) = imuParameters.rotation.stand.processNoiseQ00;
    Q_rotation(1,1) = imuParameters.rotation.stand.processNoiseQ11;
    Q_rotation(2,2) = imuParameters.rotation.stand.processNoiseQ22;
    Q_rotation(3,3) = imuParameters.rotation.stand.processNoiseQ33;
    Q_rotation(4,4) = imuParameters.rotation.stand.processNoiseQ44;
    Q_rotation(5,5) = imuParameters.rotation.stand.processNoiseQ55;

    Q_rotation_walk << Eigen::Matrix<double,6,6>::Zero();
    Q_rotation_walk(0,0) = imuParameters.rotation.walk.processNoiseQ00;
    Q_rotation_walk(1,1) = imuParameters.rotation.walk.processNoiseQ11;
    Q_rotation_walk(2,2) = imuParameters.rotation.walk.processNoiseQ22;
    Q_rotation_walk(3,3) = imuParameters.rotation.walk.processNoiseQ33;
    Q_rotation_walk(4,4) = imuParameters.rotation.walk.processNoiseQ44;
    Q_rotation_walk(5,5) = imuParameters.rotation.walk.processNoiseQ55;

    // measurement covariance matrix
    R_rotation << Eigen::Matrix<double,6,6>::Zero();
    R_rotation(0,0) = imuParameters.rotation.stand.measurementNoiseR00;
    R_rotation(1,1) = imuParameters.rotation.stand.measurementNoiseR11;
    R_rotation(2,2) = imuParameters.rotation.stand.measurementNoiseR22;
    R_rotation(3,3) = imuParameters.rotation.stand.measurementNoiseR33;
    R_rotation(4,4) = imuParameters.rotation.stand.measurementNoiseR44;
    R_rotation(5,5) = imuParameters.rotation.stand.measurementNoiseR55;

    R_rotation_walk << Eigen::Matrix<double,6,6>::Zero();
    R_rotation_walk(0,0) = imuParameters.rotation.walk.measurementNoiseR00;
    R_rotation_walk(1,1) = imuParameters.rotation.walk.measurementNoiseR11;
    R_rotation_walk(2,2) = imuParameters.rotation.walk.measurementNoiseR22;
    R_rotation_walk(3,3) = imuParameters.rotation.walk.measurementNoiseR33;
    R_rotation_walk(4,4) = imuParameters.rotation.walk.measurementNoiseR44;
    R_rotation_walk(5,5) = imuParameters.rotation.walk.measurementNoiseR55;
}

#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif
