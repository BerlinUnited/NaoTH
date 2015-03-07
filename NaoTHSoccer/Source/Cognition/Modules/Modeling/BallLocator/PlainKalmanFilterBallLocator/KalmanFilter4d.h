#ifndef KALMANFILTER4D_H
#define KALMANFILTER4D_H

#include <eigen3/Eigen/Eigen>

class KalmanFilter4d
{
public:
    KalmanFilter4d();
    ~KalmanFilter4d();

    void prediction(const Eigen::Vector4d& u, double dt);
    void update(const Eigen::Vector2d &z);

    void setState(Eigen::Vector4d state);

    const Eigen::Vector4d& getState() const;
    const Eigen::Matrix4d& getCovariance() const;

private:
    Eigen::Matrix4d F; // state transition matrix
    Eigen::Matrix4d B; // control matrix
    Eigen::Vector4d x; // state x_location, x_velocity, y_location, y_velocity
    Eigen::Vector4d u; // control vector x_velocity, x_acceleration, y_velocity, y_acceleration
    Eigen::Vector4d x_pre;  // predicted state
    Eigen::Vector4d x_corr; // corrected state

    Eigen::Matrix4d Q; // covariance matrix of process noise
    Eigen::Matrix4d P; // covariance matrix of current state
    Eigen::Matrix4d P_pre;  // covariance matrix of predicted state
    Eigen::Matrix4d P_corr; // covariance matrix of corrected state

    Eigen::Matrix<double,2,4> H; // state to measurement transformation matrix
    Eigen::Matrix2d R; // covariance matrix of measurement noise

    Eigen::Matrix<double, 4,2> K; // kalman gain

    const double dt; // 30 ms

};

#endif // KALMANFILTER4D_H
