#include "bodyCollisionDetector.h"

bodyCollisionDetector::~bodyCollisionDetector()
{
}

void bodyCollisionDetector::execute()
{
    Vector2d inertial = getInertialModel().orientation;
    if (torsoParams.useSteffensInertial) {
        inertial = getIMUData().orientation;
    }

    Vector2d gyro = Vector2d(getGyrometerData().data.x, getGyrometerData().data.y);
    static Vector2d filteredGyro = gyro;

    const double alpha = torsoParams.gyroFilterAlpha;
    filteredGyro = filteredGyro * (1.0f - alpha) + gyro * alpha;

    const double observerMeasurementDelay = 40;
    const int frameDelay = static_cast<int>(observerMeasurementDelay / (getRobotInfo().getBasicTimeStepInSecond() * 1000));

    static RingBuffer<Vector2d, 10> buffer;
    static Vector2d lastGyroError;
    static RotationMatrix lastBodyRotationMatrix = getTargetHipFeetPose().pose.hip.rotation;

    const RotationMatrix relativeRotation = getTargetHipFeetPose().pose.hip.rotation.invert() * lastBodyRotationMatrix;
    lastBodyRotationMatrix = getTargetHipFeetPose().pose.hip.rotation;

    const double rotationY = atan2(relativeRotation.c[2].x, relativeRotation.c[2].z);
    buffer.add(Vector2d(relativeRotation.getXAngle(), rotationY));

    if (!(getMotionStatus().currentMotion == motion::walk))
    {
        buffer.clear();
        return;
    }

    if (buffer.isFull() && frameDelay > 0 && frameDelay < buffer.size())
    {
        const Vector2d requestedVelocity = (buffer[frameDelay - 1] - buffer[frameDelay]) / getRobotInfo().getBasicTimeStepInSecond();
        const Vector2d error = requestedVelocity - filteredGyro;
        const Vector2d errorDerivative = (error - lastGyroError) / getRobotInfo().getBasicTimeStepInSecond();

        double correctionY = torsoParams.rotation.VelocityP.y * error.y +
            torsoParams.rotation.D.y * errorDerivative.y;

        double correctionX = torsoParams.rotation.VelocityP.x * error.x +
            torsoParams.rotation.D.x * errorDerivative.x;

        correctionX += torsoParams.rotation.P.x * inertial.x;
        correctionY += torsoParams.rotation.P.y * inertial.y;

        PLOT("bodyCollisionDetector:correctionX", correctionX);
        PLOT("bodyCollisionDetector:correctionY", correctionY);

        //Collision Detection Math goes here


        lastGyroError = error;
        return;
    }

}