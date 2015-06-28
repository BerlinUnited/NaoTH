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
}

void ExtendedKalmanFilter4d::update(const Eigen::Vector2d& z)
{
    Eigen::Vector2d predicted_measurement;

    predicted_measurement = getStateInMeasurementSpace();

    // approximate H with central differential quotient

    bool in_front_of_plane = true;
    double h = 0.0001;
    Vector2d dx1, dx2, dy1, dy2;

    if(useCamTop) {
        in_front_of_plane = in_front_of_plane && CameraGeometry::relativePointToImage(*camMatTop, *camInfoTop, Vector3d(x(0)-h, x(2), ball_height), dx1);
        in_front_of_plane = in_front_of_plane && CameraGeometry::relativePointToImage(*camMatTop, *camInfoTop, Vector3d(x(0)+h, x(2), ball_height), dx2);
        in_front_of_plane = in_front_of_plane && CameraGeometry::relativePointToImage(*camMatTop, *camInfoTop, Vector3d(x(0), x(2)-h, ball_height), dy1);
        in_front_of_plane = in_front_of_plane && CameraGeometry::relativePointToImage(*camMatTop, *camInfoTop, Vector3d(x(0), x(2)+h, ball_height), dy2);

        dx1 = CameraGeometry::angleToPointInImage(*camMatTop,*camInfoTop,dx1.x,dx1.y);
        dx2 = CameraGeometry::angleToPointInImage(*camMatTop,*camInfoTop,dx2.x,dx2.y);
        dy1 = CameraGeometry::angleToPointInImage(*camMatTop,*camInfoTop,dy1.x,dy1.y);
        dy2 = CameraGeometry::angleToPointInImage(*camMatTop,*camInfoTop,dy2.x,dy2.y);
    } else {
        in_front_of_plane = in_front_of_plane && CameraGeometry::relativePointToImage(*camMat, *camInfo, Vector3d(x(0)-h, x(2), ball_height), dx1);
        in_front_of_plane = in_front_of_plane && CameraGeometry::relativePointToImage(*camMat, *camInfo, Vector3d(x(0)+h, x(2), ball_height), dx2);
        in_front_of_plane = in_front_of_plane && CameraGeometry::relativePointToImage(*camMat, *camInfo, Vector3d(x(0), x(2)-h, ball_height), dy1);
        in_front_of_plane = in_front_of_plane && CameraGeometry::relativePointToImage(*camMat, *camInfo, Vector3d(x(0), x(2)+h, ball_height), dy2);

        dx1 = CameraGeometry::angleToPointInImage(*camMat,*camInfo,dx1.x,dx1.y);
        dx2 = CameraGeometry::angleToPointInImage(*camMat,*camInfo,dx2.x,dx2.y);
        dy1 = CameraGeometry::angleToPointInImage(*camMat,*camInfo,dy1.x,dy1.y);
        dy2 = CameraGeometry::angleToPointInImage(*camMat,*camInfo,dy2.x,dy2.y);
    }

    Vector2d dx = (dx2-dx1)/(2*h);
    Vector2d dy = (dy2-dy1)/(2*h);

    Eigen::Matrix<double,2,4> H_approx;
    H_approx << dx.x, 0, dy.x, 0,
                dx.y, 0, dy.y, 0;

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
}

Eigen::Vector2d ExtendedKalmanFilter4d::createMeasurementVector(const BallPercept& bp)
{
    Vector2d angles;
    Eigen::Vector2d z;

    angles = CameraGeometry::angleToPointInImage(*camMat,*camInfo,bp.centerInImage.x,bp.centerInImage.y);

    z << angles.x, angles.y;

    useCamTop = false;

    return z;
}

Eigen::Vector2d ExtendedKalmanFilter4d::createMeasurementVector(const BallPerceptTop& bp)
{
    Vector2d angles;
    Eigen::Vector2d z;

    angles = CameraGeometry::angleToPointInImage(*camMatTop,*camInfoTop,bp.centerInImage.x,bp.centerInImage.y);

    z << angles.x, angles.y;

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

    Vector2d angles;

    if(useCamTop){
        in_front_of_plane = CameraGeometry::relativePointToImage(*camMatTop, *camInfoTop, point, pointInImage);
        angles            = CameraGeometry::angleToPointInImage(*camMatTop,*camInfoTop,pointInImage.x,pointInImage.y);
    }
    else {
        in_front_of_plane = CameraGeometry::relativePointToImage(*camMat, *camInfo, point, pointInImage);
        angles            = CameraGeometry::angleToPointInImage(*camMat,*camInfo,pointInImage.x,pointInImage.y);
    }

    Eigen::Vector2d z;

    if(in_front_of_plane)
        z << angles.x, angles.y;// angle horizontal, angle vertical
    else
        z << std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity();

    return z;
}
