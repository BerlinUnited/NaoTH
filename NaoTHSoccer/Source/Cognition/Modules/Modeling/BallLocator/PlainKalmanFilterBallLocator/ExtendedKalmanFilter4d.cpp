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

    P_pre = P;  // covariance matrix of predicted state
    P_corr = P; // covariance matrix of corrected state

    x_pre = x;
    x_corr = x;
}

ExtendedKalmanFilter4d::~ExtendedKalmanFilter4d()
{

}

void ExtendedKalmanFilter4d::prediction(const Eigen::Vector2d& u, double dt)
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

    updateEllipse();
}

void ExtendedKalmanFilter4d::update(const Eigen::Vector2d& z,const Measurement_Function_H& h)
{
    Eigen::Vector2d predicted_measurement;

    predicted_measurement = getStateInMeasurementSpace(h);

    // approximate H with central differential quotient

    double e = 0.0001;
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

    // use approximation
    H = H_approx;

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

    updateEllipse();
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

Eigen::Vector2d ExtendedKalmanFilter4d::getStateInMeasurementSpace(const Measurement_Function_H& h) const
{
    return h(x(0),x(2));
}

void ExtendedKalmanFilter4d::updateEllipse()
{
    Eigen::EigenSolver< Eigen::Matrix<double,2,2> > es;

    Eigen::Matrix2d loc_cov;

    loc_cov << P(0,0), P(0,2),
               P(2,0), P(2,2);

    es.compute(loc_cov,true);

    if(std::abs(es.eigenvalues()[0]) > std::abs(es.eigenvalues()[1]))
    {
        ellipse.minor = std::sqrt(5.99*std::abs(es.eigenvalues()[1]));
        ellipse.major = std::sqrt(5.99*std::abs(es.eigenvalues()[0]));
        ellipse.angle = std::atan2(es.eigenvectors()(1,1).real(),es.eigenvectors()(0,1).real());
    } else {
        ellipse.minor = std::sqrt(5.99*std::abs(es.eigenvalues()[0]));
        ellipse.major = std::sqrt(5.99*std::abs(es.eigenvalues()[1]));
        ellipse.angle = std::atan2(es.eigenvectors()(1,0).real(),es.eigenvectors()(0,0).real());
    }
}

const Ellipse2d& ExtendedKalmanFilter4d::getEllipse() const
{
    return ellipse;
}
