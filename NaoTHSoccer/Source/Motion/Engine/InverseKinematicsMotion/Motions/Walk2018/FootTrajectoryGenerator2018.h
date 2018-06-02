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

#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Infrastructure/RobotInfo.h"

#include "Representations/Motion/Walk2018/StepBuffer.h"
#include "Representations/Motion/Walk2018/TargetCoMFeetPose.h"
#include "Representations/Motion/Walk2018/Walk2018Parameters.h"

#include "Tools/Debug/DebugPlot.h"
#include "Tools/Debug/DebugRequest.h"

#include "Tools/DataStructures/Spline.h"

BEGIN_DECLARE_MODULE(FootTrajectoryGenerator2018)
    PROVIDE(DebugPlot)
    PROVIDE(DebugRequest)
    REQUIRE(Walk2018Parameters)

    REQUIRE(FrameInfo)
    REQUIRE(StepBuffer)
    REQUIRE(RobotInfo)

    PROVIDE(TargetCoMFeetPose)  // setting target foot positions
END_DECLARE_MODULE(FootTrajectoryGenerator2018)

class FootTrajectoryGenerator2018 : private FootTrajectoryGenerator2018Base
{
  public:
    FootTrajectoryGenerator2018() : parameters(getWalk2018Parameters().footTrajectoryGenerator2018Params) {

    // reference points for splines in xy
    const std::vector<double> xA{ 0.0, 0.125, 0.25, 0.5, 0.75, 0.875, 1.0 };
    const std::vector<double> yA{
      0.0,
      (1 - cos(0.125*Math::pi))*0.5,
      (1 - cos(0.25 *Math::pi))*0.5,
      (1 - cos(0.5  *Math::pi))*0.5,
      (1 - cos(0.75 *Math::pi))*0.5,
      (1 - cos(0.875*Math::pi))*0.5,
      1.0
    };

    // reference points for splines in z
    const std::vector<double> xB{ 0.0, 0.125, 0.25, 0.5, 0.75, 0.875, 1.0 };
    const std::vector<double> yB{
      0.0,
      (1 - cos(0.125*Math::pi2))*0.5,
      (1 - cos(0.25 *Math::pi2))*0.5,
      1.0,
      (1 - cos(0.75 *Math::pi2))*0.5,
      (1 - cos(0.875*Math::pi2))*0.5,
      0.0
    };

        theCubicSplineXY.set_boundary(tk::spline::first_deriv,0.0, tk::spline::first_deriv,0.0, false);
        theCubicSplineXY.set_points(xA,yA);

        theCubicSplineZ.set_boundary(tk::spline::first_deriv,0.0, tk::spline::first_deriv,0.0, false);
        theCubicSplineZ.set_points(xB,yB);
    }

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

  private:
    tk::spline theCubicSplineXY;
    tk::spline theCubicSplineZ;

  public:
    const FootTrajectoryGenerator2018Parameters &parameters;
};


#endif  /* _FOOT_TRAJECTORY_GENERATOR_2018_H */
