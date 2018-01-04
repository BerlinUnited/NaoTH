/**
 * @File:   FootTrajectoryGenerator.h
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 * foot trajectory for walking
 */

#ifndef _FOOT_TRAJECTORY_GENERATOR_2018_H
#define _FOOT_TRAJECTORY_GENERATOR_2018_H

#include <ModuleFramework/Module.h>
#include "Tools/Math/Pose3D.h"

//#include "FootStep.h"
//#include "../IKParameters.h"
//#include "Motion/Engine/InverseKinematicsMotion/InverseKinematicsMotionEngine.h"

//#include <list>
//#include <vector>
//#include <map>

//#include "Tools/Math/Matrix3x3.h"
//#include "Tools/Math/Vector2.h"

#include "Representations/Motion/Walk2018/StepBuffer.h"
#include "Representations/Motion/Walk2018/TargetCoMFeetPose.h"
#include "Motion/Engine/InverseKinematicsMotion/InverseKinematicsMotionEngine.h"

#include "Tools/Debug/DebugPlot.h"
#include "Tools/Debug/DebugDrawings.h"
#include "Tools/Debug/DebugRequest.h"

BEGIN_DECLARE_MODULE(FootTrajectoryGenerator2018)
    PROVIDE(DebugPlot)
    PROVIDE(DebugRequest)
    PROVIDE(DebugDrawings)

    REQUIRE(FrameInfo)
    REQUIRE(InverseKinematicsMotionEngineService)
    REQUIRE(StepBuffer)
    REQUIRE(RobotInfo)

    PROVIDE(TargetCoMFeetPose)  // setting target foot positions
END_DECLARE_MODULE(FootTrajectoryGenerator2018)

class FootTrajectoryGenerator2018 : private FootTrajectoryGenerator2018Base
{
  public:
    FootTrajectoryGenerator2018(){}

    virtual void execute();

  private:
    Pose3D genTrajectory(const Pose3D& oldFoot, const Pose3D& targetFoot,
                         double cycle, double samplesDoubleSupport, double samplesSingleSupport,
                         double stepHeight, double footPitchOffset, double footRollOffset ) const;

    Pose3D stepControl(const Pose3D& oldFoot, const Pose3D& targetFoot,
                       double cycle, double samplesDoubleSupport, double samplesSingleSupport,
                       double stepHeight, double footPitchOffset, double footRollOffset,
                       double speedDirection, double scale) const;

    Pose3D genTrajectoryWithSplines(const Pose3D& oldFoot, const Pose3D& targetFoot,
                                    double cycle, double duration,
                                    double stepHeight, double footPitchOffset, double footRollOffset ) const;

    Pose3D calculateLiftingFootPos(const Step& step) const;
};


#endif  /* _FOOT_TRAJECTORY_GENERATOR_2018_H */
