#ifndef EIGEN_DONT_VECTORIZE
#define EIGEN_DONT_VECTORIZE
#endif

#ifndef EIGEN_DISABLE_UNALIGNED_ARRAY_ASSERT
#define EIGEN_DISABLE_UNALIGNED_ARRAY_ASSERT
#endif

#ifndef EXTENDEDKALMANFILTER4D_H
#define EXTENDEDKALMANFILTER4D_H

#include <Eigen/Eigen>

#include <Representations/Perception/BallPercept.h>

#include <Representations/Perception/CameraMatrix.h>
#include <Representations/Infrastructure/CameraInfo.h>

#include <Tools/Math/Vector2.h>
#include <Tools/Math/Vector3.h>
#include <Tools/CameraGeometry.h>

#include "MeasurementFunctions.h"

struct Ellipse2d{
    double angle;
    double minor;
    double major;
};

class ExtendedKalmanFilter4d
{
public:
    ExtendedKalmanFilter4d(const Eigen::Vector4d& state, const Eigen::Matrix2d& processNoiseStdSingleDimension, const Eigen::Matrix2d& measurementNoiseStd, const Eigen::Matrix2d& initialStateStdSingleDimension);

    ~ExtendedKalmanFilter4d();

    void prediction(const Eigen::Vector2d &u, double dt);
    void update(const Eigen::Vector2d &z, const Measurement_Function_H& h);

public:

    //--- setter ---//
    void setState(Eigen::Vector4d& state);
    void setCovarianceOfProcessNoise(const Eigen::Matrix2d& q);
    void setCovarianceOfMeasurementNoise(const Eigen::Matrix2d& r);

    //--- getter ---//
    const Eigen::Matrix4d& getProcessCovariance() const;
    const Eigen::Matrix2d& getMeasurementCovariance() const;
    const Eigen::Vector4d& getState() const;
    Eigen::Vector2d        getStateInMeasurementSpace(const Measurement_Function_H& h) const; // horizontal, vertical

    const Ellipse2d& getEllipse() const;

private:

    void updateEllipse();

private:

    // transformation matrices
    Eigen::Matrix4d F;           // state transition matrix
    Eigen::Matrix<double,4,2> B; // control matrix
    Eigen::Matrix<double,2,4> H; // state to measurement transformation matrix

    // states and control
    Eigen::Vector4d x;      // state x_location, x_velocity, y_location, y_velocity
    Eigen::Vector4d u;      // control vector x_velocity, x_acceleration, y_velocity, y_acceleration
    Eigen::Vector4d x_pre;  // predicted state
    Eigen::Vector4d x_corr; // corrected state

    // covariances
    Eigen::Matrix4d Q;      // covariance matrix of process noise
    Eigen::Matrix2d R;      // covariance matrix of measurement noise
    Eigen::Matrix4d P;      // covariance matrix of current state
    Eigen::Matrix4d P_pre;  // covariance matrix of predicted state
    Eigen::Matrix4d P_corr; // covariance matrix of corrected state

    // kalman gain
    Eigen::Matrix<double, 4,2> K;

    Ellipse2d ellipse;
};

#endif // EXTENDEDKALMANFILTER4D_H
