#include "KalmanFilter4d.h"

KalmanFilter4d::KalmanFilter4d()
{
    double dt = 30;

    // state transition matrix
    F << 1, dt, 0, 0,
         0,  1, 0, 0,
         0,  0, 1, dt,
         0,  0, 0, 1;

    // initial state x
    x << 0, 0, 0, 0;

    // control matrix
    B << dt*dt/2, 0,
         dt     , 0,
         0, dt*dt/2,
         0, dt;

    // covariance matrix of process noise (values taken from old kalman filter)
    double q = 3;
    Q << q, 0, 0, 0,
         0, q, 0, 0,
         0, 0, q, 0,
         0, 0, 0, q;

    // inital covariance matrix of current state (values taken from old kalman filter)
    double p = 62500;
    P << p, 0/*p*/, 0, 0,
         0/*p*/, p, 0, 0,
         0, 0, p, 0/*p*/,
         0, 0, 0/*p*/, p;

    // state to measurement transformation matrix
    H << 1, 0, 0, 0,
         0, 0, 1, 0;

    // covariance matrix of measurement noise (values taken from old kalman filter)
    // added 100 in R(1,1) and R(3,3) to "ensure" invertiablity during kalman gain computation
    R << 10, 0,
         0, 10;

}

KalmanFilter4d::KalmanFilter4d(Eigen::Vector4d& state):
    x(state)
{
    double dt = 30;

    // state transition matrix
    F << 1, dt, 0, 0,
         0,  1, 0, 0,
         0,  0, 1, dt,
         0,  0, 0, 1;

    // control matrix
    B << dt*dt/2, 0,
         dt     , 0,
         0, dt*dt/2,
         0, dt;


    // covariance matrix of process noise (values taken from old kalman filter)
    double q = 3;
    Q << q, 0, 0, 0,
         0, q, 0, 0,
         0, 0, q, 0,
         0, 0, 0, q;

    // inital covariance matrix of current state (values taken from old kalman filter)
    double p = 62500;
    P << p, 0/*p*/, 0, 0,
         0/*p*/, p, 0, 0,
         0, 0, p, 0/*p*/,
         0, 0, 0/*p*/, p;

    // state to measurement transformation matrix
    H << 1, 0, 0, 0,
         0, 0, 1, 0;

    // covariance matrix of measurement noise (values taken from old kalman filter)
    // added 100 in R(1,1) and R(3,3) to "ensure" invertiablity during kalman gain computation
    R << 10, 0,
         0, 10;
}


KalmanFilter4d::KalmanFilter4d(Eigen::Vector2d& measurement)
{
double dt;

// state transition matrix
F << 1, dt, 0, 0,
     0,  1, 0, 0,
     0,  0, 1, dt,
     0,  0, 0, 1;

// control matrix
B << dt*dt/2, 0,
     dt     , 0,
     0, dt*dt/2,
     0, dt;

// initial state x
x << measurement(0), 0, measurement(1), 0;

// covariance matrix of process noise (values taken from old kalman filter)
double q = 3;
Q << q, 0, 0, 0,
     0, q, 0, 0,
     0, 0, q, 0,
     0, 0, 0, q;

// inital covariance matrix of current state (values taken from old kalman filter)
double p = 62500;
P << p, 0/*p*/, 0, 0,
     0/*p*/, p, 0, 0,
     0, 0, p, 0/*p*/,
     0, 0, 0/*p*/, p;

// state to measurement transformation matrix
H << 1, 0, 0, 0,
     0, 0, 1, 0;

// covariance matrix of measurement noise (values taken from old kalman filter)
// added 100 in R(1,1) and R(3,3) to "ensure" invertiablity during kalman gain computation
R << 10, 0,
     0, 10;
}

KalmanFilter4d::~KalmanFilter4d()
{

}

void KalmanFilter4d::prediction(const Eigen::Vector2d& u, double dt)
{
    // adapt state transition matrix to dt
    F(0,1) = dt;
    F(2,3) = dt;

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
