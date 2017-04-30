#include "IMUModel.h"

//TODO: remove pragma, problem with eigens optimization stuff "because conversion sequence for the argument is better"
#pragma GCC diagnostic ignored "-Wconversion"

IMUModel::IMUModel(){
    DEBUG_REQUEST_REGISTER("IMUModel:reset_filter", "reset filter", false);
    DEBUG_REQUEST_REGISTER("IMUModel:use_both", "uses accelerometer and gyrometer to estimate the orientation", false);
    DEBUG_REQUEST_REGISTER("IMUModel:use_only_gyro", "uses only the gyrometer to estimate the orientation (integration)", false);

    // covariances of the acceleration filter
    ukf_acc_global.P = Eigen::Matrix<double,3,3>::Identity();        // covariance matrix of current state
    ukf_acc_global.Q = 0.01 * Eigen::Matrix<double,3,3>::Identity(); // covariance matrix of process noise

    R_acc << 5.074939351879890342e-04, -1.561730283237946278e-05,  1.012849085655689321e-04,
            -1.561730283237946278e-05,  2.570436087068024501e-04, -4.159091012580820026e-05,
             1.012849085655689321e-04, -4.159091012580820026e-05,  4.727921819788054878e-04; // covariance matrix of measurement noise

    // covariances of the rotation filter
    ukf_rot.P = Eigen::Matrix<double,3,3>::Identity(); // covariance matrix of current state
    ukf_rot.Q = Eigen::Matrix<double,3,3>::Identity(); // covariance matrix of process noise

    // rotation (x,y,z) [rad]?
    ukf_rot.Q(0,0) = 0.00001;
    ukf_rot.Q(1,1) = 0.00001;
    ukf_rot.Q(2,2) = 0.00001;

    // rotational_velocity (x,y,z) [rad/s], too small?
//    ukf_rot.Q(3,3) = 0.1;
//    ukf_rot.Q(4,4) = 0.1;
//    ukf_rot.Q(5,5) = 0.1;

    // synthetic measurement covariance matrix used for testing
    R_rotation << 10e-2, 0, 0,// 0, 0, 0,
                  0, 10e-2, 0,// 0, 0, 0,
                  0, 0, 10e-2;// 0, 0, 0,
//                  0, 0, 0,  3.434758685147043306e-06, -8.299226917536411892e-08,  5.842662059539863827e-08,
//                  0, 0, 0, -8.299226917536411892e-08,  1.006052718494827880e-05,  1.346681994776136150e-06,
//                  0, 0, 0,  5.842662059539863827e-08,  1.346681994776136150e-06,  3.242298821157115427e-06;

    // set covariance matrix of measurement noise
    // measured covariance of acceleration and rotational velocity (motion log, 60 seconds)
//            R << 5.074939351879890342e-04, -1.561730283237946278e-05,  1.012849085655689321e-04, 0     , 0     , 0     , -3.078687958578659292e-08, -1.132513004663809251e-06, -6.485352375515866273e-07,
//                -1.561730283237946278e-05,  2.570436087068024501e-04, -4.159091012580820026e-05, 0     , 0     , 0     , -3.013278205585369588e-07,  1.736820285922189584e-06, -4.599219827687661978e-07,
//                 1.012849085655689321e-04, -4.159091012580820026e-05,  4.727921819788054878e-04, 0     , 0     , 0     ,  5.523361976811979815e-07, -1.730307422507887473e-07, -3.030009469390110280e-07,
//                 0                       ,  0                       ,  0                       , 10e-11, 0     , 0     ,  0                       ,  0                       ,  0                       ,
//                 0                       ,  0                       ,  0                       , 0     , 10e-11, 0     ,  0                       ,  0                       ,  0                       ,
//                 0                       ,  0                       ,  0                       , 0     , 0     , 10e-11,  0                       ,  0                       ,  0                       ,
//                -3.078687958578659292e-08, -3.013278205585369588e-07,  5.523361976811979815e-07, 0     , 0     , 0     ,  3.434758685147043306e-06, -8.299226917536411892e-08,  5.842662059539863827e-08,
//                -1.132513004663809251e-06,  1.736820285922189584e-06, -1.730307422507887473e-07, 0     , 0     , 0     , -8.299226917536411892e-08,  1.006052718494827880e-05,  1.346681994776136150e-06,
//                -6.485352375515866273e-07, -4.599219827687661978e-07, -3.030009469390110280e-07, 0     , 0     , 0     ,  5.842662059539863827e-08,  1.346681994776136150e-06,  3.242298821157115427e-06;

    // --- for testing integration around z
    angle_about_z = 0;
    // --- end
}

