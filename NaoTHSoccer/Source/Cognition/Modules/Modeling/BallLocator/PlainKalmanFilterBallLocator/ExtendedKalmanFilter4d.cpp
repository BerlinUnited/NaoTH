#include "ExtendedKalmanFilter4d.h"

bool ExtendedKalmanFilter4d::useCamTop = false;

ExtendedKalmanFilter4d::ExtendedKalmanFilter4d(const Eigen::Vector4d& state, const Eigen::Matrix2d& processNoiseStdSingleDimension, const Eigen::Matrix2d& measurementNoiseStd, const Eigen::Matrix2d& initialStateStdSingleDimension, const CameraMatrix& camMat, const CameraMatrixTop& camMatTop, const naoth::CameraInfo& camInfo, const naoth::CameraInfoTop& camInfoTop):
    camMat(&camMat),
    camMatTop(&camMatTop),
    camInfo(&camInfo),
    camInfoTop(&camInfoTop),
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
}

ExtendedKalmanFilter4d::~ExtendedKalmanFilter4d()
{

}

void ExtendedKalmanFilter4d::prediction(const Eigen::Vector2d& /*u*/, double dt)
{
    // adapt state transition matrix
    F << 1, dt, 0, 0,
         0,  1, 0, 0,
         0,  0, 1, dt,
         0,  0, 0, 1;

    F = Eigen::Matrix4d::Identity();

    // adapt control matrix to dt
    B << dt*dt/2, 0,
         dt     , 0,
         0      , dt*dt/2,
         0      , dt;

    // predict
    x_pre = F * x /*+ B * u*/;
    P_pre = F * P * F.transpose() + Q;

    x = x_pre;
    P = P_pre;
}

void ExtendedKalmanFilter4d::update(const Eigen::Vector2d& z)
{
    Eigen::Vector2d predicted_measurement;

    predicted_measurement = getStateInMeasurementSpace();

    // create H

    Vector3d point(x(0),x(2),ball_height);

    Vector3d vectorToPoint;

    if(useCamTop)
    {
        vectorToPoint = camMatTop->rotation.invert() * (point - camMatTop->translation);
    } else {
        vectorToPoint = camMat->rotation.invert() * (point - camMat->translation);
    }

    double k = vectorToPoint.x;
    double g = vectorToPoint.y;
    double m = vectorToPoint.z;

    double r21 = camMatTop->rotation.invert()[0][1];
    double r22 = camMatTop->rotation.invert()[1][1];
    double r11 = camMatTop->rotation.invert()[0][0];
    double r12 = camMatTop->rotation.invert()[1][0];
    double r31 = camMatTop->rotation.invert()[0][2];
    double r32 = camMatTop->rotation.invert()[1][2];

    H << (r21*k-r11*g)/(k*k) ,0 , (r31*k-r11*m)/(k*k), 0,
         (r22*k-r12*g)/(k*k) ,0 , (r32*k-r12*m)/(k*k), 0;

    if(useCamTop)
    {
        H = -camInfoTop->getFocalLength() * H;
    } else {
        H = -camInfo->getFocalLength() * H;
    }

    Eigen::Matrix2d temp1 = H * P_pre * H.transpose() + R;

    K = P_pre * H.transpose() * temp1.inverse();

    x_corr = x_pre + K * (z - predicted_measurement);

    P_corr = (Eigen::Matrix4d::Identity()-K*H)*P_pre;

    x = x_corr;
    P = P_corr;
}

Eigen::Vector2d ExtendedKalmanFilter4d::createMeasurementVector(const BallPercept& bp)
{
    Eigen::Vector2d z;

    z << bp.centerInImage.x, bp.centerInImage.y;

    useCamTop = false;

    return z;
}

Eigen::Vector2d ExtendedKalmanFilter4d::createMeasurementVector(const BallPerceptTop& bp)
{
    Eigen::Vector2d z;

    z << bp.centerInImage.x, bp.centerInImage.y;

    useCamTop = true;

    return z;
}

//--- setter ---//

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

Eigen::Vector2d ExtendedKalmanFilter4d::getStateInMeasurementSpace() const
{
    bool in_front_of_plane;
    Vector3d point(x(0), x(2), ball_height);
    Vector2d pointInImage;

    if(useCamTop)
        in_front_of_plane = CameraGeometry::relativePointToImage(*camMatTop, *camInfoTop, point, pointInImage);
    else
        in_front_of_plane = CameraGeometry::relativePointToImage(*camMat, *camInfo, point, pointInImage);

    Eigen::Vector2d z;

    if(in_front_of_plane)
        z << pointInImage.x, pointInImage.y;
    else
        z << std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity();

    return z;
}
