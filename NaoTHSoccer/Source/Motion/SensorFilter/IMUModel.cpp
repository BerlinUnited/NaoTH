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
    ukf_rot.P        = Eigen::Matrix<double,3,3>::Identity(); // covariance matrix of current state

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

    ukf_rot.generateSigmaPoints();

    Eigen::Vector3d gyro;
    // gyro z axis seems to measure in opposite direction (turning left measures negative angular velocity, should be positive)
    gyro << getGyrometerData().data.x, getGyrometerData().data.y, -getGyrometerData().data.z;
    ukf_rot.predict(gyro,0.01); // getRobotInfo().getBasicTimeStepInSecond()

    // don't generate sigma points again because the process noise would be applied a second time
    // ukf.generateSigmaPoints();

    Eigen::Vector3d acceleration = Eigen::Vector3d(getAccelerometerData().data.x, getAccelerometerData().data.y, getAccelerometerData().data.z);

    IMU_RotationMeasurement z;
    z << acceleration.normalized();

    if(getMotionStatus().currentMotion == motion::walk){
        //TODO: mhmhmhmh...
        if(imuParameters.enableWhileWalking){
            ukf_rot.update(z, R_rotation_walk);
        }
    } else {
        ukf_rot.update(z, R_rotation);
    }

    IMU_AccMeasurementGlobal z_acc = ukf_rot.state.getRotationAsQuaternion()._transformVector(acceleration);
    double u_acc = 0;
    ukf_acc_global.generateSigmaPoints();
    ukf_acc_global.predict(u_acc, 0.01); // getRobotInfo().getBasicTimeStepInSecond()
    ukf_acc_global.update(z_acc, R_acc);

    writeIMUData();

    plots();

    lastFrameInfo = getFrameInfo();
}

void IMUModel::writeIMUData(){
    // raw sensor values
    getIMUData().rotational_velocity_sensor = getGyrometerData().data;
    getIMUData().acceleration_sensor = getAccelerometerData().data;

    // global position data
    getIMUData().location += getIMUData().velocity * 0.01; // is this a timestep?
    getIMUData().velocity += getIMUData().acceleration * 0.01; // is this a timestep? getRobotInfo().getBasicTimeStepInSecond()
    
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



    // from inertiasensorfilter
    //getIMUData().orientation = Vector2d(atan2( q.toRotationMatrix()(2,1), q.toRotationMatrix()(2,2)),
    //                                    atan2(-q.toRotationMatrix()(2,0), q.toRotationMatrix()(2,2)));

    getIMUData().orientation = Vector2d(atan2( rot[1].z, rot[2].z),
                                        atan2(-rot[0].z, rot[2].z));

    // only to enable transparent switching with InertiaSensorFilter
    //getInertialModel().orientation = getIMUData().orientation;
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
{   /* parameters for stand */
    // covariances of the acceleration filter
    ukf_acc_global.Q << imuParameters.stand.processNoiseAccQ00,                                0,                                0,
                                                       0, imuParameters.stand.processNoiseAccQ11,                                0,
                                                       0,                                0, imuParameters.stand.processNoiseAccQ22;

    // covariances of the rotation filter
    ukf_rot.Q << imuParameters.stand.processNoiseRotationQ00,                                     0,                               0,
                                                     0, imuParameters.stand.processNoiseRotationQ11,                                     0,
                                                     0,                                     0, imuParameters.stand.processNoiseRotationQ22;
    
    // measurement covariance matrix for the acceleration while stepping
    R_acc << imuParameters.stand.measurementNoiseAccR00, imuParameters.stand.measurementNoiseAccR01, imuParameters.stand.measurementNoiseAccR02,
             imuParameters.stand.measurementNoiseAccR01, imuParameters.stand.measurementNoiseAccR11, imuParameters.stand.measurementNoiseAccR12,
             imuParameters.stand.measurementNoiseAccR02, imuParameters.stand.measurementNoiseAccR12, imuParameters.stand.measurementNoiseAccR22;

    // measurement covariance matrix for the rotation while stepping
    R_rotation << imuParameters.stand.measurementNoiseRotationR00, imuParameters.stand.measurementNoiseRotationR01, imuParameters.stand.measurementNoiseRotationR02,
                  imuParameters.stand.measurementNoiseRotationR01, imuParameters.stand.measurementNoiseRotationR11, imuParameters.stand.measurementNoiseRotationR12,
                  imuParameters.stand.measurementNoiseRotationR02, imuParameters.stand.measurementNoiseRotationR12, imuParameters.stand.measurementNoiseRotationR22;

    /* parameters or walk */
    // covariances of the acceleration filter
    ukf_acc_global.Q << imuParameters.walk.processNoiseAccQ00,                                0,                                0,
                                                       0, imuParameters.walk.processNoiseAccQ11,                                0,
                                                       0,                                0, imuParameters.walk.processNoiseAccQ22;

    // covariances of the rotation filter
    ukf_rot.Q << imuParameters.walk.processNoiseRotationQ00,                                     0,                                     0,
                                                     0, imuParameters.walk.processNoiseRotationQ11,                                     0,
                                                     0,                                     0, imuParameters.walk.processNoiseRotationQ22;

    // measurement covariance matrix for the acceleration while stepping
    R_acc_walk << imuParameters.walk.measurementNoiseAccR00, imuParameters.walk.measurementNoiseAccR01, imuParameters.walk.measurementNoiseAccR02,
                  imuParameters.walk.measurementNoiseAccR01, imuParameters.walk.measurementNoiseAccR11, imuParameters.walk.measurementNoiseAccR12,
                  imuParameters.walk.measurementNoiseAccR02, imuParameters.walk.measurementNoiseAccR12, imuParameters.walk.measurementNoiseAccR22;

    // measurement covariance matrix for the rotation while stepping
    R_rotation_walk << imuParameters.walk.measurementNoiseRotationR00, imuParameters.walk.measurementNoiseRotationR01, imuParameters.walk.measurementNoiseRotationR02,
                       imuParameters.walk.measurementNoiseRotationR01, imuParameters.walk.measurementNoiseRotationR11, imuParameters.walk.measurementNoiseRotationR12,
                       imuParameters.walk.measurementNoiseRotationR02, imuParameters.walk.measurementNoiseRotationR12, imuParameters.walk.measurementNoiseRotationR22;
}

#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif
