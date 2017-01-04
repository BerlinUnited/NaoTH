#include "IMUModel.h"

//TODO: remove pragma
#pragma GCC diagnostic ignored "-Wconversion"

IMUModel::IMUModel()
{
    // initial state
    ukf.state = Eigen::Matrix<double,22,1>::Zero();

    // assume gravity align with z axis
    //ukf.state.gravity()(2) = -9.81;

    // assume zero rotation
    ukf.state.rotation()(3,0) = 1;

    // initial state covariance
    ukf.P = Eigen::Matrix<double,21,21>::Identity();

    // bias_acceleration (x,y,z) [m/s^2], too small?
    ukf.P(9,9)   = 10e-10;
    ukf.P(10,10) = 10e-10;
    ukf.P(11,11) = 10e-10;

    // bias_rotational_velocity (x,y,z) [m/s^2], too small?
    ukf.P(18,18) = 10e-10;
    ukf.P(19,19) = 10e-10;
    ukf.P(20,20) = 10e-10;

    // set covariance matrix of process noise
    ukf.Q = Eigen::Matrix<double,21,21>::Identity();

    // location (x,y,z) [m]
    ukf.Q(0,0) = 0.0001;
    ukf.Q(1,1) = 0.0001;
    ukf.Q(2,2) = 0.0001;

    // velocity (x,y,z) [m/s]
    ukf.Q(3,3) = 0.001;
    ukf.Q(4,4) = 0.001;
    ukf.Q(5,5) = 0.001;

    // acceleration (x,y,z) [m/s^2]
    ukf.Q(6,6) = 0.01;
    ukf.Q(7,7) = 0.01;
    ukf.Q(8,8) = 0.01;

    // bias_acceleration (x,y,z) [m/s^2], too small?
    ukf.Q(9,9)   = 10e-10;
    ukf.Q(10,10) = 10e-10;
    ukf.Q(11,11) = 10e-10;

    // rotation (x,y,z) [rad]?
    ukf.Q(12,12) = 0.001;
    ukf.Q(13,13) = 0.001;
    ukf.Q(14,14) = 0.001;

    // rotational_velocity (x,y,z) [rad/s], too small?
    ukf.Q(15,15) = 0.1;
    ukf.Q(16,16) = 0.1;
    ukf.Q(17,17) = 0.1;

    // bias_rotational_velocity (x,y,z) [m/s^2], too small?
    ukf.Q(18,18) = 10e-10;
    ukf.Q(19,19) = 10e-10;
    ukf.Q(20,20) = 10e-10;

    // set covariance matrix of measurement noise
    ukf.R = Eigen::Matrix<double,6,6>();

    // measured covariance of acceleration and rotational velocity (motion log, 60 seconds)
    ukf.R << 5.074939351879890342e-04, -1.561730283237946278e-05,  1.012849085655689321e-04, -3.078687958578659292e-08, -1.132513004663809251e-06, -6.485352375515866273e-07,
            -1.561730283237946278e-05,  2.570436087068024501e-04, -4.159091012580820026e-05, -3.013278205585369588e-07,  1.736820285922189584e-06, -4.599219827687661978e-07,
             1.012849085655689321e-04, -4.159091012580820026e-05,  4.727921819788054878e-04,  5.523361976811979815e-07, -1.730307422507887473e-07, -3.030009469390110280e-07,
            -3.078687958578659292e-08, -3.013278205585369588e-07,  5.523361976811979815e-07,  3.434758685147043306e-06, -8.299226917536411892e-08,  5.842662059539863827e-08,
            -1.132513004663809251e-06,  1.736820285922189584e-06, -1.730307422507887473e-07, -8.299226917536411892e-08,  1.006052718494827880e-05,  1.346681994776136150e-06,
            -6.485352375515866273e-07, -4.599219827687661978e-07, -3.030009469390110280e-07,  5.842662059539863827e-08,  1.346681994776136150e-06,  3.242298821157115427e-06;

    DEBUG_REQUEST_REGISTER("IMUModel:reset_filter", "reset filter", false);
}

void IMUModel::execute(){
    DEBUG_REQUEST("IMUModel:reset_filter",
                  resetFilter();
    );

    ukf.generateSigmaPoints();
    ukf.predict(0.1);

    // don't generate sigma points again because the process noise would be applied a second time
    // ukf.generateSigmaPoints();

    Measurement z;
    z << getAccelerometerData().data.x, getAccelerometerData().data.y, getAccelerometerData().data.z, getGyrometerData().data.x, getGyrometerData().data.y, getGyrometerData().data.z/*, 9.81*/;

    ukf.update(z);

    writeIMUData();

    plots();
}

