/**
* @file IMUModel.cpp
*
* Definition of class IMUModel
*
* @author <a href="mailto:kaden@informatik.hu-berlin.de">Steffen Kaden</a>
*/

#include "IMUModel.h"

IMUModel::IMUModel() :
    integrated(1,0,0,0)
{
    DEBUG_REQUEST_REGISTER("IMUModel:reset_filter", "reset filter", false);
    DEBUG_REQUEST_REGISTER("IMUModel:reset_representation", "reset representation", false);
    DEBUG_REQUEST_REGISTER("IMUModel:reloadParameters", "", false);
    DEBUG_REQUEST_REGISTER("IMUModel:enableFilterWhileWalking", "enables filter update while walking", false);

    DEBUG_REQUEST_REGISTER("IMUModel:drawOnlyOrientation","if activated it only draws the orientation (x,y)",false);
    DEBUG_REQUEST_REGISTER("IMUModel:enablePlotsAndDrawings", "enables plots and the drawings in 3d viewer", false);

    ukf_acc_global.P = Eigen::Matrix<double,3,3>::Identity(); // covariance matrix of current state
    ukf_rot.P        = Eigen::Matrix<double,6,6>::Identity(); // covariance matrix of current state

    getDebugParameterList().add(&imuParameters);

    reloadParameters();
}

IMUModel::~IMUModel()
{
    getDebugParameterList().remove(&imuParameters);
}

void IMUModel::execute()
{
    DEBUG_REQUEST("IMUModel:reset_filter",
                  ukf_rot.reset();
                  ukf_acc_global.reset();
                  integrated=Eigen::Quaterniond(1,0,0,0);
    );

    DEBUG_REQUEST("IMUModel:reset_representation",
        getIMUData().reset();
    );

    DEBUG_REQUEST("IMUModel:reloadParameters",
        reloadParameters();
    );

    /* handle ukf filter for rotation */
    if(getMotionStatus().currentMotion == motion::walk){
        ukf_rot.Q = Q_rotation_walk;
    } else {
        ukf_rot.Q = Q_rotation;
    }

    ukf_rot.generateSigmaPoints();

    Eigen::Vector3d u_rot(0,0,0); // TODO: use generated angular acceleration as control vector?
    ukf_rot.predict(u_rot, getRobotInfo().getBasicTimeStepInSecond());

    // don't generate sigma points again because the process noise would be applied a second time
    // ukf.generateSigmaPoints();

    Eigen::Vector3d gyro;
    gyro << getGyrometerData().data.x, getGyrometerData().data.y, getGyrometerData().data.z;
    Eigen::Vector3d acceleration = Eigen::Vector3d(getAccelerometerData().data.x, getAccelerometerData().data.y, getAccelerometerData().data.z);

    IMU_RotationMeasurement z;
    z << acceleration.normalized(), gyro;

    if(getMotionStatus().currentMotion == motion::walk) {
        if(imuParameters.enableWhileWalking) {
            ukf_rot.update(z, R_rotation_walk);
        }
    } else {
        ukf_rot.update(z, R_rotation);
    }
    /* rotation ukf end */

    /* handle ukf filter for global acceleration */
    // transform acceleration measurement into global reference frame
    // TODO: Odometry as location measurement?
    // TODO: velocity of trunk in supfoot / local robot frame as velocity measurement
    // TODO: really needs bias removal or "calibration" of g
    IMU_AccMeasurementGlobal z_acc = ukf_rot.state.getRotationAsQuaternion()._transformVector(acceleration);

    if(getMotionStatus().currentMotion == motion::walk) {
        ukf_acc_global.Q = Q_acc_walk;
    } else {
        ukf_acc_global.Q = Q_acc;
    }

    ukf_acc_global.generateSigmaPoints();

    Eigen::Vector3d u_acc(0,0,0); // TODO: use generated jerk as control vector?
    ukf_acc_global.predict(u_acc, getRobotInfo().getBasicTimeStepInSecond());

    if(getMotionStatus().currentMotion == motion::walk) {
        if(imuParameters.enableWhileWalking) {
            ukf_acc_global.update(z_acc, R_acc_walk);
        }
    } else {
        ukf_acc_global.update(z_acc, R_acc);
    }
    /* acc ukf end */

    writeIMUData();

    DEBUG_REQUEST("IMUModel:enablePlotsAndDrawings",
        plots();
    );

    lastFrameInfo = getFrameInfo();
}

