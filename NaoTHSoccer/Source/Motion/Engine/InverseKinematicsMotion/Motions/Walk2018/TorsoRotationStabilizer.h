/**
 * @File:   FootTrajectoryGenerator.h
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 * foot trajectory for walking
 */

#ifndef _TORSO_ROTATION_STABILIZER_H
#define _TORSO_ROTATION_STABILIZER_H

#include <ModuleFramework/Module.h>

#include "Representations/Motion/Walk2018/StepBuffer.h"
#include "Representations/Motion/Walk2018/TargetHipFeetPose.h"
#include "Representations/Motion/Walk2018/Walk2018Parameters.h"
#include "Representations/Infrastructure/CalibrationData.h"
#include "Representations/Modeling/IMUData.h"
#include "Representations/Modeling/InertialModel.h"
#include "Representations/Infrastructure/GyrometerData.h"
#include "Representations/Infrastructure/RobotInfo.h"

#include "Tools/Debug/DebugPlot.h"
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugParameterList.h"

BEGIN_DECLARE_MODULE(TorsoRotationStabilizer)
    PROVIDE(DebugPlot)
    PROVIDE(DebugRequest)

    REQUIRE(Walk2018Parameters)

    REQUIRE(FrameInfo)
    REQUIRE(CalibrationData)
    REQUIRE(InertialModel)
    REQUIRE(IMUData)
    REQUIRE(GyrometerData)
    REQUIRE(RobotInfo)
    REQUIRE(StepBuffer)

    PROVIDE(TargetHipFeetPose)
END_DECLARE_MODULE(TorsoRotationStabilizer)

class TorsoRotationStabilizer : private TorsoRotationStabilizerBase
{
  public:
    TorsoRotationStabilizer() : parameters(getWalk2018Parameters().torsoRotationStabilizerParams) {}

  virtual void execute(){
    if(getCalibrationData().calibrated && parameters.rotationStabilize) {
      if(getStepBuffer().first().footStep.liftingFoot() == FootStep::LEFT) {
        getTargetHipFeetPose().pose.localInRightFoot();
      } else if(getStepBuffer().first().footStep.liftingFoot() == FootStep::RIGHT) {
        getTargetHipFeetPose().pose.localInLeftFoot();
      } else {
        getTargetHipFeetPose().pose.localInHip();
      }


      Vector2d inertial = getInertialModel().orientation;
      if(parameters.useSteffensInertial) {
        inertial = getIMUData().orientation;
      }
      rotationStabilize(
        inertial,
        getGyrometerData(),
        getRobotInfo().getBasicTimeStepInSecond(),
        parameters.rotation.P,
        parameters.rotation.VelocityP,
        parameters.rotation.D,
        getTargetHipFeetPose().pose
      );
    }
  }

  private:
      bool rotationStabilize(
        //const InertialModel& theInertialModel,
        const Vector2d& inertial,
        const GyrometerData& theGyrometerData,
        const double timeDelta,
        const Vector2d&  rotationP,
        const Vector2d&  rotationVelocityP,
        const Vector2d&  rotationD,
        InverseKinematic::HipFeetPose& p)
      {
        Vector2d gyro = Vector2d(theGyrometerData.data.x, theGyrometerData.data.y);
        static Vector2d filteredGyro = gyro;

        const double alpha = parameters.gyroFilterAlpha;
        filteredGyro = filteredGyro * (1.0f - alpha) + gyro * alpha;

        const double observerMeasurementDelay = 40;
        const int frameDelay = static_cast<int>(observerMeasurementDelay / (timeDelta*1000));

        static RingBuffer<Vector2d, 10> buffer;
        static Vector2d lastGyroError;
        static RotationMatrix lastBodyRotationMatrix = p.hip.rotation;

        const RotationMatrix relativeRotation = p.hip.rotation.invert() * lastBodyRotationMatrix;
        lastBodyRotationMatrix = p.hip.rotation;

        const double rotationY = atan2(relativeRotation.c[2].x, relativeRotation.c[2].z);
        buffer.add(Vector2d(relativeRotation.getXAngle(), rotationY));

        if(buffer.isFull() && frameDelay > 0 && frameDelay < buffer.size())
        {
          const Vector2d requestedVelocity = (buffer[frameDelay-1] - buffer[frameDelay]) / timeDelta;
          const Vector2d error = requestedVelocity - filteredGyro;
          const Vector2d errorDerivative = (error - lastGyroError) / timeDelta;

          double correctionY = rotationVelocityP.y * error.y +
                               rotationD.y * errorDerivative.y;

          double correctionX =rotationVelocityP.x * error.x +
                              rotationD.x * errorDerivative.x;

          correctionX += rotationP.x * inertial.x;
          correctionY += rotationP.y * inertial.y;

          PLOT("torsoRotationStabilizer:correctionX", correctionX);
          PLOT("torsoRotationStabilizer:correctionY", correctionY);

          if(parameters.localRotationCalibration) {
            p.localInHip();
            p.hip.rotateX(correctionX);
            p.hip.rotateY(correctionY);
          } else {
            double height = NaoInfo::ThighLength + NaoInfo::TibiaLength + NaoInfo::FootHeight;
            p.hip.translate(0, 0, -height);
            p.hip.rotateX(correctionX);
            p.hip.rotateY(correctionY);
            p.hip.translate(0, 0, height);
          }

          lastGyroError = error;
        }
        return true;
      }

  public:
      const TorsoRotationStabilizerParameters& parameters;
};

#endif  /* _TORSO_ROTATION_STABILIZER_H */
