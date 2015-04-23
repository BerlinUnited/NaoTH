#ifndef EIGEN_DONT_VECTORIZE
#define EIGEN_DONT_VECTORIZE
#endif

#ifndef EIGEN_DISABLE_UNALIGNED_ARRAY_ASSERT
#define EIGEN_DISABLE_UNALIGNED_ARRAY_ASSERT
#endif

#ifndef KALMANFILTER4D_H
#define KALMANFILTER4D_H

#include <Eigen/Eigen>

class KalmanFilter4d
{
public:
    KalmanFilter4d(const Eigen::Vector4d& state, const Eigen::Matrix2d& processNoiseStdSingleDimension, const Eigen::Matrix2d& measurementNoiseStd, const Eigen::Matrix2d& initialStateStdSingleDimension);

    ~KalmanFilter4d();

    void prediction(const Eigen::Vector2d &u, double dt);
    void update(const Eigen::Vector2d &z);
    void extendedUpdate(const Eigen::Vector2d& z, const double height);

    void setState(Eigen::Vector4d& state);
    void setCovarianceOfProcessNoise(const Eigen::Matrix2d& q);
    void setCovarianceOfMeasurementNoise(const Eigen::Matrix2d& r);

    const Eigen::Vector4d& getState() const;
    Eigen::Vector2d getStateInMeasurementSpace() const;
    Eigen::Vector2d getStateInSphericalMeasurementSpace(const double height) const;
    const Eigen::Matrix4d& getProcessCovariance() const;
    const Eigen::Matrix2d& getMeasurementCovariance() const;

private:

    Eigen::Matrix4d F; // state transition matrix
    Eigen::Matrix<double,4,2> B; // control matrix
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

};

#endif // KALMANFILTER4D_H
