#include "ExtendedKalmanFilter4d.h"

#include <Eigen/Eigenvalues>

ExtendedKalmanFilter4d::ExtendedKalmanFilter4d(const Eigen::Vector4d& state, const Eigen::Matrix2d& processNoiseStdSingleDimension, const Eigen::Matrix2d& measurementNoiseCovariances, const Eigen::Matrix2d& initialStateStdSingleDimension):
    x(state)
{
    Eigen::Matrix2d q;
    q = processNoiseStdSingleDimension.cwiseProduct(processNoiseStdSingleDimension);

    // covariance matrix of process noise (values taken from old kalman filter)
    Q << q, Eigen::Matrix2d::Zero(), Eigen::Matrix2d::Zero(), q;

    // covariance matrix of measurement noise
    Eigen::Matrix2d r;
    R << measurementNoiseCovariances;

    // inital covariance matrix of current state (values taken from old kalman filter)
    //double p = 62500;
    Eigen::Matrix2d p;
    p = initialStateStdSingleDimension.cwiseProduct(initialStateStdSingleDimension);
    P << p, Eigen::Matrix2d::Zero(), Eigen::Matrix2d::Zero(), p;

    P_pre = P;  // covariance matrix of predicted state
    P_corr = P; // covariance matrix of corrected state

    x_pre = x;
    x_corr = x;

    updateEllipses();
}

ExtendedKalmanFilter4d::~ExtendedKalmanFilter4d()
{

}

void ExtendedKalmanFilter4d::predict(const Eigen::Vector2d& u, double dt)
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

    updateEllipses();
}

void ExtendedKalmanFilter4d::update(const Eigen::Vector2d& z, const Measurement_Function_H& h)
{
    Eigen::Vector2d predicted_measurement;

    predicted_measurement = getStateInMeasurementSpace(h);

    // approximate H with central differential quotient
    H = approximateH(h);

    Eigen::Matrix2d temp1 = H * P_pre * H.transpose() + R;

    K = P_pre * H.transpose() * temp1.inverse();

    // just for debugging - begin
    x_pre = x;
    P_pre = P;
    // just for debugging - end

    x_corr = x_pre + K * (z - predicted_measurement);

    P_corr = (Eigen::Matrix4d::Identity()-K*H)*P_pre;

    x = x_corr;
    P = P_corr;

    updateEllipses();
}

Eigen::Matrix<double,2,4> ExtendedKalmanFilter4d::approximateH(const Measurement_Function_H &h) const {
    double e = 1e-4;
    Eigen::Vector2d dx1, dx2, dy1, dy2;

    dx1 = h(x(0)-e,x(2));
    dx2 = h(x(0)+e,x(2));

    dy1 = h(x(0),x(2)-e);
    dy2 = h(x(0),x(2)+e);

    Eigen::Vector2d dx = (dx2-dx1)/(2*e);
    Eigen::Vector2d dy = (dy2-dy1)/(2*e);

    Eigen::Matrix<double,2,4> H_approx;
    H_approx << dx(0), 0, dy(0), 0,
                dx(1), 0, dy(1), 0;

    return H_approx;
}

//--- setter ---//

void ExtendedKalmanFilter4d::setState(Eigen::Vector4d& state)
{
    x = state;
}

void ExtendedKalmanFilter4d::setCovarianceOfProcessNoise(const Eigen::Matrix2d& q){
    Q << q, Eigen::Matrix2d::Zero(), Eigen::Matrix2d::Zero(), q;
}

void ExtendedKalmanFilter4d::setCovarianceOfState(const Eigen::Matrix4d& p){
    P << p;
}

void ExtendedKalmanFilter4d::setCovarianceOfMeasurementNoise(const Eigen::Matrix2d& r){
    R << r;
}

//--- getter ---//
Eigen::Matrix2d ExtendedKalmanFilter4d::getStateCovarianceInMeasurementSpace(const Measurement_Function_H& h) const // horizontal, vertical
{
    Eigen::Matrix<double,2,4> H_approx;

    H_approx = approximateH(h);

    return H_approx * P * H_approx.transpose();
}

void ExtendedKalmanFilter4d::updateEllipses()
{
    Eigen::EigenSolver< Eigen::Matrix<double,2,2> > es;

    // determine error ellipse for the location
    Eigen::Matrix2d loc_cov;

    loc_cov << P(0,0), P(0,2),
               P(2,0), P(2,2);

    es.compute(loc_cov,true);

    if(std::abs(es.eigenvalues()[0]) > std::abs(es.eigenvalues()[1]))
    {
        ellipse_location.minor = std::sqrt(5.99*std::abs(es.eigenvalues()[1]));
        ellipse_location.major = std::sqrt(5.99*std::abs(es.eigenvalues()[0]));
        ellipse_location.angle = std::atan2(es.eigenvectors()(1,1).real(),es.eigenvectors()(0,1).real());
    } else {
        ellipse_location.minor = std::sqrt(5.99*std::abs(es.eigenvalues()[0]));
        ellipse_location.major = std::sqrt(5.99*std::abs(es.eigenvalues()[1]));
        ellipse_location.angle = std::atan2(es.eigenvectors()(1,0).real(),es.eigenvectors()(0,0).real());
    }

    // determine error ellipse of the velocity
    Eigen::Matrix2d vol_cov;

    vol_cov << P(1,1), P(1,3),
               P(3,1), P(3,3);

    es.compute(vol_cov,true);

    if(std::abs(es.eigenvalues()[0]) > std::abs(es.eigenvalues()[1]))
    {
        ellipse_velocity.minor = std::sqrt(5.99*std::abs(es.eigenvalues()[1]));
        ellipse_velocity.major = std::sqrt(5.99*std::abs(es.eigenvalues()[0]));
        ellipse_velocity.angle = std::atan2(es.eigenvectors()(1,1).real(),es.eigenvectors()(0,1).real());
    } else {
        ellipse_velocity.minor = std::sqrt(5.99*std::abs(es.eigenvalues()[0]));
        ellipse_velocity.major = std::sqrt(5.99*std::abs(es.eigenvalues()[1]));
        ellipse_velocity.angle = std::atan2(es.eigenvectors()(1,0).real(),es.eigenvectors()(0,0).real());
    }
}