void IMUModel::resetFilter(){
    // initial state
    ukf.state = Eigen::Matrix<double,22,1>::Zero();

    // assume zero rotation
    ukf.state.rotation()(3,0) = 1;

    ukf.P = Eigen::Matrix<double,21,21>::Identity();

    // bias_acceleration (x,y,z) [m/s^2], too small?
    ukf.P(9,9)   = 10e-10;
    ukf.P(10,10) = 10e-10;
    ukf.P(11,11) = 10e-10;

    // bias_rotational_velocity (x,y,z) [m/s^2], too small?
    ukf.P(18,18) = 10e-10;
    ukf.P(19,19) = 10e-10;
    ukf.P(20,20) = 10e-10;
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

    getIMUData().bias_acceleration.x = ukf.state.bias_acceleration()(0,0);
    getIMUData().bias_acceleration.y = ukf.state.bias_acceleration()(1,0);
    getIMUData().bias_acceleration.z = ukf.state.bias_acceleration()(2,0);

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

    getIMUData().bias_rotational_velocity.x = ukf.state.bias_rotational_velocity()(0,0);
    getIMUData().bias_rotational_velocity.y = ukf.state.bias_rotational_velocity()(1,0);
    getIMUData().bias_rotational_velocity.z = ukf.state.bias_rotational_velocity()(2,0);

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

    PLOT("IMUModel:State:bias_acceleration:x", ukf.state.bias_acceleration()(0,0));
    PLOT("IMUModel:State:bias_acceleration:y", ukf.state.bias_acceleration()(1,0));
    PLOT("IMUModel:State:bias_acceleration:z", ukf.state.bias_acceleration()(2,0));

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

    PLOT("IMUModel:State:bias_rotational_velocity:x", ukf.state.bias_rotational_velocity()(0,0));
    PLOT("IMUModel:State:bias_rotational_velocity:y", ukf.state.bias_rotational_velocity()(1,0));
    PLOT("IMUModel:State:bias_rotational_velocity:z", ukf.state.bias_rotational_velocity()(2,0));

    PLOT("IMUModel:Measurement:rotational_velocity:x", getGyrometerData().data.x);
    PLOT("IMUModel:Measurement:rotational_velocity:y", getGyrometerData().data.y);
    PLOT("IMUModel:Measurement:rotational_velocity:z", getGyrometerData().data.z);
}

