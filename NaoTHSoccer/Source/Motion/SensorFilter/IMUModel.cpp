#include "IMUModel.h"

//TODO: remove pragma
#pragma GCC diagnostic ignored "-Wconversion"

IMUModel::IMUModel()
{
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

    Measurement z;
    z << acceleration, getGyrometerData().data.x, getGyrometerData().data.y, getGyrometerData().data.z, rotation_acc_to_gravity.coeffs();

    ukf.update(z);

    writeIMUData();

    plots();

    lastFrameInfo     = getFrameInfo();
}

void IMUModel::writeIMUData(){
    getIMUData().location.x = ukf.state.location()(0,0);
    getIMUData().location.y = ukf.state.location()(1,0);
    getIMUData().location.z = ukf.state.location()(2,0);

    getIMUData().velocity.x = ukf.state.velocity()(0,0);
    getIMUData().velocity.y = ukf.state.velocity()(1,0);
    getIMUData().velocity.z = ukf.state.velocity()(2,0);

    getIMUData().acceleration.x = ukf.state.acceleration()(0,0);
    getIMUData().acceleration.y = ukf.state.acceleration()(1,0);
    getIMUData().acceleration.z = ukf.state.acceleration()(2,0);

    getIMUData().acceleration_sensor = getAccelerometerData().data;

//    getIMUData().bias_acceleration.x = ukf.state.bias_acceleration()(0,0);
//    getIMUData().bias_acceleration.y = ukf.state.bias_acceleration()(1,0);
//    getIMUData().bias_acceleration.z = ukf.state.bias_acceleration()(2,0);

    Eigen::Quaterniond q;
    q = Eigen::Quaterniond(Eigen::Vector4d(ukf.state.rotation()));
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
    // getIMUData().orientation = Vector2d(atan2(q.toRotationMatrix()(2,1),  q.toRotationMatrix()(2,2)),
    //                                     atan2(-q.toRotationMatrix()(2,0), q.toRotationMatrix()(2,2)));

    Eigen::Vector3d new_z = q._transformVector(Eigen::Vector3d(0,0,1));
    Eigen::Vector3d temp;

    temp << 0, new_z(1), new_z(2);
    getIMUData().orientation.x = std::acos(1/temp.norm()*(temp.transpose()*Eigen::Vector3d(0,0,1))(0,0));

    temp << new_z(0), 0, new_z(2);
    getIMUData().orientation.y = std::acos(1/temp.norm()*(temp.transpose()*Eigen::Vector3d(0,0,1))(0,0));
}

