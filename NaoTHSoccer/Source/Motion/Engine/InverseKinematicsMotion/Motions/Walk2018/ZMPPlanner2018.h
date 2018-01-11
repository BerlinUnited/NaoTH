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

#include "Tools/Debug/DebugPlot.h"
#include "Tools/Debug/DebugDrawings.h"
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugParameterList.h"

BEGIN_DECLARE_MODULE(ZMPPlanner2018)
    PROVIDE(DebugPlot)
    PROVIDE(DebugRequest)
    PROVIDE(DebugDrawings)
    PROVIDE(DebugParameterList)

    REQUIRE(FrameInfo)
    REQUIRE(RobotInfo)

    PROVIDE(StepBuffer)         // reason: increasing planning cycle
    PROVIDE(ZMPReferenceBuffer)
END_DECLARE_MODULE(ZMPPlanner2018)

class ZMPPlanner2018 : private ZMPPlanner2018Base
{
  public:
    ZMPPlanner2018(){
      getDebugParameterList().add(&parameters);
    }

    virtual ~ZMPPlanner2018();

    void init(int initial_number_of_cycles, Vector3d initialZMP, Vector3d targetZMP);

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

    class Parameters: public ParameterList{
    public:
      Parameters() : ParameterList("ZMPPlanner2018")
      {
          PARAMETER_REGISTER(bezierZMP.inFootScalingY) = 1;
          PARAMETER_REGISTER(bezierZMP.inFootSpacing)  = 10;
          PARAMETER_REGISTER(bezierZMP.transitionScaling) = 0.6;
          PARAMETER_REGISTER(bezierZMP.offsetX) = 15;
          PARAMETER_REGISTER(bezierZMP.offsetY) = 0;
          PARAMETER_REGISTER(bezierZMP.offsetXForKicks) = 0;
          PARAMETER_REGISTER(bezierZMP.offsetYForKicks) = -7;

          PARAMETER_REGISTER(simpleZMP.offsetX) = 20;
          PARAMETER_REGISTER(simpleZMP.offsetY) = -7;
          PARAMETER_REGISTER(simpleZMP.kickOffsetY)  = -7;

          PARAMETER_REGISTER(comHeight) = 260;
          PARAMETER_REGISTER(ZMPOffsetYByCharacter) = 0;
          PARAMETER_REGISTER(newZMP_ON) = true;

          PARAMETER_REGISTER(stabilization.maxHipOffsetBasedOnStepLength.x) = 5;
          PARAMETER_REGISTER(stabilization.maxHipOffsetBasedOnStepLength.y) = 0;
          PARAMETER_REGISTER(stabilization.maxHipOffsetBasedOnStepLengthForKicks.x) = 0;
          PARAMETER_REGISTER(stabilization.maxHipOffsetBasedOnStepLengthForKicks.y) = 0;

          syncWithConfig();
      }

      struct Bezier {
        double inFootScalingY;
        double inFootSpacing;
        double transitionScaling;

        double offsetX;
        double offsetY;
        double offsetXForKicks;
        double offsetYForKicks;
      } bezierZMP;

      struct Simple {
        double offsetX;
        double offsetY;
        double kickOffsetY;
      } simpleZMP;

      struct Stabilization {
        Vector2d maxHipOffsetBasedOnStepLength;
        Vector2d maxHipOffsetBasedOnStepLengthForKicks;
      } stabilization;

      bool newZMP_ON;

      double comHeight;
      double ZMPOffsetYByCharacter;
    } parameters;

    double zmpOffsetY, newZMPOffsetY, zmpOffsetX, newZMPOffsetX;
};

#endif // _ZMP_PLANNER_2018_H
