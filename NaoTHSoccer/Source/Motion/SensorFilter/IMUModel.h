/**
* @file IMUModel.h
*
* Declaration of class IMUModel
*
* @author <a href="mailto:kaden@informatik.hu-berlin.de">Steffen Kaden</a>
*/


#ifndef IMUMODEL_H
#define IMUMODEL_H

#include <ModuleFramework/Module.h>

// debug
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugDrawings3D.h"
#include "Tools/Debug/DebugPlot.h"
#include "Tools/Debug/DebugParameterList.h"

// basics
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Infrastructure/RobotInfo.h"

// sensors
#include <Representations/Infrastructure/GyrometerData.h>
#include <Representations/Infrastructure/AccelerometerData.h>
#include <Representations/Infrastructure/InertialSensorData.h>
#include "Representations/Motion/MotionStatus.h"
#include "Representations/Modeling/InertialModel.h"

// result
#include <Representations/Modeling/IMUData.h>

// tools
#include "Tools/Filters/KalmanFilter/UnscentedKalmanFilter/UnscentedKalmanFilter.h"
#include "IMURotationMeasurement.h"
#include "IMURotationState.h"

#include <Eigen/Core>
#include <Eigen/Geometry>

BEGIN_DECLARE_MODULE(IMUModel)
    PROVIDE(DebugRequest)
    PROVIDE(DebugDrawings3D)
    PROVIDE(DebugPlot)
    PROVIDE(DebugParameterList)

    REQUIRE(FrameInfo)
    REQUIRE(RobotInfo)

    REQUIRE(GyrometerData)
    REQUIRE(AccelerometerData)
    REQUIRE(InertialSensorData)
    REQUIRE(InertialModel)

    REQUIRE(MotionStatus)

    PROVIDE(IMUData)
END_DECLARE_MODULE(IMUModel)

class IMUModel: private IMUModelBase
{
public:
    IMUModel();
    virtual ~IMUModel();

    virtual void execute();
    void writeIMUData();
    void plots();
    void reloadParameters();
    void reset_filter();

    FrameInfo lastFrameInfo;
    GyrometerData lastGyrometerData;
    AccelerometerData lastAccelerometerData;

    /* filter for rotation */
    using IMUMeasurement = Measurement<6>;  // both acc + gyro
    using RotationUKF = UKF<RotationState<AccMeasurement, GyroMeasurement, IMUMeasurement, 6> >;

    RotationUKF ukf_rot;

    Eigen::Matrix<double,6,6> Q_rotation;
    Eigen::Matrix<double,6,6> Q_rotation_walk;
    Eigen::Matrix<double,6,6> R_rotation;
    Eigen::Matrix<double,6,6> R_rotation_walk;

    /* filter for global acceleration */
    UKF<State<Measurement<3>, 3> > ukf_acc_global;

    typedef Measurement<3> IMU_AccMeasurementGlobal;

    Eigen::Matrix<double,3,3> Q_acc;
    Eigen::Matrix<double,3,3> Q_acc_walk;
    Eigen::Matrix<double,3,3> R_acc;
    Eigen::Matrix<double,3,3> R_acc_walk;

private: /* small helper */
    Eigen::Vector3d quaternionToRotationVector(const Eigen::Quaterniond& q) const {
        Eigen::AngleAxisd temp(q);
        return temp.angle() * temp.axis();
    }

    Vector3d quaternionToVector3D(const Eigen::Quaterniond& q) const {
        Eigen::AngleAxisd temp(q);
        Eigen::Vector3d temp2(temp.angle() * temp.axis());
        return Vector3d(temp2(0),temp2(1),temp2(2));
    }

    Vector3d eigenVectorToVector3D(const Eigen::Vector3d& v) const {
        return Vector3d(v(0),v(1),v(2));
    }

public:
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

           // Gyro Covariance Matrix (sample size: 154605) - 20201210
           // [[ 5.53092822e-05 -3.30760425e-05 -4.97718715e-06]
           //  [-3.30760425e-05  6.38229161e-05 -8.44537228e-06]
           //  [-4.97718715e-06 -8.44537228e-06  6.73641306e-06]]

           // Accelerometer Covariance Matrix (sample size: 154605) - 20201210
           // [[ 2.63164975e-04  8.46430940e-07  5.08220594e-06]
           //  [ 8.46430940e-07  1.44437874e-04 -4.11118190e-07]
           //  [ 5.08220594e-06 -4.11118190e-07  3.01043187e-04]]

           PARAMETER_REGISTER(enableWhileWalking) = true;
           PARAMETER_REGISTER(check_input_sensors) = true;
           PARAMETER_REGISTER(prediction_horizon) = 2;

           /* acceleration filter parameter */
           // while standing
           PARAMETER_REGISTER(acceleration.stand.processNoiseAcc) = 0.01; // [m^2/s^4]
           PARAMETER_REGISTER(acceleration.stand.measurementNoiseAcc) =  1; // [m^2/s^4]

           // walking
           PARAMETER_REGISTER(acceleration.walk.processNoiseAcc) = 0.01; // [m^2/s^4]
           PARAMETER_REGISTER(acceleration.walk.measurementNoiseAcc) =  1; // [m^2/s^4]
           /* acceleration filter parameter end */

           /* rotation filter parameter */
           // while standing
           PARAMETER_REGISTER(rotation.stand.processNoiseRot) = 0.01; // [rad^2] 9.86 ~ std of angle around x = pi
           PARAMETER_REGISTER(rotation.stand.processNoiseGyro) = 0.01; // [rad^2/s^2]

           // the measurement is the measured acceleration and gyrometer vector
           PARAMETER_REGISTER(rotation.stand.measurementNoiseAcc) = 10.0;  // [m^2/s^4]
           PARAMETER_REGISTER(rotation.stand.measurementNoiseGyro) =  1.0; // [rad^2/s^2]

           // while walking
           PARAMETER_REGISTER(rotation.walk.processNoiseRot) = 0.01; // [rad^2]
           PARAMETER_REGISTER(rotation.walk.processNoiseGyro) = 0.01; // [rad^2/s^2]

           PARAMETER_REGISTER(rotation.walk.measurementNoiseAcc) = 50.0;  // [m^2/s^4]
           PARAMETER_REGISTER(rotation.walk.measurementNoiseGyro) =  0.1; // [rad^2/s^2]
           /* rotation filter parameter end */

           syncWithConfig();
       }

       bool enableWhileWalking;
       bool check_input_sensors; // if true the filter will only be updated if the input changes (i.e. acc and gyro)
       double prediction_horizon; // number of frames the model shall predict in IMUData

       struct Filter{
           struct Mode{
                double processNoiseAcc;
                double measurementNoiseAcc;
           } walk, stand;
       } acceleration;

       struct Filter2{
           struct Mode{
                double processNoiseRot;
                double processNoiseGyro;

                //TODO: provide all entries of the covariance matrix?
                double measurementNoiseAcc;
                double measurementNoiseGyro;
           } walk, stand;
       } rotation;

    } imuParameters;

     Eigen::Quaterniond integrated;
};

#endif // IMUMODEL_H
