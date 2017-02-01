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
    z << acceleration, quaternionToRotationVector(rotation_acc_to_gravity), getGyrometerData().data.x, getGyrometerData().data.y, -getGyrometerData().data.z;

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

    getIMUData().acceleration.x = ukf.state.acceleration()(0,0);
    getIMUData().acceleration.y = ukf.state.acceleration()(1,0);
    getIMUData().acceleration.z = ukf.state.acceleration()(2,0);

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

//    getIMUData().bias_rotational_velocity.x = ukf.state.bias_rotational_velocity()(0,0);
//    getIMUData().bias_rotational_velocity.y = ukf.state.bias_rotational_velocity()(1,0);
//    getIMUData().bias_rotational_velocity.z = ukf.state.bias_rotational_velocity()(2,0);

    // from inertiasensorfilter
    getIMUData().orientation = Vector2d(atan2(q.toRotationMatrix()(2,1),  q.toRotationMatrix()(2,2)),
                                        atan2(-q.toRotationMatrix()(2,0), q.toRotationMatrix()(2,2)));

//    Eigen::Vector3d new_z = q._transformVector(Eigen::Vector3d(0,0,1));
//    Eigen::Vector3d temp;

//    temp << 0, new_z(1), new_z(2);
//    getIMUData().orientation.x = std::acos(1/temp.norm()*(temp.transpose()*Eigen::Vector3d(0,0,1))(0,0));

//    temp << new_z(0), 0, new_z(2);
//    getIMUData().orientation.y = std::acos(1/temp.norm()*(temp.transpose()*Eigen::Vector3d(0,0,1))(0,0));
}

