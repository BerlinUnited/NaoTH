#include "IMUModel.h"

//TODO: remove pragma, problem with eigens optimization stuff "because conversion sequence for the argument is better"
#pragma GCC diagnostic ignored "-Wconversion"

IMUModel::IMUModel():
    integrated(1,0,0,0)
{
    DEBUG_REQUEST_REGISTER("IMUModel:reset_filter", "reset filter", false);
    DEBUG_REQUEST_REGISTER("IMUModel:use_both", "uses accelerometer and gyrometer to estimate the orientation", false);
    DEBUG_REQUEST_REGISTER("IMUModel:use_only_gyro", "uses only the gyrometer to estimate the orientation (integration)", false);

    // Parameter Related Debug Requests
    DEBUG_REQUEST_REGISTER("IMUModel:reloadParameters", "reloads the filter parameters from the imuParameter object", false);

    ukf_acc_global.P = Eigen::Matrix<double,3,3>::Identity();        // covariance matrix of current state
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

    STOPWATCH_START("Motion:IMU:SigmaPointsRot");
    ukf_rot.generateSigmaPoints();
    STOPWATCH_STOP("Motion:IMU:SigmaPointsRot");

    Eigen::Vector3d gyro;
    gyro << getGyrometerData().data.x, getGyrometerData().data.y, -getGyrometerData().data.z;

    STOPWATCH_START("Motion:IMU:predictRot");
    ukf_rot.predict(gyro,0.01);
    STOPWATCH_STOP("Motion:IMU:predictRot");

    // don't generate sigma points again because the process noise would be applied a second time
    // ukf.generateSigmaPoints();

    Eigen::Vector3d acceleration = Eigen::Vector3d(getAccelerometerData().data.x, getAccelerometerData().data.y, getAccelerometerData().data.z);

    // --- for debug purpose only
//    bool use_gyro_only = false;
//    bool use_both = false;

//    DEBUG_REQUEST("IMUModel:use_both", use_both = true;);
//    DEBUG_REQUEST("IMUModel:use_only_gyro", use_gyro_only = true;);
    // --- end

    // TODO: empirical determination or better condition
    //if((fabs(acceleration.norm() - 9.81) < 0.5 || use_both) && !use_gyro_only ){ // use acceleration
        Eigen::Quaterniond rotation_acc_to_gravity;
        rotation_acc_to_gravity.setFromTwoVectors(acceleration,Eigen::Vector3d(0,0,-1));

        IMU_RotationMeasurement z;
        // gyro z axis seems to measure in opposite direction (turning left measures negative angular velocity, should be positive)
        z << quaternionToRotationVector(rotation_acc_to_gravity);//, getGyrometerData().data.x, getGyrometerData().data.y, -getGyrometerData().data.z;

        STOPWATCH_START("Motion:IMU:upadteRot");
        ukf_rot.update(z, R_rotation);
        STOPWATCH_STOP("Motion:IMU:upadteRot");

        updated_by_both = true;
    //}
        /* else { // use only gyro meter
        IMU_RotVelMeasurement z;
        // gyro z axis seems to measure in opposite direction (turning left measures negative angular velocity, should be positive)
        z << getGyrometerData().data.x, getGyrometerData().data.y, -getGyrometerData().data.z;

        //Eigen::Matrixd<3,3> R = R_rotation.block<3,3>(3,3);
        ukf_rot.update(z, R_rotation.block<3,3>(3,3));

        updated_by_both = false;
    }*/

    IMU_AccMeasurementGlobal z_acc = ukf_rot.state.getRotationAsQuaternion()._transformVector(acceleration);
    STOPWATCH_START("Motion:IMU:SigmaPointsAcc");
    ukf_acc_global.generateSigmaPoints();
    STOPWATCH_STOP("Motion:IMU:SigmaPointsAcc");
    double u = 0;

    STOPWATCH_START("Motion:IMU:predictAcc");
    ukf_acc_global.predict(u, 0.01);
    STOPWATCH_STOP("Motion:IMU:predictAcc");

    STOPWATCH_START("Motion:IMU:upadteAcc");
    ukf_acc_global.update(z_acc, R_acc);
    STOPWATCH_STOP("Motion:IMU:upadteAcc");


    STOPWATCH_START("Motion:IMU:writeData");
    writeIMUData();
    STOPWATCH_STOP("Motion:IMU:writeData");

    STOPWATCH_START("Motion:IMU:plotting");
    plots();
    STOPWATCH_STOP("Motion:IMU:plotting");

    lastFrameInfo = getFrameInfo();
}

