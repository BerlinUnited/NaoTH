#include "ExtendedKalmanFilter4d.h"

ExtendedKalmanFilter4d::ExtendedKalmanFilter4d(const Eigen::Vector4d& state, const Eigen::Matrix2d& processNoiseStdSingleDimension, const Eigen::Matrix2d& measurementNoiseStd, const Eigen::Matrix2d& initialStateStdSingleDimension):
    x(state)
{
    Eigen::Matrix2d q;
    q = processNoiseStdSingleDimension.cwiseProduct(processNoiseStdSingleDimension);

    // covariance matrix of process noise (values taken from old kalman filter)
    Q << q, Eigen::Matrix2d::Zero(), Eigen::Matrix2d::Zero(), q;

    Eigen::Matrix2d r;
    r = measurementNoiseStd.cwiseProduct(measurementNoiseStd);

    // covariance matrix of measurement noise (values taken from old kalman filter)
    R << r;

    // inital covariance matrix of current state (values taken from old kalman filter)
    //double p = 62500;
    Eigen::Matrix2d p;
    p = initialStateStdSingleDimension.cwiseProduct(initialStateStdSingleDimension);
    P << p, Eigen::Matrix2d::Zero(), Eigen::Matrix2d::Zero(), p;

    // state to measurement transformation matrix
    H << 1, 0, 0, 0,
         0, 0, 1, 0;
}

ExtendedKalmanFilter4d::~ExtendedKalmanFilter4d()
{

}

void ExtendedKalmanFilter4d::prediction(const Eigen::Vector2d& u, double /*dt*/)
{
    // adapt state transition matrix
    /*F << 1, dt, 0, 0,
         0,  1, 0, 0,
         0,  0, 1, dt,
         0,  0, 0, 1;*/

    F = Eigen::Matrix4d::Identity();

    // adapt control matrix to dt
    /*B << dt*dt/2, 0,
         dt     , 0,
         0      , dt*dt/2,
         0      , dt;*/

    B << 0, 0,
            0     , 0,
            0      , 0,
            0      , 0;

    // predict
    x_pre = F * x + B * u;
    P_pre = F * P * F.transpose() + Q;

    x = x_pre;
    P = P_pre;
}

void ExtendedKalmanFilter4d::update(const Eigen::Vector2d& z)
{
    H << 1, 0, 0, 0,
         0, 0, 1, 0;

    Eigen::Matrix2d temp1 = H * P_pre * H.transpose() + R;

    K = P_pre * H.transpose() * temp1.inverse();

    x_corr = x_pre + K * (z - H * x_pre );

    P_corr = P_pre - K*H*P_pre;

    x = x_corr;
    P = P_corr;
}


void ExtendedKalmanFilter4d::extendedUpdate(const Eigen::Vector2d& z, const double height)
{
    Eigen::Vector2d predicted_measurement;
    const double x_state = x_pre(0);
    const double y_state = x_pre(2);
    const double d = std::sqrt(x_state*x_state + y_state*y_state);

    H << height*x_state/(d*(height*height+d*d)), 0, height*y_state/(d*(height*height+d*d)), 0,
                                 -y_state/(d*d), 0, x_state/(d*d)                        , 0;

    predicted_measurement << std::atan2(height,d),
                             std::atan2(y_state, x_state);

    Eigen::Matrix2d temp1 = H * P_pre * H.transpose() + R;

    K = P_pre * H.transpose() * temp1.inverse();

    x_corr = x_pre + K * (z - predicted_measurement);

    //P_corr = P_pre - K*H*P_pre;

    P_corr = (Eigen::Matrix4d::Identity()-K*H)*P_pre;

    x = x_corr;
    P = P_corr;
}

void ExtendedKalmanFilter4d::setState(Eigen::Vector4d& state)
{
    x = state;
}

void ExtendedKalmanFilter4d::setCovarianceOfProcessNoise(const Eigen::Matrix2d& q){
    Q << q, Eigen::Matrix2d::Zero(), Eigen::Matrix2d::Zero(), q;
}

void ExtendedKalmanFilter4d::setCovarianceOfMeasurementNoise(const Eigen::Matrix2d& r){
    R << r;
}

const Eigen::Vector4d& ExtendedKalmanFilter4d::getState() const
{
    return x;
}

const Eigen::Matrix4d& ExtendedKalmanFilter4d::getProcessCovariance() const
{
    return P;
}

const Eigen::Matrix2d& ExtendedKalmanFilter4d::getMeasurementCovariance() const
{
    return R;
}

Eigen::Vector2d ExtendedKalmanFilter4d::getStateInMeasurementSpace() const {

    Eigen::Matrix<double, 2, 4> H;
    H << 1, 0, 0, 0,
         0, 0, 1, 0;

    Eigen::Vector2d a;
    a = H*x;
    return a;
}

// maybe too special
Eigen::Vector2d ExtendedKalmanFilter4d::getStateInSphericalMeasurementSpace(const double height) const {
    Eigen::Vector2d z;
    //const double height = getCameraMatrixTop().translation.z; // part of the filter height? old (from last update) or current?
    const double x_coord = x(0);
    const double y_coord = x(2);
    const double d = std::sqrt(x_coord*x_coord + y_coord*y_coord);

    z << std::atan2(height,d),
         std::atan2(y_coord,x_coord);

    return z;
}
