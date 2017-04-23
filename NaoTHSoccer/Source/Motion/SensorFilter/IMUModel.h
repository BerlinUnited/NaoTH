#ifndef IMUMODEL_H
#define IMUMODEL_H

#include <ModuleFramework/Module.h>

#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugPlot.h"
#include "Tools/Debug/DebugDrawings3D.h"

#include <Representations/Infrastructure/GyrometerData.h>
#include <Representations/Infrastructure/AccelerometerData.h>

#include "Representations/Infrastructure/FrameInfo.h"
#include <Representations/Modeling/IMUData.h>

#include "Tools/Filters/KalmanFilter/UnscentedKalmanFilter/UnscentedKalmanFilter.h"

#include "IMURotationMeasurement.h"
#include "IMURotationState.h"

BEGIN_DECLARE_MODULE(IMUModel)

    PROVIDE(DebugRequest)
    PROVIDE(DebugDrawings3D)
    PROVIDE(DebugPlot)

    REQUIRE(FrameInfo)

    REQUIRE(GyrometerData)
    REQUIRE(AccelerometerData)

    PROVIDE(IMUData)
END_DECLARE_MODULE(IMUModel)

class IMUModel: private IMUModelBase
{
public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

public:
    IMUModel();

    void execute();
    void writeIMUData(double dt);
    void plots();

private:
    FrameInfo lastFrameInfo;

    UKF<6,6,RotationState<RotationMeasurement<6>, Measurement<3>,6> > ukf_rot;
    UKF<3,3,State<Measurement<3>, 3> > ukf_acc_global;

    typedef RotationMeasurement<6> IMU_RotationMeasurement;
    typedef Measurement<3>         IMU_RotVelMeasurement;
    typedef Measurement<3>         IMU_AccMeasurementGlobal;

    Eigen::Vector3d quaternionToRotationVector(const Eigen::Quaterniond& q) const{
        Eigen::AngleAxisd temp(q);
        return temp.angle() * temp.axis();
    }

    bool updated_by_both;
};

#endif // IMUMODEL_H