void IMUModel::writeIMUData(){
    // raw sensor values
    getIMUData().rotational_velocity_sensor = getGyrometerData().data;
    getIMUData().acceleration_sensor = getAccelerometerData().data;

    // global position data
    getIMUData().location += getIMUData().velocity * 0.01;

    getIMUData().velocity += getIMUData().acceleration * 0.01;

    getIMUData().acceleration.x = ukf_acc_global.state.acceleration()(0,0);
    getIMUData().acceleration.y = ukf_acc_global.state.acceleration()(1,0);
    getIMUData().acceleration.z = ukf_acc_global.state.acceleration()(2,0) + 9.81;

    Eigen::Quaterniond q;
    q = ukf_rot.state.getRotationAsQuaternion();
    Eigen::Vector3d angles = q.toRotationMatrix().eulerAngles(0,1,2);
    getIMUData().rotation.x = angles(0);
    getIMUData().rotation.y = angles(1);
    getIMUData().rotation.z = angles(2);

//    getIMUData().rotational_velocity.x = ukf_rot.state.rotational_velocity()(0,0);
//    getIMUData().rotational_velocity.y = ukf_rot.state.rotational_velocity()(1,0);
//    getIMUData().rotational_velocity.z = ukf_rot.state.rotational_velocity()(2,0);

    // from inertiasensorfilter
    getIMUData().orientation = Vector2d(atan2(q.toRotationMatrix()(2,1),  q.toRotationMatrix()(2,2)),
                                        atan2(-q.toRotationMatrix()(2,0), q.toRotationMatrix()(2,2)));

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
        integrated = integrated * Eigen::Quaterniond(rot_vel_mat);;

        Vector3d translation(0,250,250);
        LINE_3D(ColorClasses::red, translation, translation + quaternionToVector3D(integrated) * 100.0);
        LINE_3D(ColorClasses::blue, translation, translation + quaternionToVector3D(ukf_rot.state.getRotationAsQuaternion()) * 100.0);

        PLOT("IMUModel:Error:relative_to_pure_integration[°]", Math::toDegrees(Eigen::AngleAxisd(integrated*ukf_rot.state.getRotationAsQuaternion().inverse()).angle()));
    // --- end

//    PLOT("IMUModel:State:acceleration:x", ukf.state.acceleration()(0,0));
//    PLOT("IMUModel:State:acceleration:y", ukf.state.acceleration()(1,0));
//    PLOT("IMUModel:State:acceleration:z", ukf.state.acceleration()(2,0));

    PLOT("IMUModel:Measurement:acceleration:x", getAccelerometerData().data.x);
    PLOT("IMUModel:Measurement:acceleration:y", getAccelerometerData().data.y);
    PLOT("IMUModel:Measurement:acceleration:z", getAccelerometerData().data.z);

    Eigen::Quaterniond q;
    q = ukf_rot.state.getRotationAsQuaternion();
    Eigen::Vector3d angles = q.toRotationMatrix().eulerAngles(0,1,2);
    PLOT("IMUModel:State:rotation:x", angles(0));
    PLOT("IMUModel:State:rotation:y", angles(1));
    PLOT("IMUModel:State:rotation:z", angles(2));

    Eigen::AngleAxisd temp(q);
    Eigen::Vector3d temp2(temp.angle()*temp.axis());

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

    PLOT("IMUModel:updated_by_both", updated_by_both);
}

void IMUModel::reloadParameters()
{
    // covariances of the acceleration filter
    ukf_acc_global.Q << imuParameters.processNoiseAccQ00,                                0,                                0,
                                                       0, imuParameters.processNoiseAccQ11,                                0,
                                                       0,                                0, imuParameters.processNoiseAccQ22;

    // covariances of the rotation filter
    ukf_rot.Q << imuParameters.processNoiseRotationQ00,                                     0,                                     0,
                                                     0, imuParameters.processNoiseRotationQ11,                                     0,
                                                     0,                                     0, imuParameters.processNoiseRotationQ22;

    // measurement covariance matrix for the acceleration while stepping
    R_acc << imuParameters.measurementNoiseAccR00, imuParameters.measurementNoiseAccR01, imuParameters.measurementNoiseAccR02,
             imuParameters.measurementNoiseAccR01, imuParameters.measurementNoiseAccR11, imuParameters.measurementNoiseAccR12,
             imuParameters.measurementNoiseAccR02, imuParameters.measurementNoiseAccR12, imuParameters.measurementNoiseAccR22;

    // measurement covariance matrix for the rotation while stepping
    R_rotation << imuParameters.measurementNoiseAngleR00, imuParameters.measurementNoiseAngleR01, imuParameters.measurementNoiseAngleR02,
                  imuParameters.measurementNoiseAngleR01, imuParameters.measurementNoiseAngleR11, imuParameters.measurementNoiseAngleR12,
                  imuParameters.measurementNoiseAngleR02, imuParameters.measurementNoiseAngleR12, imuParameters.measurementNoiseAngleR22;
}

#pragma GCC diagnostic pop
