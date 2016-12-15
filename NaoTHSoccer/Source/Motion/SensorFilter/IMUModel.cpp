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
    ukf.P = Eigen::Matrix<double,22,22>::Identity();

    // bias_acceleration (x,y,z) [m/s^2], too small?
    ukf.P(9,9) = 0.000001;
    ukf.P(10,10) = 0.000001;
    ukf.P(11,11) = 0.000001;

    // bias_rotational_velocity (x,y,z) [m/s^2], too small?
    ukf.P(19,19) = 0.000001;
    ukf.P(20,20) = 0.000001;
    ukf.P(21,21) = 0.000001;

    // set covariance matrix of process noise
    ukf.Q = Eigen::Matrix<double,22,22>::Identity();

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

    // rotation quaterion (x,y,z,w) [rad]?
    ukf.Q(12,12) = 0.001;
    ukf.Q(13,13) = 0.001;
    ukf.Q(14,14) = 0.001;

    // rotational_velocity (x,y,z) [rad/s], too small?
    ukf.Q(16,16) = 0.1;
    ukf.Q(17,17) = 0.1;
    ukf.Q(18,18) = 0.1;

    // bias_rotational_velocity (x,y,z) [m/s^2], too small?
    ukf.Q(19,19) = 0.000001;
    ukf.Q(20,20) = 0.000001;
    ukf.Q(21,21) = 0.000001;

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

    ukf.P = Eigen::Matrix<double,22,22>::Identity();

    // bias_acceleration (x,y,z) [m/s^2], too small?
    ukf.P(9,9) = 0.000001;
    ukf.P(10,10) = 0.000001;
    ukf.P(11,11) = 0.000001;

    // bias_rotational_velocity (x,y,z) [m/s^2], too small?
    ukf.P(19,19) = 0.000001;
    ukf.P(20,20) = 0.000001;
    ukf.P(21,21) = 0.000001;
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
    mean = lambda / (dim_state + lambda) * sigmaPoints.back();

    for(int i = 0; i < dim_state; i++){
        mean += 1/(2*(dim_state + lambda)) * sigmaPoints[i];
        mean += 1/(2*(dim_state + lambda)) * sigmaPoints[i + dim_state];
    }

    // calculate new process covariance
    Eigen::Matrix<double, dim_state, dim_state> cov;
    cov << (lambda / (dim_state + lambda) + (1 - alpha*alpha + beta) ) * (sigmaPoints.back() - mean) * (sigmaPoints.back() - mean).transpose();

    for(int i = 0; i < dim_state; i++){
        cov += 1/(2*(dim_state + lambda)) * (sigmaPoints[i] - mean)*(sigmaPoints[i] - mean).transpose();
        cov += 1/(2*(dim_state + lambda)) * (sigmaPoints[i + dim_state] - mean)*(sigmaPoints[i + dim_state] - mean).transpose();
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
    Measurement predicted_z = lambda / (dim_state + lambda) * sigmaMeasurements.back();
    for(int i = 0; i < dim_state; i++){
        predicted_z += 1/(2*(dim_state + lambda)) * sigmaMeasurements[i];
        predicted_z += 1/(2*(dim_state + lambda)) * sigmaMeasurements[i + dim_state];
    }

    // calculate current measurement covariance
    Eigen::Matrix<double, dim_measurement, dim_measurement> Pzz;
    Pzz << (lambda / (dim_state + lambda) + (1 - alpha*alpha + beta) ) * (sigmaMeasurements.back() - predicted_z) * (sigmaMeasurements.back() - predicted_z).transpose();

    for(int i = 0; i < dim_state; i++){
        Pzz += 1/(2*(dim_state + lambda)) * (sigmaMeasurements[i] - predicted_z) * (sigmaMeasurements[i] - predicted_z).transpose();
        Pzz += 1/(2*(dim_state + lambda)) * (sigmaMeasurements[i + dim_state] - predicted_z) * (sigmaMeasurements[i + dim_state] - predicted_z).transpose();
    }

    Pzz += R;

    // calculate state-measurement cross-covariance
    Eigen::Matrix<double, dim_state, dim_measurement> Pxz;
    Pxz << (lambda / (dim_state + lambda) + (1 - alpha*alpha + beta) ) * (sigmaPoints.back() - state) * (sigmaMeasurements.back() - predicted_z).transpose();

    for(int i = 0; i < dim_state; i++){
        Pxz += 1/(2*(dim_state + lambda)) * (sigmaPoints[i] - state) * (sigmaMeasurements[i] - predicted_z).transpose();
        Pxz += 1/(2*(dim_state + lambda)) * (sigmaPoints[i + dim_state] - state) * (sigmaMeasurements[i + dim_state] - predicted_z).transpose();
    }

    // calculate kalman gain
    Eigen::Matrix<double, dim_state, dim_measurement> K;
    K =Pxz*Pzz.inverse();

    // calculate new state and covariance
    state = state + K*(z - predicted_z);

    Eigen::Matrix<double,dim_state,dim_state> P_wiki;
    Eigen::Matrix<double,dim_state,dim_state> P_gh;
    P_wiki   = P - K*Pzz*K.transpose(); // https://en.m.wikipedia.org/wiki/Kalman_filter
    P_gh     = P - Pxz*Pzz.inverse()*Pxz.transpose(); // from "Performance and Implementation Aspects of Nonlinear Filtering" by Gustaf Hendeby

    P = P_wiki;
}

