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

// TODO: which one is better?
#include "Tools/DataStructures/Spline.h"
#include "Tools/Math/CubicSpline.h"


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
    FootTrajectoryGenerator2018() 
      : parameters(getWalk2018Parameters().footTrajectoryGenerator2018Params) 
    {
      // Generate reference splines for step trajectories. 
      // Those splines are adjusted to the different steps.

      // reference points for splines in xy
      const std::vector<double> t_XY { 0.0, 0.125, 0.25, 0.5, 0.75, 0.875, 1.0 };
      const std::vector<double> f_XY {
        0.0,
        (1 - cos(0.125*Math::pi))*0.5,
        (1 - cos(0.25 *Math::pi))*0.5,
        (1 - cos(0.5  *Math::pi))*0.5,
        (1 - cos(0.75 *Math::pi))*0.5,
        (1 - cos(0.875*Math::pi))*0.5,
        1.0
      };

      theCubicSplineXY.set_boundary(tk::spline::first_deriv, 0.0, tk::spline::first_deriv,0.0, false);
      theCubicSplineXY.set_points(t_XY, f_XY);

      
      // reference points for splines in z
      const std::vector<double> t_Z { 0.0, 0.125, 0.25, 0.5, 0.75, 0.875, 1.0 };
      const std::vector<double> f_Z {
        0.0,
        (1 - cos(0.125*Math::pi2))*0.5,
        (1 - cos(0.25 *Math::pi2))*0.5,
        1.0,
        (1 - cos(0.75 *Math::pi2))*0.5,
        (1 - cos(0.875*Math::pi2))*0.5,
        0.0
      };

      theCubicSplineZ.set_boundary(tk::spline::first_deriv ,0.0, tk::spline::first_deriv,0.0, false);
      theCubicSplineZ.set_points(t_Z, f_Z);
    }

    virtual void execute();

  private:

    // use the following functions to generate the trajectory and calculate the nex pos for the current step
    Pose3D calculateLiftingFootPos(const Step& step) const;


    // LEGACY (uses sin and cos for trajectory generation)
    // genera trajectory for STEP_CONTROL using sin and cos
    Pose3D stepControl(const Pose3D& oldFoot, const Pose3D& targetFoot,
                        double cycle, double samplesDoubleSupport, double samplesSingleSupport,
                        double stepHeight, double footPitchOffset, double footRollOffset,
                        double speedDirection, double scale) const;


    // adjusted for forward kicks
    // TODO: check if the sideicks still work
    Pose3D stepControlNew(const FootStep& step,
                          double cycle, double samplesDoubleSupport, double samplesSingleSupport,
                          double stepHeight, double footPitchOffset, double footRollOffset,
                          double speedDirection, double scale, double sidekick_width) const;


    // LEGACY (uses sin and cos for trajectory generation)
    // generte a trajectory for WALK steps
    Pose3D genTrajectory(const Pose3D& oldFoot, const Pose3D& targetFoot,
                         double cycle, double samplesDoubleSupport, double samplesSingleSupport,
                         double stepHeight, double footPitchOffset, double footRollOffset ) const;


    Pose3D genTrajectoryWithSplines(const Pose3D& oldFoot, const Pose3D& targetFoot,
                                    double cycle, double duration,
                                    double stepHeight, double footPitchOffset, double footRollOffset ) const;


  private:
    tk::spline theCubicSplineXY;
    tk::spline theCubicSplineZ;

  public:
    const FootTrajectoryGenerator2018Parameters &parameters;
};


#endif  /* _FOOT_TRAJECTORY_GENERATOR_2018_H */