void IMUModel::writeIMUData()
{
    // raw sensor values
    getIMUData().rotational_velocity_sensor = getGyrometerData().data;
    getIMUData().acceleration_sensor        = getAccelerometerData().data;

    // global position data
    // TODO: check for correct integration
    // TODO: prediction or state?
    getIMUData().acceleration.x = ukf_acc_global.state.acceleration()(0,0);
    getIMUData().acceleration.y = ukf_acc_global.state.acceleration()(1,0);
    getIMUData().acceleration.z = ukf_acc_global.state.acceleration()(2,0) - 9.81; //Math::g

    getIMUData().location += getIMUData().velocity * getRobotInfo().getBasicTimeStepInSecond() + getIMUData().acceleration * getRobotInfo().getBasicTimeStepInSecond() * getRobotInfo().getBasicTimeStepInSecond() * 0.5;
    getIMUData().velocity += getIMUData().acceleration * getRobotInfo().getBasicTimeStepInSecond();

    // the state we are estimating in ukf_rot is 20 ms in the past. so predict 20 ms as estimate for the real current state
    UKF<RotationState<Measurement<6>,6> > sensor_delay_corrected_rot = ukf_rot;
    sensor_delay_corrected_rot.generateSigmaPoints();
    Eigen::Vector3d u_rot(0,0,0);
    sensor_delay_corrected_rot.predict(u_rot, 2 * getRobotInfo().getBasicTimeStepInSecond());

    // store rotation in IMUData as a rotation vector
    getIMUData().rotation = eigenVectorToVector3D(sensor_delay_corrected_rot.state.rotation());
    RotationMatrix bodyIntoGlobalMapping(getIMUData().rotation);

    /*
     * Note: the following code lines use the inverse mapping, i.e. globalIntoBodyMapping, by using the third row of bodyIntoGlobalMapping's matrix representation
     * Note: The negative of the determined angles is the correct solution in this case because the projected "global" z axis is not pointing upwards in the torso's coordinate system.
     * The projected "global" z axis in the Body frame is NOT (0,0,1)!
     * So actually we want the inverse mapping.
     * Inverting the angle is sufficient because it's basically only a 2D rotation in the XZ or YZ plane
     * Note: using bodyIntoGlobalMapping would be wrong because the global frame can be rotated around z regarding the body
     * this would result in a redistribution of the inclination on the x,y axis.
     * (e.g. z rotation about 90 degrees -> a rotation about the body's y axis becomes a rotation about the global x axis)
     * Note: don't use the y axis to determine the inclination in YZ plane because it might be mapped onto the x axis (a rotation about 90° around z)
     * this results in huge devation of the angles determined by atan2 because the projected y axis might end up in the second or third quadrant of the YZ plane
     */

    Eigen::Vector3d global_Z_in_body(bodyIntoGlobalMapping[0].z, bodyIntoGlobalMapping[1].z, bodyIntoGlobalMapping[2].z);
    getIMUData().orientation_rotvec = quaternionToVector3D(Eigen::Quaterniond::FromTwoVectors(global_Z_in_body, Eigen::Vector3d(0,0,1)));
    RotationMatrix bodyIntoGlobalMappingWithoutZ(getIMUData().orientation_rotvec);

    getIMUData().orientation = Vector2d(-atan2(bodyIntoGlobalMappingWithoutZ[2].y, bodyIntoGlobalMappingWithoutZ[1].y),
                                        -atan2(-bodyIntoGlobalMappingWithoutZ[2].x, bodyIntoGlobalMappingWithoutZ[0].x));

    PLOT("IMUModel:State:orientation:x", Math::toDegrees(getIMUData().orientation.x));
    PLOT("IMUModel:State:orientation:y", Math::toDegrees(getIMUData().orientation.y));

    getIMUData().rotational_velocity.x = ukf_rot.state.rotational_velocity()(0,0);
    getIMUData().rotational_velocity.y = ukf_rot.state.rotational_velocity()(1,0);
    getIMUData().rotational_velocity.z = ukf_rot.state.rotational_velocity()(2,0);
}

