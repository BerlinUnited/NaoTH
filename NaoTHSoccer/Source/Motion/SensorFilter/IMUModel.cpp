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
    ukf.state.rotation()(3) = 1;

    // initial state covariance
    ukf.P = Eigen::Matrix<double,21,21>::Identity();

    // bias_acceleration (x,y,z) [m/s^2], too small?
    ukf.P(9,9) = 0.000001;
    ukf.P(10,10) = 0.000001;
    ukf.P(11,11) = 0.000001;

    // bias_rotational_velocity (x,y,z) [m/s^2], too small?
    ukf.P(18,18) = 0.000001;
    ukf.P(19,19) = 0.000001;
    ukf.P(20,20) = 0.000001;

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
    ukf.Q(9,9) = 0.000001;
    ukf.Q(10,10) = 0.000001;
    ukf.Q(11,11) = 0.000001;

    // rotation (x,y,z) [rad]?
    ukf.Q(12,12) = 0.001;
    ukf.Q(13,13) = 0.001;
    ukf.Q(14,14) = 0.001;

    // rotational_velocity (x,y,z) [rad/s], too small?
    ukf.Q(15,15) = 0.1;
    ukf.Q(16,16) = 0.1;
    ukf.Q(17,17) = 0.1;

    // bias_rotational_velocity (x,y,z) [m/s^2], too small?
    ukf.Q(18,18) = 0.000001;
    ukf.Q(19,19) = 0.000001;
    ukf.Q(20,20) = 0.000001;

    // set covariance matrix of measurement noise
    // TODO: determine experimentally
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

    ukf.generateSigmaPoints();

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
    ukf.state.rotation()(3) = 1;

    ukf.P = Eigen::Matrix<double,21,21>::Identity();

    // bias_acceleration (x,y,z) [m/s^2], too small?
    ukf.P(9,9) = 0.000001;
    ukf.P(10,10) = 0.000001;
    ukf.P(11,11) = 0.000001;

    // bias_rotational_velocity (x,y,z) [m/s^2], too small?
    ukf.P(18,18) = 0.000001;
    ukf.P(19,19) = 0.000001;
    ukf.P(20,20) = 0.000001;
}

void IMUModel::writeIMUData(){
    getIMUData().location.x = ukf.state.location()(0);
    getIMUData().location.y = ukf.state.location()(1);
    getIMUData().location.z = ukf.state.location()(2);

    getIMUData().velocity.x = ukf.state.velocity()(0);
    getIMUData().velocity.y = ukf.state.velocity()(1);
    getIMUData().velocity.z = ukf.state.velocity()(2);

    getIMUData().acceleration.x = ukf.state.acceleration()(0);
    getIMUData().acceleration.y = ukf.state.acceleration()(1);
    getIMUData().acceleration.z = ukf.state.acceleration()(2);

    getIMUData().acceleration_sensor = getAccelerometerData().data;

    getIMUData().bias_acceleration.x = ukf.state.bias_acceleration()(0);
    getIMUData().bias_acceleration.y = ukf.state.bias_acceleration()(1);
    getIMUData().bias_acceleration.z = ukf.state.bias_acceleration()(2);

    Eigen::Quaterniond q;
    q = Eigen::Quaterniond(Eigen::Vector4d(ukf.state.rotation()));
    Eigen::Vector3d angles = q.toRotationMatrix().eulerAngles(0,1,2);
    getIMUData().rotation.x = angles(0);
    getIMUData().rotation.y = angles(1);
    getIMUData().rotation.z = angles(2);

    getIMUData().rotational_velocity.x = ukf.state.rotational_velocity()(0);
    getIMUData().rotational_velocity.y = ukf.state.rotational_velocity()(1);
    getIMUData().rotational_velocity.z = ukf.state.rotational_velocity()(2);

    getIMUData().rotational_velocity_sensor = getGyrometerData().data;

    getIMUData().bias_rotational_velocity.x = ukf.state.bias_rotational_velocity()(0);
    getIMUData().bias_rotational_velocity.y = ukf.state.bias_rotational_velocity()(1);
    getIMUData().bias_rotational_velocity.z = ukf.state.bias_rotational_velocity()(2);
}

