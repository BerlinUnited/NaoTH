#include "KalmanFilter4d.h"

KalmanFilter4d::KalmanFilter4d(const Eigen::Vector4d& state, const Eigen::Matrix2d& processNoiseStdSingleDimension, const Eigen::Matrix2d& measurementNoiseStd, const Eigen::Matrix2d& initialStateStdSingleDimension):
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

KalmanFilter4d::~KalmanFilter4d()
{

}

void KalmanFilter4d::prediction(const Eigen::Vector2d& u, double dt)
{
    // adapt state transition matrix
    F << 1, dt, 0, 0,
         0,  1, 0, 0,
         0,  0, 1, dt,
         0,  0, 0, 1;

    // adapt control matrix to dt
    B << dt*dt/2, 0,
         dt     , 0,
         0      , dt*dt/2,
         0      , dt;

    // predict
    x_pre = F * x + B * u;
    P_pre = F * P * F.transpose() + Q;

    x = x_pre;
    P = P_pre;
}

void KalmanFilter4d::update(const Eigen::Vector2d& z)
{
    Eigen::Matrix2d temp1 = H * P_pre * H.transpose() + R;

    K = P_pre * H.transpose() * temp1.inverse();

    x_corr = x_pre + K * (z - H * x_pre );

    P_corr = P_pre - K*H*P_pre;

    x = x_corr;
    P = P_corr;
}


void KalmanFilter4d::extendedUpdate(const Eigen::Matrix<double,2,4>& h, const Eigen::Vector2d& z)
{
    H = h;

    Eigen::Matrix2d temp1 = H * P_pre * H.transpose() + R;

    K = P_pre * H.transpose() * temp1.inverse();

    x_corr = x_pre + K * (z - H * x_pre );

    P_corr = P_pre - K*H*P_pre;

    x = x_corr;
    P = P_corr;
}

void KalmanFilter4d::setState(Eigen::Vector4d& state)
{
    x = state;
}

void KalmanFilter4d::setCovarianceOfProcessNoise(const Eigen::Matrix2d& q){
    Q << q, Eigen::Matrix2d::Zero(), Eigen::Matrix2d::Zero(), q;
}

void KalmanFilter4d::setCovarianceOfMeasurementNoise(const Eigen::Matrix2d& r){
    R << r;
}

const Eigen::Vector4d& KalmanFilter4d::getState() const
{
    return x;
}

const Eigen::Matrix4d& KalmanFilter4d::getProcessCovariance() const
{
    return P;
}

const Eigen::Matrix2d& KalmanFilter4d::getMeasurementCovariance() const
{
    return R;
}

Eigen::Vector2d KalmanFilter4d::getStateInMeasurementSpace() const {
    Eigen::Vector2d a;
    a = H*x;
    return a;
}

// maybe too special
Eigen::Vector2d KalmanFilter4d::getStateInSphericalMeasurementSpace(const double height) const {
    Eigen::Vector2d z;
    //const double height = getCameraMatrixTop().translation.z; // part of the filter height? old (from last update) or current?
    const double x_coord = x(0);
    const double y_coord = x(2);
    const double d = std::sqrt(x_coord*x_coord + y_coord*y_coord);

    z << std::atan2(height,d),
         std::atan2(x_coord,y_coord);
    return z;
}
