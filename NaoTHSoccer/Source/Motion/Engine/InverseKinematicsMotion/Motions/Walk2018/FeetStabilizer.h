/**
 * @File:   FootTrajectoryGenerator.h
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 * foot trajectory for walking
 */

#ifndef _FEET_STABILIZER_H
#define _FEET_STABILIZER_H

#include <ModuleFramework/Module.h>

#include "Representations/Motion/Walk2018/StepBuffer.h"
#include "Representations/Motion/Walk2018/TargetHipFeetPose.h"
#include "Representations/Infrastructure/CalibrationData.h"
#include "Representations/Modeling/InertialModel.h"
#include "Representations/Infrastructure/GyrometerData.h"
#include "Representations/Infrastructure/RobotInfo.h"
#include "Representations/Infrastructure/JointData.h"

#include "Motion/Engine/InverseKinematicsMotion/InverseKinematicsMotionEngine.h"

#include "Tools/Debug/DebugPlot.h"
#include "Tools/Debug/DebugRequest.h"

BEGIN_DECLARE_MODULE(FeetStabilizer)
    PROVIDE(DebugPlot)
    PROVIDE(DebugRequest)

    REQUIRE(FrameInfo)
    REQUIRE(InertialModel)
    REQUIRE(GyrometerData)
    REQUIRE(RobotInfo)
    REQUIRE(StepBuffer)
    REQUIRE(InverseKinematicsMotionEngineService)

    PROVIDE(MotorJointData)
END_DECLARE_MODULE(FeetStabilizer)

class FeetStabilizer : private FeetStabilizerBase
{
  public:
    FeetStabilizer(){}

  virtual void execute(){
    const Step& executingStep = getStepBuffer().first();

    // the same as in "FootTrajectorGenerator::genTrajectory"
    // TODO: remove dupication
    double samplesDoubleSupport = std::max(0, (int) (getInverseKinematicsMotionEngineService().getEngine().getParameters().walk.step.doubleSupportTime / getRobotInfo().basicTimeStep));
    double samplesSingleSupport = executingStep.numberOfCycles - samplesDoubleSupport;
    ASSERT(samplesSingleSupport >= 0 && samplesDoubleSupport >= 0);

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
    static Vector3d lastGyro = gyro;
    Vector3d filteredGyro    = filteredGyro*0.8 + gyro*0.2;

    Vector2d weight;
    weight.x =
        getInverseKinematicsMotionEngineService().getEngine().getParameters().walk.stabilization.stabilizeFeetP.x * inertial.x
      + getInverseKinematicsMotionEngineService().getEngine().getParameters().walk.stabilization.stabilizeFeetD.x * filteredGyro.x;

    weight.y =
        getInverseKinematicsMotionEngineService().getEngine().getParameters().walk.stabilization.stabilizeFeetP.y * inertial.y
      + getInverseKinematicsMotionEngineService().getEngine().getParameters().walk.stabilization.stabilizeFeetD.y * filteredGyro.y;

    switch(executingStep.footStep.liftingFoot()) {
        case FootStep::LEFT:
          // adjust left
          getMotorJointData().position[JointData::LAnkleRoll] -= inertial.x*z;
          getMotorJointData().position[JointData::LAnklePitch] -= inertial.y*z;

          // stabilize right
          getMotorJointData().position[JointData::RAnkleRoll] += weight.x*z;
          getMotorJointData().position[JointData::RAnklePitch] += weight.y*z;
          break;
      case FootStep::RIGHT:
          // adjust right
          getMotorJointData().position[JointData::RAnkleRoll] -= inertial.x*z;
          getMotorJointData().position[JointData::RAnklePitch] -= inertial.y*z;

          // stabilize left
          getMotorJointData().position[JointData::LAnkleRoll] += weight.x*z;
          getMotorJointData().position[JointData::LAnklePitch] += weight.y*z;
          break;
      default:
          break; // don't stabilize in double support mode
    };

    lastGyro = gyro;
  }

};

#endif  /* _FEET_STABILIZER_H */
