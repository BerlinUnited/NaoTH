/**
 * @File:   FootTrajectoryGenerator.h
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 * foot trajectory for walking
 */

#ifndef _FEET_STABILIZER_H
#define _FEET_STABILIZER_H

#include <ModuleFramework/Module.h>

#include "Representations/Motion/Walk2018/Walk2018Parameters.h"

#include "Representations/Motion/Walk2018/StepBuffer.h"
#include "Representations/Modeling/InertialModel.h"
#include "Representations/Infrastructure/GyrometerData.h"
#include "Representations/Infrastructure/RobotInfo.h"
#include "Representations/Infrastructure/JointData.h"

#include "Tools/Debug/DebugPlot.h"
#include "Tools/Debug/DebugRequest.h"

BEGIN_DECLARE_MODULE(FeetStabilizer)
    PROVIDE(DebugPlot)
    PROVIDE(DebugRequest)

    REQUIRE(Walk2018Parameters)

    REQUIRE(FrameInfo)
    REQUIRE(InertialModel)
    REQUIRE(GyrometerData)
    REQUIRE(RobotInfo)
    REQUIRE(StepBuffer)

    PROVIDE(MotorJointData)
END_DECLARE_MODULE(FeetStabilizer)


class FeetStabilizer : private FeetStabilizerBase
{
  public:
    FeetStabilizer() : parameters(getWalk2018Parameters().feetStabilizerParams) {}

    const FeetStabilizerParameters& parameters;

    virtual void execute()
    {
        if (!parameters.stabilizeFeet) return;

        const Step& executingStep = getStepBuffer().first();

        double samplesDoubleSupport = executingStep.samplesDoubleSupport;
        double samplesSingleSupport = executingStep.samplesSingleSupport;

        double doubleSupportEnd = samplesDoubleSupport / 2;
        double doubleSupportBegin = samplesDoubleSupport / 2 + samplesSingleSupport;

        double cycle = executingStep.executingCycle;
        double z = 0;

        if (cycle > doubleSupportEnd && cycle <= doubleSupportBegin)
        {
          double t = 1 - (doubleSupportBegin - cycle) / samplesSingleSupport;
          t = t * Math::pi - Math::pi_2; // scale t
          z = (1 + cos(t * 2))*0.5;
        }

        const Vector2d& inertial = getInertialModel().orientation;
        const Vector3d& gyro     = getGyrometerData().data;

        // HACK: small filter...
        const double alpha = parameters.gyroFilterAlpha;
        filteredGyro = filteredGyro * (1.0f - alpha) + gyro * alpha;

        Vector2d weight;
        weight.x =
            parameters.P.x * inertial.x
          + parameters.D.x * filteredGyro.x;

        weight.y =
            parameters.P.y * inertial.y
          + parameters.D.y * filteredGyro.y;

        switch(executingStep.footStep.liftingFoot()) {
            case FootStep::LEFT:
              // adjust left
              getMotorJointData().position[JointData::LAnkleRoll]  -= inertial.x*z;
              getMotorJointData().position[JointData::LAnklePitch] -= inertial.y*z;

              // stabilize right
              getMotorJointData().position[JointData::RAnkleRoll]  += weight.x*z;
              getMotorJointData().position[JointData::RAnklePitch] += weight.y*z;
              break;
          case FootStep::RIGHT:
              // adjust right
              getMotorJointData().position[JointData::RAnkleRoll]  -= inertial.x*z;
              getMotorJointData().position[JointData::RAnklePitch] -= inertial.y*z;

              // stabilize left
              getMotorJointData().position[JointData::LAnkleRoll]  += weight.x*z;
              getMotorJointData().position[JointData::LAnklePitch] += weight.y*z;
              break;
          default:
              break; // don't stabilize in double support mode
        };
    }

    void reset(){
        filteredGyro = Vector3d();
    }

  private:
    Vector3d filteredGyro;
};

#endif  /* _FEET_STABILIZER_H */