void IMUModel::plots(){
    PLOT("IMUModel:State:location:x", ukf.state.location()(0));
    PLOT("IMUModel:State:location:y", ukf.state.location()(1));
    PLOT("IMUModel:State:location:z", ukf.state.location()(2));

    PLOT("IMUModel:State:velocity:x", ukf.state.velocity()(0));
    PLOT("IMUModel:State:velocity:y", ukf.state.velocity()(1));
    PLOT("IMUModel:State:velocity:z", ukf.state.velocity()(2));

    PLOT("IMUModel:State:acceleration:x", ukf.state.acceleration()(0));
    PLOT("IMUModel:State:acceleration:y", ukf.state.acceleration()(1));
    PLOT("IMUModel:State:acceleration:z", ukf.state.acceleration()(2));

    PLOT("IMUModel:Measurement:acceleration:x", getAccelerometerData().data.x);
    PLOT("IMUModel:Measurement:acceleration:y", getAccelerometerData().data.y);
    PLOT("IMUModel:Measurement:acceleration:z", getAccelerometerData().data.z);

    Eigen::Quaterniond q;
    q = Eigen::Quaterniond(Eigen::Vector4d(ukf.state.rotation()));
    Eigen::Vector3d angles = q.toRotationMatrix().eulerAngles(0,1,2);
    PLOT("IMUModel:State:rotation:x", angles(0));
    PLOT("IMUModel:State:rotation:y", angles(1));
    PLOT("IMUModel:State:rotation:z", angles(2));

    PLOT("IMUModel:State:rotational_velocity:x", ukf.state.rotational_velocity()(0));
    PLOT("IMUModel:State:rotational_velocity:y", ukf.state.rotational_velocity()(1));
    PLOT("IMUModel:State:rotational_velocity:z", ukf.state.rotational_velocity()(2));

    PLOT("IMUModel:Measurement:rotational_velocity:x", getGyrometerData().data.x);
    PLOT("IMUModel:Measurement:rotational_velocity:y", getGyrometerData().data.y);
    PLOT("IMUModel:Measurement:rotational_velocity:z", getGyrometerData().data.z);
}

//template <int dim_state, int dim_measurement>
//UKF<dim_state, dim_measurement>::UKF<dim_state, dim_measurement>(){

//}

