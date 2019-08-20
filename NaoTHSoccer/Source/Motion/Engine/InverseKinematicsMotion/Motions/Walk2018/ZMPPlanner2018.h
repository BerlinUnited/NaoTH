/**
* @file FootStepPlanner.h
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* plan the foot step according to motion request
*/

#ifndef _ZMP_PLANNER_2018_H
#define _ZMP_PLANNER_2018_H

#include <ModuleFramework/Module.h>

#include "FootStep.h"

#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Infrastructure/RobotInfo.h"
#include "Representations/Motion/Walk2018/ZMPReferenceBuffer.h"
#include "Representations/Motion/Walk2018/StepBuffer.h"

#include "Representations/Motion/Walk2018/Walk2018Parameters.h"

#include "Tools/Debug/DebugPlot.h"
#include "Tools/Debug/DebugDrawings.h"
#include "Tools/Debug/DebugRequest.h"

BEGIN_DECLARE_MODULE(ZMPPlanner2018)
    PROVIDE(DebugPlot)
    PROVIDE(DebugRequest)
    PROVIDE(DebugDrawings)

    REQUIRE(Walk2018Parameters)

    REQUIRE(FrameInfo)
    REQUIRE(RobotInfo)

    PROVIDE(StepBuffer)         // reason: increasing planning cycle
    PROVIDE(ZMPReferenceBuffer)
END_DECLARE_MODULE(ZMPPlanner2018)

class ZMPPlanner2018 : private ZMPPlanner2018Base
{
  public:
    ZMPPlanner2018() : parameters(getWalk2018Parameters().zmpPlanner2018Params){}

    void init(size_t initial_number_of_cycles, Vector3d initialZMP, Vector3d targetZMP);

    virtual void execute();

    /**
     * @brief just set the ZMP on support foot
     */
    static Vector2d simplest(const FootStep& step, double offsetX, double offsetY);

    static Vector2d bezierBased(const FootStep step, double cycle, double samplesDoubleSupport, double samplesSingleSupport,
                                double offsetX, double offsetY,double inFootScalingY, double inFootSpacing, double transitionScaling);

  private:
    Vector3d calculateStableCoMByFeet(InverseKinematic::FeetPose feet, double offsetX) const;

    static std::vector<Vector2d> FourPointBezier2D(const std::vector<Vector2d>& polygon, unsigned int number_of_samples){
        std::vector<Vector2d> trajectory;

        for(int i = 0; static_cast<double>(i)/static_cast<double>(number_of_samples) <= 1; ++i){
            double t = static_cast<double>(i)/static_cast<double>(number_of_samples);

            trajectory.push_back(  polygon[0] * ((1-t)*(1-t)*(1-t))
                                 + polygon[1] * (3*t*(1-t)*(1-t))
                                 + polygon[2] * (3*t*t*(1-t))
                                 + polygon[3] * (t*t*t));
        }

        return  trajectory;
    }

  public:
    const ZMPPlanner2018Parameters& parameters;

    double zmpOffsetY, newZMPOffsetY, zmpOffsetX, newZMPOffsetX;
};

#endif // _ZMP_PLANNER_2018_H