void IMUModel::execute(){
    DEBUG_REQUEST("IMUModel:reset_filter",
                  ukf_rot.reset();
                  ukf_acc_global.reset();
    );

    double dt = getFrameInfo().getTimeInSeconds() - lastFrameInfo.getTimeInSeconds();

    ukf_rot.generateSigmaPoints();
    Eigen::Vector3d gyro;
    gyro << getGyrometerData().data.x, getGyrometerData().data.y, -getGyrometerData().data.z;
    ukf_rot.predict(gyro, dt);

    // don't generate sigma points again because the process noise would be applied a second time
    // ukf.generateSigmaPoints();

    Eigen::Vector3d acceleration = Eigen::Vector3d(getAccelerometerData().data.x, getAccelerometerData().data.y, getAccelerometerData().data.z);

    // --- for debug purpose only
    bool use_gyro_only = false;
    bool use_both = false;

    DEBUG_REQUEST("IMUModel:use_both", use_both = true;);
    DEBUG_REQUEST("IMUModel:use_only_gyro", use_gyro_only = true;);
    // --- end

    // --- for testing integration around z
    angle_about_z += -getGyrometerData().data.z * dt;
    PLOT("IMUModel:angle_about:z", Math::toDegrees(fmod(angle_about_z, 2*M_PI)));
    // --- end

    // TODO: empirical determination or better condition
    //if((fabs(acceleration.norm() - 9.81) < 0.5 || use_both) && !use_gyro_only ){ // use acceleration
        Eigen::Quaterniond rotation_acc_to_gravity;
        rotation_acc_to_gravity.setFromTwoVectors(acceleration,Eigen::Vector3d(0,0,-1));

        IMU_RotationMeasurement z;
        // gyro z axis seems to measure in opposite direction (turning left measures negative angular velocity, should be positive)
        z << quaternionToRotationVector(rotation_acc_to_gravity);//, getGyrometerData().data.x, getGyrometerData().data.y, -getGyrometerData().data.z;

        ukf_rot.update(z, R_rotation);

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

    ukf_acc_global.generateSigmaPoints();
    double u = 0;
    ukf_acc_global.predict(u, dt);
    ukf_acc_global.update(z_acc, R_acc);

    writeIMUData(dt);

    plots();

    lastFrameInfo = getFrameInfo();
}

void IMUModel::writeIMUData(double dt){
    // raw sensor values
    getIMUData().rotational_velocity_sensor = getGyrometerData().data;
    getIMUData().acceleration_sensor = getAccelerometerData().data;

    // global position data
    getIMUData().location += getIMUData().velocity * dt;

    getIMUData().velocity += getIMUData().acceleration * dt;

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
}

void IMUModel::plots(){
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

    Vector3d xAxis = Vector3<double>(60, 0, 0);
    Vector3d yAxis = Vector3<double>(0, 60, 0);
    Vector3d zAxis = Vector3<double>(0, 0, 60);

    // plot gravity
    Eigen::Vector3d acceleration;
    acceleration << getAccelerometerData().data.x, getAccelerometerData().data.y, getAccelerometerData().data.z;
    Eigen::Vector3d temp3 = ukf_rot.state.getRotationAsQuaternion()._transformVector(acceleration);
    Vector3d gravity(6*temp3(0),6*temp3(1),6*temp3(2));

    xAxis = pose * xAxis;
    yAxis = pose * yAxis;
    zAxis = pose * zAxis;

    BOX_3D("FFA07A", 15, 15, 30, pose);
    LINE_3D(ColorClasses::red, pose.translation, xAxis);
    LINE_3D(ColorClasses::green, pose.translation, yAxis);
    LINE_3D(ColorClasses::blue, pose.translation, zAxis);

    LINE_3D(ColorClasses::black, pose.translation, pose.translation + gravity);

//    PLOT("IMUModel:State:rotational_velocity:x", ukf_rot.state.rotational_velocity()(0,0));
//    PLOT("IMUModel:State:rotational_velocity:y", ukf_rot.state.rotational_velocity()(1,0));
//    PLOT("IMUModel:State:rotational_velocity:z", ukf_rot.state.rotational_velocity()(2,0));

//    PLOT("IMUModel:State:bias_rotational_velocity:x", ukf.state.bias_rotational_velocity()(0,0));
//    PLOT("IMUModel:State:bias_rotational_velocity:y", ukf.state.bias_rotational_velocity()(1,0));
//    PLOT("IMUModel:State:bias_rotational_velocity:z", ukf.state.bias_rotational_velocity()(2,0));

    PLOT("IMUModel:Measurement:rotational_velocity:x", getGyrometerData().data.x);
    PLOT("IMUModel:Measurement:rotational_velocity:y", getGyrometerData().data.y);
    PLOT("IMUModel:Measurement:rotational_velocity:z", getGyrometerData().data.z);

    PLOT("IMUModel:updated_by_both", updated_by_both);
}

#pragma GCC diagnostic pop
