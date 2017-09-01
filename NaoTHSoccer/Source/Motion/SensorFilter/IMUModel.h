#ifndef IMUMODEL_H
#define IMUMODEL_H

#include <ModuleFramework/Module.h>

#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugPlot.h"
#include "Tools/Debug/DebugDrawings3D.h"
#include "Tools/Debug/DebugParameterList.h"
#include <Representations/Debug/Stopwatch.h>

#include <Representations/Infrastructure/GyrometerData.h>
#include <Representations/Infrastructure/AccelerometerData.h>

#include "Representations/Infrastructure/FrameInfo.h"
#include <Representations/Modeling/IMUData.h>
#include <Representations/Modeling/InertialModel.h>

#include "Representations/Motion/MotionStatus.h"

#include "Tools/Filters/KalmanFilter/UnscentedKalmanFilter/UnscentedKalmanFilter.h"

#include "IMURotationMeasurement.h"
#include "IMURotationState.h"

BEGIN_DECLARE_MODULE(IMUModel)
    PROVIDE(DebugRequest)
    PROVIDE(DebugDrawings3D)
    PROVIDE(DebugPlot)
    PROVIDE(DebugParameterList)

    REQUIRE(FrameInfo)

    REQUIRE(GyrometerData)
    REQUIRE(AccelerometerData)

    REQUIRE(MotionStatus)

    PROVIDE(IMUData)
    PROVIDE(InertialModel) // only to enable transparent switching with InertiaSensorFilter
END_DECLARE_MODULE(IMUModel)

class IMUModel: private IMUModelBase
{
public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

public:
    IMUModel();
    virtual~IMUModel();

    virtual void execute();
    void writeIMUData();
    void plots();

private:
    FrameInfo lastFrameInfo;

    UKF<RotationState<Measurement<3>,3> > ukf_rot;
    UKF<State<Measurement<3>, 3> > ukf_acc_global;

    typedef Measurement<3> IMU_RotationMeasurement;
    typedef Measurement<3> IMU_AccMeasurementGlobal;

    Eigen::Vector3d quaternionToRotationVector(const Eigen::Quaterniond& q) const{
        Eigen::AngleAxisd temp(q);
        return temp.angle() * temp.axis();
    }

    Vector3d quaternionToVector3D(const Eigen::Quaterniond& q) const{
        Eigen::AngleAxisd temp(q);
        Eigen::Vector3d temp2(temp.angle() * temp.axis());
        return Vector3d(temp2(0),temp2(1),temp2(2));
    }

    Vector3d eigenVectorToVector3D(const Eigen::Vector3d& v) const {
        return Vector3d(v(0),v(1),v(2));
    }

    Eigen::Matrix<double,3,3> R_acc;
    Eigen::Matrix<double,3,3> R_rotation;

    Eigen::Matrix<double,3,3> R_acc_walk;
    Eigen::Matrix<double,3,3> R_rotation_walk;

    void reloadParameters();

