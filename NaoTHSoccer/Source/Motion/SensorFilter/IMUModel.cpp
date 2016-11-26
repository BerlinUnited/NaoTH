#include "IMUModel.h"

//TODO: remove pragma
#pragma GCC diagnostic ignored "-Wconversion"

IMUModel::IMUModel()
{
    // initial state
    ukf.state = Eigen::Matrix<double,15,1>::Zero();

    // assume gravity align with z axis
    ukf.state.gravity()(2) = -9.81;

    ukf.P = Eigen::Matrix<double,15,15>::Identity();

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
    z << getAccelerometerData().data.x, getAccelerometerData().data.y, getAccelerometerData().data.z/*, 9.81*//*, getGyrometerData().data.x, getGyrometerData().data.y, getGyrometerData().data.z*/;

    ukf.update(z);

    writeIMUData();
}

void IMUModel::resetFilter(){
    // initial state
    ukf.state = Eigen::Matrix<double,15,1>::Zero();

    // assume gravity align with z axis
    ukf.state.gravity()(2) = -9.81;

    ukf.P = Eigen::Matrix<double,15,15>::Identity();
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

    getIMUData().bias_acceleration.x = ukf.state.bias_acceleration()(0);
    getIMUData().bias_acceleration.y = ukf.state.bias_acceleration()(1);
    getIMUData().bias_acceleration.z = ukf.state.bias_acceleration()(2);

    getIMUData().gravity.x = ukf.state.gravity()(0);
    getIMUData().gravity.y = ukf.state.gravity()(1);
    getIMUData().gravity.z = ukf.state.gravity()(2);
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
    P     = cov;
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
    P_wiki     = P - K*Pzz*K.transpose(); // https://en.m.wikipedia.org/wiki/Kalman_filter
    P_gh     = P - Pxz*Pzz.inverse()*Pxz.transpose(); // from "Performance and Implementation Aspects of Nonlinear Filtering" by Gustaf Hendeby

    P = P_wiki;
}

template <int dim_state, int dim_measurement>
void UKF<dim_state, dim_measurement>::transitionFunction(State& state, double dt){
    // rotational_velocity to quaterion
//    double norm_rv = state.rotational_velocity().norm();

//    Eigen::Quaterniond rotation_increment(
//                            std::cos(norm_rv/2*dt),
//                            state.rotational_velocity()(0)/norm_rv*std::sin(norm_rv*dt/2),
//                            state.rotational_velocity()(1)/norm_rv*std::sin(norm_rv*dt/2),
//                            state.rotational_velocity()(2)/norm_rv*std::sin(norm_rv*dt/2));

//    state.rotation() = rotation_increment*state.rotation(); // continue rotation assuming constant velocity

//    // the other vectorial entities don't rotate with the sensor frame -> rotate in other direction
//    rotation_increment = rotation_increment.inverse();

//    state.gravity()      = rotation_increment._transformVector(state.gravity());
//    state.acceleration() = rotation_increment._transformVector(state.acceleration());
//    state.velocity()     = rotation_increment._transformVector(state.velocity());
//    state.location()     = rotation_increment._transformVector(state.location());

    state.location() = state.location() + state.velocity()*dt + state.acceleration()*dt*dt/2;
    state.velocity() = state.velocity() + state.acceleration()*dt;
}

template <int dim_state, int dim_measurement>
typename UKF<dim_state, dim_measurement>::Measurement UKF<dim_state, dim_measurement>::stateToMeasurementSpaceFunction(State& state){

    // state to measurement function
    Eigen::Vector3d acceleration_in_measurement_space        = state.gravity() + state.acceleration() + state.bias_acceleration();
    //Eigen::Vector3d rotational_velocity_in_measurement_space = state.rotational_velocity() + state.bias_rotational_velocity();

    Measurement return_val;
    return_val << acceleration_in_measurement_space/*, 9.81*//*, state.gravity().norm()*//*, rotational_velocity_in_measurement_space*/;

    return return_val;
}

template <int dim_state, int dim_measurement>
void UKF<dim_state, dim_measurement>::generateSigmaPoints(){
    sigmaPoints.resize(dim_state*2+1);

    sigmaPoints[dim_state*2] = state;

    // HACK: ensure positiv semidefinity of P
    // P = 0.5*P+0.5*P.transpose();
    // P = P + 10E-4*Eigen::Matrix<double,dim_state,dim_state>::Identity();

//    Eigen::LDLT<Eigen::Matrix<double,dim_state,dim_state> > choleskyDecompositionOfCov(P); // compute the Cholesky decomposition of A
//    Eigen::Matrix<double,dim_state,dim_state> L = choleskyDecompositionOfCov.matrixL();
//    Eigen::Matrix<double,dim_state,dim_state> D = choleskyDecompositionOfCov.vectorD().asDiagonal();
//    D = D.array().sqrt();
//    Eigen::Matrix<double,dim_state,dim_state> sqrtP = L * D * L.transpose();

    Eigen::LLT<Eigen::Matrix<double,dim_state,dim_state> > choleskyDecompositionOfCov(P); // compute the Cholesky decomposition of A
    Eigen::Matrix<double,dim_state,dim_state> L = choleskyDecompositionOfCov.matrixL();
    double factor = std::sqrt(dim_state + lambda);
    for(int i = 0; i < dim_state; i++){
        sigmaPoints[i] = state + factor * L.col(i);
        sigmaPoints[i + dim_state] = state - factor * L.col(i);
    }
}


#pragma GCC diagnostic pop
