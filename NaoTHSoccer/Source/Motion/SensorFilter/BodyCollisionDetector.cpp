/*
  This module should detect if and in which direction the robot was pushed. It should not calculate and set correction steps.
  The implementation is not finished and in a questionable stage. A proper Implementation should have different mechanisms for detecting pushing during walk and stand.

  To test the implementation the debug request draw_impact_vector can be used. It should probably be adjusted. This debug request should either draw the direction of the
  push or the opposite direction.
*/
#include "BodyCollisionDetector.h"

BodyCollisionDetector::~BodyCollisionDetector()
{
}

void BodyCollisionDetector::execute()
{
  // TODO explain the idea behind this implementation
  // TODO explaing the implementation
  if (torsoParams.useSteffensInertial) {
    inertial = getIMUData().orientation;
  }
  else {
    inertial = getInertialModel().orientation;
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
  buffer.add(Vector2d(relativeRotation.getXAngle(), rotationY));  // WTF

  // TODO implement different handlings for different motion states (walking, standing, ..)
  /*
  if (!(getMotionStatus().currentMotion == motion::walk))
  {
      buffer.clear();
      return;
  }
  */

  // TODO why is this if statement like this. 
  if (buffer.isFull() && frameDelay > 0 && frameDelay < buffer.size())
  {
    // this looks like a PID controller. It seems the idea was to detect pushing by the magnitude and direction of the correction vector
    // the same can be achieved with the error vector
    const Vector2d requestedVelocity = (buffer[frameDelay - 1] - buffer[frameDelay]) / getRobotInfo().getBasicTimeStepInSecond();
    const Vector2d error = requestedVelocity - filteredGyro;
    const Vector2d errorDerivative = (error - lastGyroError) / getRobotInfo().getBasicTimeStepInSecond();

    double correctionY = torsoParams.rotation.VelocityP.y * error.y +
      torsoParams.rotation.D.y * errorDerivative.y;

    double correctionX = torsoParams.rotation.VelocityP.x * error.x +
      torsoParams.rotation.D.x * errorDerivative.x;

    correctionX += torsoParams.rotation.P.x * inertial.x;
    correctionY += torsoParams.rotation.P.y * inertial.y;

    PLOT("BodyCollisionDetector:correctionX", correctionX);
    PLOT("BodyCollisionDetector:correctionY", correctionY);

    // TODO: this does not seem to work correctly. The Arrow does not point in the correct direction yet
    DEBUG_REQUEST("BodyCollisionDetector:drawImpactVector",
      draw_impact_vector(Vector2d(correctionX, correctionY));
    );

    //Collision Detection Math goes here 


    lastGyroError = error;
    return;
  }
}

void BodyCollisionDetector::draw_impact_vector(Vector2d vec)
{
  // TODO make the arrow more visible
  FIELD_DRAWING_CONTEXT;
    PEN("FF0000", 10);
    //vec = vec.normalize(500);
    ARROW(0, 0, vec.x * 10000, vec.y * 10000);
}