    class IMUParameters:  public ParameterList
    {
    public:
       IMUParameters() : ParameterList("IMUModel")
       {
           // measured covariance of acceleration and rotational velocity (sitting, motion log, 60 seconds)
           // R << 5.074939351879890342e-04, -1.561730283237946278e-05,  1.012849085655689321e-04, -3.078687958578659292e-08, -1.132513004663809251e-06, -6.485352375515866273e-07,
           //     -1.561730283237946278e-05,  2.570436087068024501e-04, -4.159091012580820026e-05, -3.013278205585369588e-07,  1.736820285922189584e-06, -4.599219827687661978e-07,
           //      1.012849085655689321e-04, -4.159091012580820026e-05,  4.727921819788054878e-04,  5.523361976811979815e-07, -1.730307422507887473e-07, -3.030009469390110280e-07,
           //     -3.078687958578659292e-08, -3.013278205585369588e-07,  5.523361976811979815e-07,  3.434758685147043306e-06, -8.299226917536411892e-08,  5.842662059539863827e-08,
           //     -1.132513004663809251e-06,  1.736820285922189584e-06, -1.730307422507887473e-07, -8.299226917536411892e-08,  1.006052718494827880e-05,  1.346681994776136150e-06,
           //     -6.485352375515866273e-07, -4.599219827687661978e-07, -3.030009469390110280e-07,  5.842662059539863827e-08,  1.346681994776136150e-06,  3.242298821157115427e-06;

           // measured covariance of acceleration and rotational velocity (stepping, motion log, ca 60 seconds)
           // R << 1.243597518737325602e+00,  4.170491338560008421e-01, -3.246651961728642261e-02,  7.198860088927394596e-02,  7.571177515371038495e-03,  3.889717847753307658e-03
           //      4.170491338560008421e-01,  3.447355924530388194e+00, -5.760325107959280988e-01,  6.324698666386123769e-01, -3.214041830314497833e-02,  3.479509634670579533e-03
           //     -3.246651961728642261e-02, -5.760325107959280988e-01,  1.204579269450257506e+01, -3.593949540100419698e-02, -1.215610929410663392e-04, -1.449923729607617909e-03
           //      7.198860088927394596e-02,  6.324698666386123769e-01, -3.593949540100419698e-02,  1.941392231130305845e-01, -7.726795031285729927e-03,  1.290728774618065365e-02
           //      7.571177515371038495e-03, -3.214041830314497833e-02, -1.215610929410663392e-04, -7.726795031285729927e-03,  1.205640301729023374e-02, -1.577172441763024426e-05
           //      3.889717847753307658e-03,  3.479509634670579533e-03, -1.449923729607617909e-03,  1.290728774618065365e-02, -1.577172441763024426e-05,  1.671519468894508767e-02

           // measurement covariance matrix for the orientation distribution resulting from the measured accelerometer (stepping, motion log, ca 60 seconds)
           // note that no rotation about the z axis is measurable if the accelerometer is used solely. Therefore an artificial variance for z is chosen.
           // R_rotation << 6.121479966093734681e-01, -2.013737709159823391e-02,    0,0,
           //              -2.013737709159823391e-02,  2.705392100034859082e-01,    0.0,
           //               0.000000000000000000e+00,  0.000000000000000000e+00, 10e-05;

           PARAMETER_REGISTER(enableWhileWalking) = true;

           PARAMETER_REGISTER(stand.processNoiseAccQ00) = 0.01; // [m^2/s^4]
           PARAMETER_REGISTER(stand.processNoiseAccQ11) = 0.01; // [m^2/s^4]
           PARAMETER_REGISTER(stand.processNoiseAccQ22) = 0.01; // [m^2/s^4]

           PARAMETER_REGISTER(stand.processNoiseRotationQ00) = 0.1; // [rad^2] 9.86 ~ std of angle around x = pi
           PARAMETER_REGISTER(stand.processNoiseRotationQ11) = 0.1; // [rad^2]
           PARAMETER_REGISTER(stand.processNoiseRotationQ22) = 0.1; // [rad^2]

           PARAMETER_REGISTER(stand.measurementNoiseAccR00) =  1.243597518737325602e+00; // [m^2/s^4]
           PARAMETER_REGISTER(stand.measurementNoiseAccR01) =  4.170491338560008421e-01; // [m^2/s^4]
           PARAMETER_REGISTER(stand.measurementNoiseAccR02) = -3.246651961728642261e-02; // [m^2/s^4]
           PARAMETER_REGISTER(stand.measurementNoiseAccR11) =  3.447355924530388194e+00; // [m^2/s^4]
           PARAMETER_REGISTER(stand.measurementNoiseAccR12) = -5.760325107959280988e-01; // [m^2/s^4]
           PARAMETER_REGISTER(stand.measurementNoiseAccR22) =  1.204579269450257506e+01; // [m^2/s^4]

           // sqrt(R00,R11,R22) ~ the standard derivation rotation with axis sqrt(R00,R11,R22) and angle |(R00,R11,R22)|
           PARAMETER_REGISTER(stand.measurementNoiseRotationR00) =  0.5;  // [rad^2]
           PARAMETER_REGISTER(stand.measurementNoiseRotationR01) =  0.0; // [rad^2]
           PARAMETER_REGISTER(stand.measurementNoiseRotationR02) =  0.0; // [rad^2]
           PARAMETER_REGISTER(stand.measurementNoiseRotationR11) =  0.5;  // [rad^2]
           PARAMETER_REGISTER(stand.measurementNoiseRotationR12) =  0.0; // [rad^2]
           PARAMETER_REGISTER(stand.measurementNoiseRotationR22) =  0.5; // [rad^2]

           // walk
           PARAMETER_REGISTER(walk.processNoiseAccQ00) = 0.01; // [m^2/s^4]
           PARAMETER_REGISTER(walk.processNoiseAccQ11) = 0.01; // [m^2/s^4]
           PARAMETER_REGISTER(walk.processNoiseAccQ22) = 0.01; // [m^2/s^4]

           PARAMETER_REGISTER(walk.processNoiseRotationQ00) = 0.1; // [rad^2]
           PARAMETER_REGISTER(walk.processNoiseRotationQ11) = 0.1; // [rad^2]
           PARAMETER_REGISTER(walk.processNoiseRotationQ22) = 0.1; // [rad^2]

           PARAMETER_REGISTER(walk.measurementNoiseAccR00) =  1.243597518737325602e+00; // [m^2/s^4]
           PARAMETER_REGISTER(walk.measurementNoiseAccR01) =  4.170491338560008421e-01; // [m^2/s^4]
           PARAMETER_REGISTER(walk.measurementNoiseAccR02) = -3.246651961728642261e-02; // [m^2/s^4]
           PARAMETER_REGISTER(walk.measurementNoiseAccR11) =  3.447355924530388194e+00; // [m^2/s^4]
           PARAMETER_REGISTER(walk.measurementNoiseAccR12) = -5.760325107959280988e-01; // [m^2/s^4]
           PARAMETER_REGISTER(walk.measurementNoiseAccR22) =  1.204579269450257506e+01; // [m^2/s^4]

           PARAMETER_REGISTER(walk.measurementNoiseRotationR00) =  50; // [rad^2]
           PARAMETER_REGISTER(walk.measurementNoiseRotationR01) =  0.0; // [rad^2]
           PARAMETER_REGISTER(walk.measurementNoiseRotationR02) =  0.0; // [rad^2]
           PARAMETER_REGISTER(walk.measurementNoiseRotationR11) =  50; // [rad^2]
           PARAMETER_REGISTER(walk.measurementNoiseRotationR12) =  0.0; // [rad^2]
           PARAMETER_REGISTER(walk.measurementNoiseRotationR22) =  50; // [rad^2]

           syncWithConfig();
       }

       bool enableWhileWalking;

       struct Params{
       double processNoiseAccQ00;
       double processNoiseAccQ11;
       double processNoiseAccQ22;

       double processNoiseRotationQ00;
       double processNoiseRotationQ11;
       double processNoiseRotationQ22;

       double measurementNoiseAccR00;
       double measurementNoiseAccR01;
       double measurementNoiseAccR02;
       double measurementNoiseAccR11;
       double measurementNoiseAccR12;
       double measurementNoiseAccR22;

       double measurementNoiseRotationR00;
       double measurementNoiseRotationR01;
       double measurementNoiseRotationR02;
       double measurementNoiseRotationR11;
       double measurementNoiseRotationR12;
       double measurementNoiseRotationR22;
       } stand, walk;

    } imuParameters;

     Eigen::Quaterniond integrated;
};

#endif // IMUMODEL_H