void IMUModel::plots(){
    PLOT("IMUModel:State:acceleration:x", ukf.state.acceleration()(0,0));
    PLOT("IMUModel:State:acceleration:y", ukf.state.acceleration()(1,0));
    PLOT("IMUModel:State:acceleration:z", ukf.state.acceleration()(2,0));

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
    Eigen::Vector3d temp3 = ukf.state.getRotationAsQuaternion()._transformVector(ukf.state.acceleration());
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

template <int dim_state, int dim_state_cov, int dim_measurement, int dim_measurement_cov, class S, class M>
void UKF<dim_state, dim_state_cov, dim_measurement, dim_measurement_cov, S, M>::reset(){
    P     = Eigen::Matrix<double,dim_state_cov,dim_state_cov>::Identity();
    state = S();
}

template <int dim_state, int dim_state_cov, int dim_measurement, int dim_measurement_cov, class S, class M>
void UKF<dim_state, dim_state_cov, dim_measurement, dim_measurement_cov, S, M>::predict(double dt){
    // transit the sigma points to the next state
    for (typename std::vector<S>::iterator i = sigmaPoints.begin(); i != sigmaPoints.end(); i++){
        transitionFunction(*i,dt);
    }

    std::vector<Eigen::Quaterniond> rotations;
    S mean;
    // calculate new state (weighted mean of sigma points)
    for(typename std::vector<S>::iterator i = sigmaPoints.begin(); i != sigmaPoints.end(); ++i){
        rotations.push_back((*i).getRotationAsQuaternion());
        mean += (*i).scale(1.0 / static_cast<double>(sigmaPoints.size()));
    }

    // more correct determination of the mean rotation
    mean.rotation() = averageRotation(rotations, rotations.back());

    // calculate new process covariance
    Eigen::Matrix<double, dim_state_cov, dim_state_cov> cov = Eigen::Matrix<double, dim_state_cov, dim_state_cov>::Zero();
    Eigen::Matrix<double, dim_state_cov,1> temp;

    for(typename std::vector<S>::iterator i = sigmaPoints.begin(); i != sigmaPoints.end(); ++i){
        temp = (*i) - mean;
        // replace wrong rotational difference
        Eigen::AngleAxis<double> error((*i).getRotationAsQuaternion() * mean.getRotationAsQuaternion().inverse());
        temp.block(3,0,3,1) = error.angle() * error.axis();
        cov += 1.0 / static_cast<double>(sigmaPoints.size()) * (temp)*(temp).transpose();
    }

    //assert (mean.allFinite());
    //assert (cov.allFinite());
    state = mean;
    P     = cov/* + Q*/; // process covariance is applied before the process model (while generating the sigma points)
}

// TODO: get rid of direct matrix accesses
template <int dim_state, int dim_state_cov, int dim_measurement, int dim_measurement_cov, class S, class M>
void UKF <dim_state, dim_state_cov, dim_measurement, dim_measurement_cov,S,M>::update(M z){
    std::vector<M> sigmaMeasurements;

    // map sigma points to measurement space
    for (typename std::vector<S>::iterator i = sigmaPoints.begin(); i != sigmaPoints.end(); i++){
        sigmaMeasurements.push_back(stateToMeasurementSpaceFunction(*i));
    }

    // calculate predicted measurement z (weighted mean of sigma points)
    std::vector<Eigen::Quaterniond> rotations;
    M predicted_z;

    for(typename std::vector<M>::iterator i = sigmaMeasurements.begin(); i != sigmaMeasurements.end(); ++i){
        rotations.push_back((*i).getRotationAsQuaternion());
        predicted_z += 1.0 / static_cast<double>(sigmaMeasurements.size()) * (*i);
    }

    // more correct determination of the mean rotation
    predicted_z.rotation() = averageRotation(rotations, rotations.back());

    // calculate current measurement covariance
    Eigen::Matrix<double, dim_measurement_cov, dim_measurement_cov> Pzz = Eigen::Matrix<double, dim_measurement_cov, dim_measurement_cov>::Zero();

    for(typename std::vector<M>::iterator i = sigmaMeasurements.begin(); i != sigmaMeasurements.end(); ++i){
        Eigen::Matrix<double, dim_measurement_cov,1> temp((*i)- predicted_z);
        // replace wrong rotational difference
        Eigen::AngleAxis<double> error((*i).getRotationAsQuaternion() * predicted_z.getRotationAsQuaternion().inverse());
        temp.block(3,0,3,1) = error.angle() * error.axis();
        Pzz += 1.0 / static_cast<double>(sigmaPoints.size()) * (temp)*(temp).transpose();
    }

    // apply measurement noise covariance
    Eigen::Matrix<double, dim_measurement_cov, dim_measurement_cov> Pvv = Pzz + R;

    // calculate state-measurement cross-covariance
    Eigen::Matrix<double, dim_state_cov, dim_measurement_cov> Pxz = Eigen::Matrix<double, dim_state_cov, dim_measurement_cov>::Zero();

    for(unsigned int i = 0; i < sigmaPoints.size(); i++){
        Eigen::Matrix<double, dim_state_cov,1> temp1(sigmaPoints[i] - state);
        // replace wrong rotational difference
        Eigen::AngleAxis<double> error(sigmaPoints[i].getRotationAsQuaternion() * state.getRotationAsQuaternion().inverse());
        temp1.block(3,0,3,1) = error.angle() * error.axis();

        Eigen::Matrix<double, dim_measurement_cov,1> temp2(sigmaMeasurements[i] - predicted_z);
        // replace wrong rotational difference
        error = Eigen::AngleAxis<double>(sigmaMeasurements[i].getRotationAsQuaternion() * predicted_z.getRotationAsQuaternion().inverse());
        temp2.block(3,0,3,1) = error.angle() * error.axis();

        Pxz += 1.0 / static_cast<double>(sigmaPoints.size()) * (temp1) * (temp2).transpose();
    }

    // calculate kalman gain
    Eigen::Matrix<double, dim_state_cov, dim_measurement_cov> K;
    K = Pxz*Pvv.inverse();

    // calculate new state and covariance

    Eigen::Matrix<double, dim_measurement_cov,1> z_innovation = z - predicted_z;
    // replace wrong rotational difference
    Eigen::AngleAxis<double> error(z.getRotationAsQuaternion() * predicted_z.getRotationAsQuaternion().inverse());
    z_innovation.block(3,0,3,1) = error.angle() * error.axis();

    S state_innovation(K*z_innovation);

    // HACK
    // TODO reduce measurement space dimensionality by one, rotation around z axis isn't measured => screws things up
    state_innovation(5,0) = 0.0;

    state += state_innovation;

    //assert (state.allFinite());

    Eigen::Matrix<double,dim_state_cov,dim_state_cov> P_wiki;
    P_wiki   = P - K*Pzz*K.transpose(); // https://en.m.wikipedia.org/wiki/Kalman_filter
    //P_gh     = P - Pxz*Pzz.inverse()*Pxz.transpose(); // from "Performance and Implementation Aspects of Nonlinear Filtering" by Gustaf Hendeby

    P = P_wiki;
    //assert (P.allFinite());
}

template <int dim_state, int dim_state_cov, int dim_measurement, int dim_measurement_cov, class S, class M>
void UKF<dim_state, dim_state_cov, dim_measurement, dim_measurement_cov,S,M>::transitionFunction(S& state, double dt){
    // rotational_velocity to quaternion
    Eigen::Vector3d rotational_velocity = state.rotational_velocity();
    Eigen::Quaterniond rotation_increment;
    if(rotational_velocity.norm() > 0) {
        rotation_increment = Eigen::Quaterniond(Eigen::AngleAxisd(rotational_velocity.norm()*dt, rotational_velocity.normalized()));
    } else {
        // zero rotation quaternion
        rotation_increment = Eigen::Quaterniond(1,0,0,0);
    }

    // continue rotation assuming constant velocity
    // TODO: compare with rotation_increment*rotation which sounds more reasonable
    Eigen::Quaterniond new_rotation = state.getRotationAsQuaternion() * rotation_increment; // follows paper
    Eigen::AngleAxisd new_angle_axis(new_rotation);

    state.rotation() = new_angle_axis.angle() * new_angle_axis.axis();

    state.acceleration() = rotation_increment.inverse()._transformVector(state.acceleration());
}

template <int dim_state, int dim_state_cov, int dim_measurement, int dim_measurement_cov, class S, class M>
M UKF<dim_state, dim_state_cov, dim_measurement, dim_measurement_cov, S, M>::stateToMeasurementSpaceFunction(S& state){
    // state to measurement function
    // transform acceleration part of the state into local measurement space (the robot's body frame = frame of accelerometer), the bias is already in this frame
    Eigen::Vector3d acceleration_in_measurement_space        = state.acceleration() /*+ state.bias_acceleration()*/; // TODO: check for rotation._transformVector(...) ...
    Eigen::Vector3d rotational_velocity_in_measurement_space = state.rotational_velocity() /*+ state.bias_rotational_velocity()*/;

    // determine rotation around x and y axis
    Eigen::Quaterniond q;
    q.setFromTwoVectors(state.getRotationAsQuaternion().inverse()._transformVector(Eigen::Vector3d(0,0,-1)),Eigen::Vector3d(0,0,-1));
    Eigen::AngleAxisd temp(q);
    Eigen::Vector3d   rotation_in_measurement_space(temp.angle() * temp.axis());

    M return_val;
    return_val << acceleration_in_measurement_space, rotation_in_measurement_space, rotational_velocity_in_measurement_space;

    return return_val;
}

template <int dim_state, int dim_state_cov, int dim_measurement, int dim_measurement_cov, class S, class M>
void UKF<dim_state, dim_state_cov, dim_measurement, dim_measurement_cov,S,M>::generateSigmaPoints(){
    sigmaPoints.resize(2*dim_state_cov+1);

    sigmaPoints[2*dim_state_cov] = state;

    // HACK: ensure positiv semidefinity of P
    // P = 0.5*P+0.5*P.transpose();
    // P = P + 10E-4*Eigen::Matrix<double,dim_state,dim_state>::Identity();

    Eigen::LLT<Eigen::Matrix<double,dim_state_cov,dim_state_cov> > choleskyDecompositionOfCov(P+Q); // apply Q befor the process model
    Eigen::Matrix<double,dim_state_cov,dim_state_cov> L = choleskyDecompositionOfCov.matrixL();

    //assert (L.allFinite());

    for(int i = 0; i < dim_state_cov; i++){
        S noise(std::sqrt(2*dim_state_cov) * L.col(i));

        // TODO: check which order is correct (matters because of the rotation part), order as used in the paper
        sigmaPoints[i]  = noise;
        sigmaPoints[i] += state;

        // generate "opposite" sigma point
        // TODO: check which order is correct (matters because of the rotation part), order as used in the paper
        sigmaPoints[i + dim_state_cov]  = -noise;
        sigmaPoints[i + dim_state_cov] += state;
    }
}

#pragma GCC diagnostic pop