template <int dim_state, int dim_measurement>
void UKF<dim_state, dim_measurement>::predict(double dt){
    // transit the sigma points to the next state
    for (typename std::vector<State>::iterator i = sigmaPoints.begin(); i != sigmaPoints.end(); i++){
        transitionFunction(*i,dt);
    }

    // calculate new state (weighted mean of sigma points)
    State mean;
    mean = sigmaPoints.back();
    mean = 1.0 / sigmaPoints.size() * sigmaPoints.back();

    for(int i = 0; i < dim_state /*- 1*/; i++){
        mean += 1.0 / sigmaPoints.size() * sigmaPoints[i];
        mean += 1.0 / sigmaPoints.size() * sigmaPoints[i + dim_state - 1];
    }

    // more correct determination of the rotational mean
    // TODO: check whether this concept of averageing can be applied directly on the rotations (average axis and average angle)
    // TODO: adjust maximum of iterations
    mean.rotation() = sigmaPoints.back().rotation();
    for(int i = 0; i < 10; ++i){
        Eigen::Quaterniond mean_rotation;
        mean_rotation = Eigen::Quaterniond(Eigen::Vector4d(mean.rotation()));

        // calculate difference between the mean_rotation and the sigma points rotation by means of a rotation
        rotational_differences.clear();
        for (typename std::vector<State>::iterator i = sigmaPoints.begin(); i != sigmaPoints.end(); ++i){
            Eigen::AngleAxis<double> rotational_difference = Eigen::AngleAxis<double>(Eigen::Quaterniond(Eigen::Vector4d((*i).rotation())) * mean_rotation.inverse());

            // store as rotation vector representation
            rotational_differences.push_back(rotational_difference.angle() * rotational_difference.axis());
        }

        // average difference quaterions in their 3d vectorial representation (length = angle, direction = axis)
        Eigen::Vector3d averaged_rotational_difference = Eigen::Vector3d::Zero();
        for (typename std::vector<Eigen::Vector3d >::iterator i = rotational_differences.begin(); i != rotational_differences.end(); ++i){
            averaged_rotational_difference += *i;
        }
        averaged_rotational_difference = 1/rotational_differences.size() * averaged_rotational_difference;

        if(averaged_rotational_difference.norm() < 10e-4){
            break;
        } else {
            // rotate current mean by the averaged rotational difference quaternion in direction to the true mean
            mean.rotation() = (Eigen::Quaterniond(Eigen::AngleAxis<double>(averaged_rotational_difference.norm(), averaged_rotational_difference.normalized())) * mean_rotation).coeffs();
        }
    }

    // calculate new process covariance
    Eigen::Matrix<double, dim_state-1,1> m_compatible = mean.toCovarianceCompatibleState();
    Eigen::Matrix<double, dim_state-1,1> temp;

    temp = sigmaPoints.back().toCovarianceCompatibleState() - m_compatible;
    // replace wrong rotational difference with the difference from the mean calculation (above)
    temp.block(12,0,3,1) = rotational_differences.back();

    Eigen::Matrix<double, dim_state-1, dim_state-1> cov;
    cov << 1.0 / sigmaPoints.size()  * (temp) * (temp).transpose();

    for(int i = 0; i < dim_state-1; i++){
        temp = sigmaPoints[i].toCovarianceCompatibleState() - m_compatible;
        // replace wrong rotational difference with the difference from the mean calculation (above)
        temp.block(12,0,3,1) = rotational_differences[i];
        cov += 1.0 / sigmaPoints.size() * (temp)*(temp).transpose();

        temp = sigmaPoints[i + dim_state - 1].toCovarianceCompatibleState() - m_compatible;
        // replace wrong rotational difference with the difference from the mean calculation (above)
        temp.block(12,0,3,1) = rotational_differences[i + dim_state - 1];
        cov += 1.0 / sigmaPoints.size() * (temp)*(temp).transpose();
    }

    state = mean;
    P     = cov/* + Q*/; // process covariance is applied before the process model (while generating the sigma points)
}

template <int dim_state, int dim_measurement>
void UKF <dim_state, dim_measurement>::update(Measurement z){
    std::vector<Measurement> sigmaMeasurements;

    // map sigma points to measurement space
    for (typename std::vector<State>::iterator i = sigmaPoints.begin(); i != sigmaPoints.end(); i++){
        sigmaMeasurements.push_back(stateToMeasurementSpaceFunction(*i));
    }

    // calculate predicted measurement z (weighted mean of sigma points)
    Measurement predicted_z = 1.0 / sigmaPoints.size() * sigmaMeasurements.back();
    for(int i = 0; i < dim_state - 1; i++){
        predicted_z += 1.0 / sigmaPoints.size() * sigmaMeasurements[i];
        predicted_z += 1.0 / sigmaPoints.size() * sigmaMeasurements[i + dim_state];
    }

    // calculate current measurement covariance
    Eigen::Matrix<double, dim_measurement, dim_measurement> Pzz;
    Pzz << 1.0 / sigmaPoints.size() * (sigmaMeasurements.back() - predicted_z) * (sigmaMeasurements.back() - predicted_z).transpose();

    for(int i = 0; i < dim_state - 1; i++){
        Pzz += 1.0 / sigmaPoints.size() * (sigmaMeasurements[i] - predicted_z) * (sigmaMeasurements[i] - predicted_z).transpose();
        Pzz += 1.0 / sigmaPoints.size()  * (sigmaMeasurements[i + dim_state] - predicted_z) * (sigmaMeasurements[i + dim_state] - predicted_z).transpose();
    }

    // apply measurement noise covariance
    Pzz += R;

    // calculate state-measurement cross-covariance
    Eigen::Matrix<double, dim_state-1, dim_measurement> Pxz;

    Eigen::Matrix<double, dim_state-1,1> state_compatible = state.toCovarianceCompatibleState();
    Eigen::Quaterniond mean_rotation = Eigen::Quaterniond(Eigen::Vector4d(state.rotation()));
    Eigen::Matrix<double, dim_state-1,1> temp;

    temp = sigmaPoints.back().toCovarianceCompatibleState() - state_compatible;
    // replace wrong rotational difference
    Eigen::AngleAxis<double> e = Eigen::AngleAxis<double>(Eigen::Quaterniond(Eigen::Vector4d(state.rotation())) * mean_rotation.inverse());
    temp.block(12,0,3,1) = e.angle() * e.axis();

    Pxz << 1.0 / sigmaPoints.size() * (temp) * (sigmaMeasurements.back() - predicted_z).transpose();

    for(int i = 0; i < dim_state - 1; i++){
        temp = sigmaPoints[i].toCovarianceCompatibleState() - state_compatible;
        // replace wrong rotational difference
        Eigen::AngleAxis<double> e = Eigen::AngleAxis<double>(Eigen::Quaterniond(Eigen::Vector4d(sigmaPoints[i].rotation())) * mean_rotation.inverse());
        temp.block(12,0,3,1) = e.angle() * e.axis();

        Pxz += 1.0 / sigmaPoints.size() * (temp) * (sigmaMeasurements[i] - predicted_z).transpose();

        temp = sigmaPoints[i + dim_state - 1].toCovarianceCompatibleState() - state_compatible;
        // replace wrong rotational difference
        e = Eigen::AngleAxis<double>(Eigen::Quaterniond(Eigen::Vector4d(sigmaPoints[i + dim_state - 1].rotation())) * mean_rotation.inverse());
        temp.block(12,0,3,1) = e.angle() * e.axis();

        Pxz += 1.0 / sigmaPoints.size() * (temp) * (sigmaMeasurements[i + dim_state - 1] - predicted_z).transpose();
    }

    // calculate kalman gain
    Eigen::Matrix<double, dim_state - 1, dim_measurement> K;
    K =Pxz*Pzz.inverse();

    // calculate new state and covariance
    State state_innovation = toFullState(K*(z - predicted_z));

    state += state_innovation;

    Eigen::Matrix<double,dim_state - 1,dim_state - 1> P_wiki;
    //Eigen::Matrix<double,dim_state,dim_state> P_gh;
    P_wiki   = P - K*Pzz*K.transpose(); // https://en.m.wikipedia.org/wiki/Kalman_filter
    //P_gh     = P - Pxz*Pzz.inverse()*Pxz.transpose(); // from "Performance and Implementation Aspects of Nonlinear Filtering" by Gustaf Hendeby

    P = P_wiki;
}