template <int dim_state, int dim_measurement>
void UKF<dim_state, dim_measurement>::predict(double dt){
    // transit the sigma points to the next state
    for (typename std::vector<State>::iterator i = sigmaPoints.begin(); i != sigmaPoints.end(); i++){
        transitionFunction(*i,dt);
    }

    // calculate new state (weighted mean of sigma points)
    State mean;
    mean = lambda / (dim_state /*- 1*/ + lambda) * sigmaPoints.back();

    for(int i = 0; i < dim_state /*- 1*/; i++){
        mean += 1/(2*(dim_state /*- 1*/ + lambda)) * sigmaPoints[i];
        mean += 1/(2*(dim_state /*- 1*/ + lambda)) * sigmaPoints[i + dim_state - 1];
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
    // replace wrong rotational difference with the difference of mean calculation (above)
    temp.block(12,0,3,1) = rotational_differences.back();

    Eigen::Matrix<double, dim_state-1, dim_state-1> cov;
    cov << (lambda / (dim_state - 1 + lambda) + (1 - alpha*alpha + beta) ) * (temp) * (temp).transpose();

    for(int i = 0; i < dim_state-1; i++){
        temp = sigmaPoints[i].toCovarianceCompatibleState() - m_compatible;
        temp.block(12,0,3,1) = rotational_differences[i];
        cov += 1/(2*(dim_state - 1 + lambda)) * (temp)*(temp).transpose();

        temp = sigmaPoints[i + dim_state - 1].toCovarianceCompatibleState() - m_compatible;
        temp.block(12,0,3,1) = rotational_differences[i + dim_state - 1];
        cov += 1/(2*(dim_state - 1 + lambda)) * (temp)*(temp).transpose();
    }

    state = mean;
    P     = cov + Q;
}

template <int dim_state, int dim_measurement>
void UKF <dim_state, dim_measurement>::update(Measurement z){
    std::vector<Measurement> sigmaMeasurements;

    // map sigma points to measurement space
    for (typename std::vector<State>::iterator i = sigmaPoints.begin(); i != sigmaPoints.end(); i++){
        sigmaMeasurements.push_back(stateToMeasurementSpaceFunction(*i));
    }

    // calculate predicted measurement z (weighted mean of sigma points)
    Measurement predicted_z = lambda / (dim_state - 1 + lambda) * sigmaMeasurements.back();
    for(int i = 0; i < dim_state - 1; i++){
        predicted_z += 1/(2*(dim_state - 1 + lambda)) * sigmaMeasurements[i];
        predicted_z += 1/(2*(dim_state - 1 + lambda)) * sigmaMeasurements[i + dim_state];
    }

    // calculate current measurement covariance
    Eigen::Matrix<double, dim_measurement, dim_measurement> Pzz;
    Pzz << (lambda / (dim_state - 1 + lambda) + (1 - alpha*alpha + beta) ) * (sigmaMeasurements.back() - predicted_z) * (sigmaMeasurements.back() - predicted_z).transpose();

    for(int i = 0; i < dim_state - 1; i++){
        Pzz += 1/(2*(dim_state - 1 + lambda)) * (sigmaMeasurements[i] - predicted_z) * (sigmaMeasurements[i] - predicted_z).transpose();
        Pzz += 1/(2*(dim_state - 1 + lambda)) * (sigmaMeasurements[i + dim_state] - predicted_z) * (sigmaMeasurements[i + dim_state] - predicted_z).transpose();
    }

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

    Pxz << (lambda / (dim_state + lambda) + (1 - alpha*alpha + beta) ) * (temp) * (sigmaMeasurements.back() - predicted_z).transpose();

    for(int i = 0; i < dim_state - 1; i++){
        temp = sigmaPoints[i].toCovarianceCompatibleState() - state_compatible;
        // replace wrong rotational difference
        Eigen::AngleAxis<double> e = Eigen::AngleAxis<double>(Eigen::Quaterniond(Eigen::Vector4d(sigmaPoints[i].rotation())) * mean_rotation.inverse());
        temp.block(12,0,3,1) = e.angle() * e.axis();

        Pxz += 1/(2*(dim_state - 1 + lambda)) * (temp) * (sigmaMeasurements[i] - predicted_z).transpose();

        temp = sigmaPoints[i + dim_state - 1].toCovarianceCompatibleState() - state_compatible;
        // replace wrong rotational difference
        e = Eigen::AngleAxis<double>(Eigen::Quaterniond(Eigen::Vector4d(sigmaPoints[i + dim_state - 1].rotation())) * mean_rotation.inverse());
        temp.block(12,0,3,1) = e.angle() * e.axis();

        Pxz += 1/(2*(dim_state - 1 + lambda)) * (temp) * (sigmaMeasurements[i + dim_state - 1] - predicted_z).transpose();
    }

    // calculate kalman gain
    Eigen::Matrix<double, dim_state - 1, dim_measurement> K;
    K =Pxz*Pzz.inverse();

    // calculate new state and covariance
    State state_innovation = toFullState(K*(z - predicted_z));
    //state = state + state_innovation;
    state.location()          += state_innovation.location();
    state.velocity()          += state_innovation.velocity();
    state.acceleration()      += state_innovation.acceleration();
    state.bias_acceleration() += state_innovation.bias_acceleration();

    state.rotation()                  = state_innovation.rotation() * state.rotation();
    state.rotational_velocity()      += state_innovation.rotational_velocity();
    state.bias_rotational_velocity() += state_innovation.bias_rotational_velocity();

    Eigen::Matrix<double,dim_state - 1,dim_state - 1> P_wiki;
    //Eigen::Matrix<double,dim_state,dim_state> P_gh;
    P_wiki   = P - K*Pzz*K.transpose(); // https://en.m.wikipedia.org/wiki/Kalman_filter
    //P_gh     = P - Pxz*Pzz.inverse()*Pxz.transpose(); // from "Performance and Implementation Aspects of Nonlinear Filtering" by Gustaf Hendeby

    P = P_wiki;
}

template <int dim_state, int dim_measurement>
void UKF<dim_state, dim_measurement>::transitionFunction(State& state, double dt){
    // rotational_velocity to quaternion
    double norm_rv = state.rotational_velocity().norm();
    Eigen::Quaterniond rotation_increment;
    if(norm_rv > 0) {
        rotation_increment = Eigen::Quaterniond (
                                std::cos(norm_rv/2*dt),
                                state.rotational_velocity()(0)/norm_rv*std::sin(norm_rv*dt/2),
                                state.rotational_velocity()(1)/norm_rv*std::sin(norm_rv*dt/2),
                                state.rotational_velocity()(2)/norm_rv*std::sin(norm_rv*dt/2));
    } else {
        // zero rotation quaternion
        rotation_increment = Eigen::Quaterniond(1,0,0,0);
    }

    Eigen::Quaterniond rotation = Eigen::Quaterniond(Eigen::Vector4d(state.rotation()));
    // continue rotation assuming constant velocity
    state.rotation() = (rotation_increment*rotation).coeffs();

    // linear motion model
    state.location() = state.location() + state.velocity()*dt + state.acceleration()*dt*dt/2;
    state.velocity() = state.velocity() + state.acceleration()*dt;
}

template <int dim_state, int dim_measurement>
typename UKF<dim_state, dim_measurement>::Measurement UKF<dim_state, dim_measurement>::stateToMeasurementSpaceFunction(State& state){

    Eigen::Quaterniond rotation = Eigen::Quaterniond(Eigen::Vector4d(state.rotation()));

    // state to measurement function
    // transform acceleration part of the state into local measurement space (the robot's body frame = frame of accelerometer), the bias is already in this frame
    Eigen::Vector3d acceleration_in_measurement_space        = rotation._transformVector(state.acceleration()) + state.bias_acceleration();
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

    Eigen::LLT<Eigen::Matrix<double,dim_state-1,dim_state-1> > choleskyDecompositionOfCov(P/*+Q*/); // apply Q befor the transistion
    Eigen::Matrix<double,dim_state-1,dim_state-1> L = choleskyDecompositionOfCov.matrixL();
    //double factor = std::sqrt(dim_state + lambda);
    for(int i = 0; i < dim_state-1; i++){

        Eigen::Quaterniond rotation = Eigen::Quaterniond(Eigen::Vector4d(state.rotation()));

        sigmaPoints[i] = state;
        Eigen::Matrix<double, dim_state-1,1> noisy_part = /* /* factor * */ */ L.col(i);

        // add noise to linear motion components
        sigmaPoints[i].location()          += /* factor * */ noisy_part.block(0,0,3,1);
        sigmaPoints[i].velocity()          += /* factor * */ noisy_part.block(3,0,3,1);
        sigmaPoints[i].acceleration()      += /* factor * */ noisy_part.block(6,0,3,1);
        sigmaPoints[i].bias_acceleration() += /* factor * */ noisy_part.block(9,0,3,1);

        // correct incorporation of the noisy rotation
        // TODO: how to use factor?
        // 1) determine noise quaternion
        double norm_rv = noisy_part.block(12,0,3,1).norm();
        Eigen::Quaterniond noisey_quaternion;
        if(norm_rv > 0) {
            noisey_quaternion = Eigen::Quaterniond (
                        std::cos(norm_rv/2),
                        noisy_part(12,0)/norm_rv * std::sin(norm_rv/2),
                        noisy_part(13,0)/norm_rv * std::sin(norm_rv/2),
                        noisy_part(14,0)/norm_rv * std::sin(norm_rv/2));
        } else {
            // zero rotation quaternion
            noisey_quaternion = Eigen::Quaterniond(1,0,0,0);
        }

        // "add" noisy rotation to current rotation
        sigmaPoints[i].rotation() = (rotation * noisey_quaternion).coeffs();

        // add noise to rotational velocity and bias
        sigmaPoints[i].rotational_velocity()      += /* factor * */ noisy_part.block(15,0,3,1);
        sigmaPoints[i].bias_rotational_velocity() += /* factor * */ noisy_part.block(18,0,3,1);


        // generate "opposite" sigma point
        sigmaPoints[i + dim_state - 1] = state;

        sigmaPoints[i + dim_state - 1].location()          -= /* factor * */ noisy_part.block(0,0,3,1);
        sigmaPoints[i + dim_state - 1].velocity()          -= /* factor * */ noisy_part.block(3,0,3,1);
        sigmaPoints[i + dim_state - 1].acceleration()      -= /* factor * */ noisy_part.block(6,0,3,1);
        sigmaPoints[i + dim_state - 1].bias_acceleration() -= /* factor * */ noisy_part.block(9,0,3,1);

        sigmaPoints[i + dim_state - 1].rotation() = (rotation * noisey_quaternion.inverse()).coeffs();

        sigmaPoints[i + dim_state - 1].rotational_velocity()      -= /* factor * */ noisy_part.block(15,0,3,1);
        sigmaPoints[i + dim_state - 1].bias_rotational_velocity() -= /* factor * */ noisy_part.block(18,0,3,1);
    }
}


#pragma GCC diagnostic pop