void IMUModel::plots(){
    PLOT("IMUModel:State:location:x", ukf.state.location()(0,0));
    PLOT("IMUModel:State:location:y", ukf.state.location()(1,0));
    PLOT("IMUModel:State:location:z", ukf.state.location()(2,0));

    PLOT("IMUModel:State:velocity:x", ukf.state.velocity()(0,0));
    PLOT("IMUModel:State:velocity:y", ukf.state.velocity()(1,0));
    PLOT("IMUModel:State:velocity:z", ukf.state.velocity()(2,0));

    PLOT("IMUModel:State:acceleration:x", ukf.state.acceleration()(0,0));
    PLOT("IMUModel:State:acceleration:y", ukf.state.acceleration()(1,0));
    PLOT("IMUModel:State:acceleration:z", ukf.state.acceleration()(2,0));

//    PLOT("IMUModel:State:bias_acceleration:x", ukf.state.bias_acceleration()(0,0));
//    PLOT("IMUModel:State:bias_acceleration:y", ukf.state.bias_acceleration()(1,0));
//    PLOT("IMUModel:State:bias_acceleration:z", ukf.state.bias_acceleration()(2,0));

    PLOT("IMUModel:Measurement:acceleration:x", getAccelerometerData().data.x);
    PLOT("IMUModel:Measurement:acceleration:y", getAccelerometerData().data.y);
    PLOT("IMUModel:Measurement:acceleration:z", getAccelerometerData().data.z);

    Eigen::Quaterniond q;
    q = Eigen::Quaterniond(Eigen::Vector4d(ukf.state.rotation()));
    Eigen::Vector3d angles = q.toRotationMatrix().eulerAngles(0,1,2);
    PLOT("IMUModel:State:rotation:x", angles(0));
    PLOT("IMUModel:State:rotation:y", angles(1));
    PLOT("IMUModel:State:rotation:z", angles(2));

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

template <int dim_state, int dim_state_cov, int dim_measurement, int dim_measurement_cov>
void UKF<dim_state, dim_state_cov, dim_measurement, dim_measurement_cov>::reset(){
    P     = Eigen::Matrix<double,dim_state_cov,dim_state_cov>::Identity();
    state = State();
}

template <int dim_state, int dim_state_cov, int dim_measurement, int dim_measurement_cov>
void UKF<dim_state, dim_state_cov, dim_measurement, dim_measurement_cov>::predict(double dt){
    // transit the sigma points to the next state
    for (typename std::vector<State>::iterator i = sigmaPoints.begin(); i != sigmaPoints.end(); i++){
        transitionFunction(*i,dt);
    }

    std::vector<Eigen::Quaterniond> rotations;
    State mean;
    // calculate new state (weighted mean of sigma points)
    for(typename std::vector<State>::iterator i = sigmaPoints.begin(); i != sigmaPoints.end(); ++i){
        rotations.push_back((*i).getRotationAsQuaternion());
        mean += 1.0 / sigmaPoints.size() * (*i);
    }

    // more correct determination of the mean rotation
    mean.rotation() = averageRotation(rotations, rotations.back()).coeffs();

    // calculate new process covariance
    Eigen::Matrix<double, dim_state_cov, dim_state_cov> cov = Eigen::Matrix<double, dim_state_cov, dim_state_cov>::Zero();
    Eigen::Matrix<double, dim_state_cov,1> temp;
    Eigen::Matrix<double, dim_state_cov,1> m_compatible = mean.toCovarianceCompatibleState();

    for(typename std::vector<State>::iterator i = sigmaPoints.begin(); i != sigmaPoints.end(); ++i){
        temp = (*i).toCovarianceCompatibleState() - m_compatible;
        // replace wrong rotational difference
        Eigen::AngleAxis<double> error((*i).getRotationAsQuaternion() * mean.getRotationAsQuaternion().inverse());
        temp.block(12,0,3,1) = error.angle() * error.axis();
        cov += 1.0 / sigmaPoints.size() * (temp)*(temp).transpose();
    }

    state = mean;
    P     = cov/* + Q*/; // process covariance is applied before the process model (while generating the sigma points)
}

template <int dim_state, int dim_state_cov, int dim_measurement, int dim_measurement_cov>
void UKF <dim_state, dim_state_cov, dim_measurement, dim_measurement_cov>::update(Measurement z){
    std::vector<Measurement> sigmaMeasurements;

    // map sigma points to measurement space
    for (typename std::vector<State>::iterator i = sigmaPoints.begin(); i != sigmaPoints.end(); i++){
        sigmaMeasurements.push_back(stateToMeasurementSpaceFunction(*i));
    }

    std::vector<Eigen::Quaterniond> rotations;
    Measurement predicted_z;
    // calculate predicted measurement z (weighted mean of sigma points)
    for(typename std::vector<Measurement>::iterator i = sigmaMeasurements.begin(); i != sigmaMeasurements.end(); ++i){
        rotations.push_back((*i).getRotationAsQuaternion());
        predicted_z += 1.0 / sigmaMeasurements.size() * (*i);
    }

    // more correct determination of the mean rotation
    predicted_z.rotation() = averageRotation(rotations, rotations.back()).coeffs();

    // calculate current measurement covariance
    Eigen::Matrix<double, dim_measurement_cov, dim_measurement_cov> Pzz = Eigen::Matrix<double, dim_measurement_cov, dim_measurement_cov>::Zero();
    Eigen::Matrix<double, dim_measurement_cov,1> pz_compatible = predicted_z.toCovarianceCompatibleState();

    for(typename std::vector<Measurement>::iterator i = sigmaMeasurements.begin(); i != sigmaMeasurements.end(); ++i){
        Eigen::Matrix<double, dim_measurement_cov,1> temp((*i).toCovarianceCompatibleState() - pz_compatible);
        // replace wrong rotational difference
        Eigen::AngleAxis<double> error((*i).getRotationAsQuaternion() * predicted_z.getRotationAsQuaternion().inverse());
        temp.block(6,0,3,1) = error.angle() * error.axis();
        Pzz += 1.0 / sigmaPoints.size() * (temp)*(temp).transpose();
    }

    // apply measurement noise covariance
    Eigen::Matrix<double, dim_measurement_cov, dim_measurement_cov> Pvv = Pzz + R;

    // calculate state-measurement cross-covariance
    Eigen::Matrix<double, dim_state_cov, dim_measurement_cov> Pxz = Eigen::Matrix<double, dim_state_cov, dim_measurement_cov>::Zero();
    Eigen::Matrix<double, dim_state_cov,1> state_compatible = state.toCovarianceCompatibleState();

    for(unsigned int i = 0; i < sigmaPoints.size(); i++){
        Eigen::Matrix<double, dim_state_cov,1> temp1(sigmaPoints[i].toCovarianceCompatibleState() - state_compatible);
        // replace wrong rotational difference
        Eigen::AngleAxis<double> error(sigmaPoints[i].getRotationAsQuaternion() * state.getRotationAsQuaternion().inverse());
        temp1.block(12,0,3,1) = error.angle() * error.axis();

        Eigen::Matrix<double, dim_measurement_cov,1> temp2(sigmaMeasurements[i].toCovarianceCompatibleState() - pz_compatible);
        // replace wrong rotational difference
        error = Eigen::AngleAxis<double>(sigmaMeasurements[i].getRotationAsQuaternion() * predicted_z.getRotationAsQuaternion().inverse());
        temp2.block(6,0,3,1) = error.angle() * error.axis();

        Pxz += 1.0 / sigmaPoints.size() * (temp1) * (temp2).transpose();
    }

    // calculate kalman gain
    Eigen::Matrix<double, dim_state_cov, dim_measurement_cov> K;
    K = Pxz*Pvv.inverse();

    // calculate new state and covariance

    Eigen::Matrix<double, dim_measurement_cov,1> z_innovation = z.toCovarianceCompatibleState() - predicted_z.toCovarianceCompatibleState();
    // replace wrong rotational difference
    Eigen::AngleAxis<double> error(z.getRotationAsQuaternion() * predicted_z.getRotationAsQuaternion().inverse());
    z_innovation.block(6,0,3,1) = error.angle() * error.axis();

    State state_innovation = toFullState(K*(z_innovation));

    state += state_innovation;

    Eigen::Matrix<double,dim_state_cov,dim_state_cov> P_wiki;
    P_wiki   = P - K*Pzz*K.transpose(); // https://en.m.wikipedia.org/wiki/Kalman_filter
    //P_gh     = P - Pxz*Pzz.inverse()*Pxz.transpose(); // from "Performance and Implementation Aspects of Nonlinear Filtering" by Gustaf Hendeby

    P = P_wiki;
}

template <int dim_state, int dim_state_cov, int dim_measurement, int dim_measurement_cov>
void UKF<dim_state, dim_state_cov, dim_measurement, dim_measurement_cov>::transitionFunction(State& state, double dt){
    // rotational_velocity to quaternion
    Eigen::Vector3d rotational_velocity = state.rotational_velocity();
    Eigen::Quaterniond rotation_increment;
    if(rotational_velocity.norm() > 0) {
        rotation_increment = Eigen::Quaterniond(Eigen::AngleAxisd(rotational_velocity.norm()*dt, rotational_velocity.normalized()));
                             // TODO: check if equal to the above
                             /* Eigen::Quaterniond (
                                std::cos(norm_rv/2*dt),
                                state.rotational_velocity()(0)/norm_rv*std::sin(norm_rv*dt/2),
                                state.rotational_velocity()(1)/norm_rv*std::sin(norm_rv*dt/2),
                                state.rotational_velocity()(2)/norm_rv*std::sin(norm_rv*dt/2)); */
    } else {
        // zero rotation quaternion
        rotation_increment = Eigen::Quaterniond(1,0,0,0);
    }

    Eigen::Quaterniond rotation = Eigen::Quaterniond(Eigen::Vector4d(state.rotation()));
    // continue rotation assuming constant velocity
    // TODO: compare with rotation_increment*rotation which sounds more reasonable
    state.rotation() = (rotation*rotation_increment).coeffs(); // follows paper

    // linear motion model
    state.location() = state.location() + state.velocity()*dt + state.acceleration()*dt*dt/2;
    state.velocity() = state.velocity() + state.acceleration()*dt;
}

template <int dim_state, int dim_state_cov, int dim_measurement, int dim_measurement_cov>
typename UKF<dim_state, dim_state_cov, dim_measurement, dim_measurement_cov>::Measurement UKF<dim_state, dim_state_cov, dim_measurement, dim_measurement_cov>::stateToMeasurementSpaceFunction(State& state){
    Eigen::Quaterniond rotation = Eigen::Quaterniond(Eigen::Vector4d(state.rotation()));

    // state to measurement function
    // transform acceleration part of the state into local measurement space (the robot's body frame = frame of accelerometer), the bias is already in this frame
    Eigen::Vector3d acceleration_in_measurement_space        = rotation.inverse()._transformVector(state.acceleration()) /*+ state.bias_acceleration()*/; // TODO: check for rotation._transformVector(...) ...
    Eigen::Vector3d rotational_velocity_in_measurement_space = state.rotational_velocity() /*+ state.bias_rotational_velocity()*/;

    // rotation from  acceleration to (0,0,-1)
    Eigen::Quaterniond rotation_in_measurement_space;
    rotation_in_measurement_space.setFromTwoVectors(acceleration_in_measurement_space,Eigen::Vector3d(0,0,-1));

    Measurement return_val;
    return_val << acceleration_in_measurement_space, rotational_velocity_in_measurement_space, rotation_in_measurement_space.coeffs();

    return return_val;
}

template <int dim_state, int dim_state_cov, int dim_measurement, int dim_measurement_cov>
void UKF<dim_state, dim_state_cov, dim_measurement, dim_measurement_cov>::generateSigmaPoints(){
    sigmaPoints.resize(2*dim_state_cov+1);

    sigmaPoints[2*dim_state_cov] = state;

    // HACK: ensure positiv semidefinity of P
    // P = 0.5*P+0.5*P.transpose();
    // P = P + 10E-4*Eigen::Matrix<double,dim_state,dim_state>::Identity();

    Eigen::LLT<Eigen::Matrix<double,dim_state_cov,dim_state_cov> > choleskyDecompositionOfCov(P+Q); // apply Q befor the process model
    Eigen::Matrix<double,dim_state_cov,dim_state_cov> L = choleskyDecompositionOfCov.matrixL();

    for(int i = 0; i < dim_state_cov; i++){
        State noise = toFullState(std::sqrt(2*dim_state_cov) * L.col(i));

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