template <int dim_state, int dim_measurement>
void UKF<dim_state, dim_measurement>::transitionFunction(State& state, double dt){
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

template <int dim_state, int dim_measurement>
typename UKF<dim_state, dim_measurement>::Measurement UKF<dim_state, dim_measurement>::stateToMeasurementSpaceFunction(State& state){
    Eigen::Quaterniond rotation = Eigen::Quaterniond(Eigen::Vector4d(state.rotation()));

    // state to measurement function
    // transform acceleration part of the state into local measurement space (the robot's body frame = frame of accelerometer), the bias is already in this frame
    Eigen::Vector3d acceleration_in_measurement_space        = rotation.inverse()._transformVector(state.acceleration()) + state.bias_acceleration(); // TODO: check for rotation._transformVector(...) ...
    Eigen::Vector3d rotational_velocity_in_measurement_space = state.rotational_velocity() + state.bias_rotational_velocity();

    Measurement return_val;
    return_val << acceleration_in_measurement_space, rotational_velocity_in_measurement_space;

    return return_val;
}

template <int dim_state, int dim_measurement>
void UKF<dim_state, dim_measurement>::generateSigmaPoints(){
    sigmaPoints.resize((dim_state-1)*2+1);

    sigmaPoints[(dim_state-1)*2] = state;

    // HACK: ensure positiv semidefinity of P
    // P = 0.5*P+0.5*P.transpose();
    // P = P + 10E-4*Eigen::Matrix<double,dim_state,dim_state>::Identity();

    Eigen::LLT<Eigen::Matrix<double,dim_state-1,dim_state-1> > choleskyDecompositionOfCov(P+Q); // apply Q befor the process model
    Eigen::Matrix<double,dim_state-1,dim_state-1> L = choleskyDecompositionOfCov.matrixL();

    for(int i = 0; i < dim_state-1; i++){
        State noise = toFullState(std::sqrt(2*(dim_state-1)) * L.col(i));

        // TODO: check which order is correct (matters because of the rotation part), order as used in the paper
        sigmaPoints[i]  = noise;
        sigmaPoints[i] += state;

        // generate "opposite" sigma point
        // TODO: check which order is correct (matters because of the rotation part), order as used in the paper
        sigmaPoints[i + dim_state - 1]  = -noise;
        sigmaPoints[i + dim_state - 1] += state;
    }
}


#pragma GCC diagnostic pop
