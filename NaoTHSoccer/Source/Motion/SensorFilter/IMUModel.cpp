#include "IMUModel.h"

//TODO: remove pragma, problem with eigens optimization stuff "because conversion sequence for the argument is better"
#pragma GCC diagnostic ignored "-Wconversion"

IMUModel::IMUModel(){
    DEBUG_REQUEST_REGISTER("IMUModel:reset_filter", "reset filter", false);
}

void IMUModel::execute(){
    DEBUG_REQUEST("IMUModel:reset_filter",
                  ukf.reset();
    );

    ukf.generateSigmaPoints();

    double dt = getFrameInfo().getTimeInSeconds() - lastFrameInfo.getTimeInSeconds();
    ukf.predict(dt);

    // don't generate sigma points again because the process noise would be applied a second time
    // ukf.generateSigmaPoints();

    Eigen::Quaterniond rotation_acc_to_gravity;
    Eigen::Vector3d acceleration = Eigen::Vector3d(getAccelerometerData().data.x, getAccelerometerData().data.y, getAccelerometerData().data.z);
    rotation_acc_to_gravity.setFromTwoVectors(acceleration,Eigen::Vector3d(0,0,-1));

    IMU_Measurement z;
    // gyro z axis seems to measure in opposite direction (turning left measures negative angular velocity, should be positive)
    z << quaternionToRotationVector(rotation_acc_to_gravity), getGyrometerData().data.x, getGyrometerData().data.y, -getGyrometerData().data.z;

    ukf.update(z);

    writeIMUData();

    plots();

    lastFrameInfo     = getFrameInfo();
}

void IMUModel::writeIMUData(){
//    getIMUData().location.x = ukf.state.location()(0,0);
//    getIMUData().location.y = ukf.state.location()(1,0);
//    getIMUData().location.z = ukf.state.location()(2,0);

//    getIMUData().velocity.x = ukf.state.velocity()(0,0);
//    getIMUData().velocity.y = ukf.state.velocity()(1,0);
//    getIMUData().velocity.z = ukf.state.velocity()(2,0);

//    getIMUData().acceleration.x = ukf.state.acceleration()(0,0);
//    getIMUData().acceleration.y = ukf.state.acceleration()(1,0);
//    getIMUData().acceleration.z = ukf.state.acceleration()(2,0);

    getIMUData().acceleration_sensor = getAccelerometerData().data;

//    getIMUData().bias_acceleration.x = ukf.state.bias_acceleration()(0,0);
//    getIMUData().bias_acceleration.y = ukf.state.bias_acceleration()(1,0);
//    getIMUData().bias_acceleration.z = ukf.state.bias_acceleration()(2,0);

    Eigen::Quaterniond q;
    q = ukf.state.getRotationAsQuaternion();
    Eigen::Vector3d angles = q.toRotationMatrix().eulerAngles(0,1,2);
    getIMUData().rotation.x = angles(0);
    getIMUData().rotation.y = angles(1);
    getIMUData().rotation.z = angles(2);

    getIMUData().rotational_velocity.x = ukf.state.rotational_velocity()(0,0);
    getIMUData().rotational_velocity.y = ukf.state.rotational_velocity()(1,0);
    getIMUData().rotational_velocity.z = ukf.state.rotational_velocity()(2,0);

    getIMUData().rotational_velocity_sensor = getGyrometerData().data;

    getIMUData().bias_rotational_velocity.x = ukf.state.bias_rotational_velocity()(0,0);
    getIMUData().bias_rotational_velocity.y = ukf.state.bias_rotational_velocity()(1,0);
    getIMUData().bias_rotational_velocity.z = ukf.state.bias_rotational_velocity()(2,0);

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
    q = ukf.state.getRotationAsQuaternion();
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
    Eigen::Vector3d temp3 = ukf.state.getRotationAsQuaternion()._transformVector(acceleration);
    Vector3d gravity(6*temp3(0),6*temp3(1),6*temp3(2));

    xAxis = pose * xAxis;
    yAxis = pose * yAxis;
    zAxis = pose * zAxis;

    BOX_3D("FFA07A", 15, 15, 30, pose);
    LINE_3D(ColorClasses::red, pose.translation, xAxis);
    LINE_3D(ColorClasses::green, pose.translation, yAxis);
    LINE_3D(ColorClasses::blue, pose.translation, zAxis);

    LINE_3D(ColorClasses::black, pose.translation, pose.translation + gravity);

    PLOT("IMUModel:State:rotational_velocity:x", ukf.state.rotational_velocity()(0,0));
    PLOT("IMUModel:State:rotational_velocity:y", ukf.state.rotational_velocity()(1,0));
    PLOT("IMUModel:State:rotational_velocity:z", ukf.state.rotational_velocity()(2,0));

//    PLOT("IMUModel:State:bias_rotational_velocity:x", ukf.state.bias_rotational_velocity()(0,0));
//    PLOT("IMUModel:State:bias_rotational_velocity:y", ukf.state.bias_rotational_velocity()(1,0));
//    PLOT("IMUModel:State:bias_rotational_velocity:z", ukf.state.bias_rotational_velocity()(2,0));

    PLOT("IMUModel:Measurement:rotational_velocity:x", getGyrometerData().data.x);
    PLOT("IMUModel:Measurement:rotational_velocity:y", getGyrometerData().data.y);
    PLOT("IMUModel:Measurement:rotational_velocity:z", getGyrometerData().data.z);
}

#pragma GCC diagnostic pop