void IMUModel::plots()
{
    // --- for testing integration
        Eigen::Matrix3d rot_vel_mat;
        // getGyrometerData().data.z is inverted!
        rot_vel_mat << 1                             , -getGyrometerData().data.z*0.01,  getGyrometerData().data.y*0.01,
                       getGyrometerData().data.z*0.01,                               1, -getGyrometerData().data.x*0.01,
                      -getGyrometerData().data.y*0.01,  getGyrometerData().data.x*0.01,                               1;

        // continue rotation assuming constant velocity
        integrated = integrated * Eigen::Quaterniond(rot_vel_mat);

        Vector3d translation(0,250,250);
        LINE_3D(ColorClasses::red, translation, translation + quaternionToVector3D(integrated) * 100.0);
        LINE_3D(ColorClasses::blue, translation, translation + quaternionToVector3D(ukf_rot.state.getRotationAsQuaternion()) * 100.0);

        PLOT("IMUModel:Error:relative_to_pure_integration[°]", Math::toDegrees(Eigen::AngleAxisd(integrated*ukf_rot.state.getRotationAsQuaternion().inverse()).angle()));
    // --- end

    Eigen::Vector3d temp2 = ukf_rot.state.rotation();
    Vector3d rot_vec(temp2(0),temp2(1),temp2(2));
    RotationMatrix rot(rot_vec);

    DEBUG_REQUEST("IMUModel:drawOnlyOrientation",
        rot = RotationMatrix(getIMUData().orientation_rotvec);
    );

    Pose3D pose(rot);
    pose.translation = Vector3d(0,0,250);

    // plot gravity
    Eigen::Vector3d acceleration;
    acceleration << getAccelerometerData().data.x, getAccelerometerData().data.y, getAccelerometerData().data.z;
    Eigen::Vector3d temp3 = ukf_rot.state.getRotationAsQuaternion()._transformVector(acceleration);
    Vector3d gravity(6*temp3(0),6*temp3(1),6*temp3(2));

    COLOR_CUBE(30, pose);
    LINE_3D(ColorClasses::black, pose.translation, pose.translation + gravity*10.0);

    PLOT("IMUModel:Measurement:rotational_velocity:x", getGyrometerData().data.x);
    PLOT("IMUModel:Measurement:rotational_velocity:y", getGyrometerData().data.y);
    PLOT("IMUModel:Measurement:rotational_velocity:z", getGyrometerData().data.z);

    PLOT("IMUModel:State:orientation:x", Math::toDegrees(getIMUData().orientation.x));
    PLOT("IMUModel:State:orientation:y", Math::toDegrees(getIMUData().orientation.y));
}

void IMUModel::reloadParameters()
{   /* parameters for the acceleration filter */
    // process noise
    Q_acc.setIdentity();
    Q_acc *= imuParameters.acceleration.stand.processNoiseAcc;

    Q_acc_walk.setIdentity();
    Q_acc_walk *= imuParameters.acceleration.walk.processNoiseAcc;

    // measurement covariance matrix
    R_acc.setIdentity();
    R_acc *= imuParameters.acceleration.stand.measurementNoiseAcc;

    R_acc_walk.setIdentity();
    R_acc_walk *= imuParameters.acceleration.walk.measurementNoiseAcc;

    /* parameters for the rotation filter */
    // process noise
    Q_rotation.setIdentity();
    Q_rotation.block<3,3>(0,0) *= imuParameters.rotation.stand.processNoiseRot;
    Q_rotation.block<3,3>(3,3) *= imuParameters.rotation.stand.processNoiseGyro;

    Q_rotation_walk.setIdentity();
    Q_rotation_walk.block<3,3>(0,0) *= imuParameters.rotation.walk.processNoiseRot;
    Q_rotation_walk.block<3,3>(3,3) *= imuParameters.rotation.walk.processNoiseGyro;

    // measurement covariance matrix
    R_rotation.setIdentity();
    R_rotation.block<3,3>(0,0) *= imuParameters.rotation.stand.measurementNoiseAcc;
    R_rotation.block<3,3>(3,3) *= imuParameters.rotation.stand.measurementNoiseGyro;

    R_rotation_walk.setIdentity();
    R_rotation_walk.block<3,3>(0,0) *= imuParameters.rotation.walk.measurementNoiseAcc;
    R_rotation_walk.block<3,3>(3,3) *= imuParameters.rotation.walk.measurementNoiseGyro;
}