template <int dim_state, int dim_measurement>
void UKF<dim_state, dim_measurement>::transitionFunction(State& state, double dt){
    // rotational_velocity to quaterion
    double norm_rv = state.rotational_velocity().norm();
    Eigen::Quaterniond rotation_increment;
    if(norm_rv > 0) {
        rotation_increment = Eigen::Quaterniond (
                                std::cos(norm_rv/2*dt),
                                state.rotational_velocity()(0)/norm_rv*std::sin(norm_rv*dt/2),
                                state.rotational_velocity()(1)/norm_rv*std::sin(norm_rv*dt/2),
                                state.rotational_velocity()(2)/norm_rv*std::sin(norm_rv*dt/2));
    } else {
        // zero rotation quaterion
        rotation_increment = Eigen::Quaterniond(1,0,0,0);
    }

    Eigen::Quaterniond rotation = Eigen::Quaterniond(Eigen::Vector4d(state.rotation()));
    state.rotation() = (rotation_increment*rotation).coeffs(); // continue rotation assuming constant velocity

    // the other vectorial entities don't rotate with the sensor frame -> rotate in other direction
    rotation_increment = rotation_increment.inverse();

    state.acceleration() = rotation_increment._transformVector(state.acceleration());
    state.velocity()     = rotation_increment._transformVector(state.velocity());
    state.location()     = rotation_increment._transformVector(state.location());

    state.location() = state.location() + state.velocity()*dt + state.acceleration()*dt*dt/2;
    state.velocity() = state.velocity() + state.acceleration()*dt;
}

template <int dim_state, int dim_measurement>
typename UKF<dim_state, dim_measurement>::Measurement UKF<dim_state, dim_measurement>::stateToMeasurementSpaceFunction(State& state){

    // state to measurement function
    Eigen::Vector3d acceleration_in_measurement_space        = /*state.gravity() +*/ state.acceleration() + state.bias_acceleration();
    Eigen::Vector3d rotational_velocity_in_measurement_space = state.rotational_velocity() + state.bias_rotational_velocity();

    Measurement return_val;
    return_val << acceleration_in_measurement_space, rotational_velocity_in_measurement_space;

    return return_val;
}

template <int dim_state, int dim_measurement>
void UKF<dim_state, dim_measurement>::generateSigmaPoints(){
    sigmaPoints.resize(dim_state*2+1);

    sigmaPoints[dim_state*2] = state;

    // HACK: ensure positiv semidefinity of P
    // P = 0.5*P+0.5*P.transpose();
    // P = P + 10E-4*Eigen::Matrix<double,dim_state,dim_state>::Identity();

    Eigen::LLT<Eigen::Matrix<double,dim_state,dim_state> > choleskyDecompositionOfCov(P); // compute the Cholesky decomposition of A
    Eigen::Matrix<double,dim_state,dim_state> L = choleskyDecompositionOfCov.matrixL();
    double factor = std::sqrt(dim_state + lambda);
    for(int i = 0; i < dim_state; i++){
        sigmaPoints[i] = state + factor * L.col(i);
        sigmaPoints[i + dim_state] = state - factor * L.col(i);
    }
}


#pragma GCC diagnostic pop
