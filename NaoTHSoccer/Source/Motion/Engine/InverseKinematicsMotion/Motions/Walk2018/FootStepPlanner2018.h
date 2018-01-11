/**
* @file FootStepPlanner.h
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* plan the foot step according to motion request
*/

#ifndef _FOOT_STEP_PLANNER_2018_H
#define _FOOT_STEP_PLANNER_2018_H

#include <ModuleFramework/Module.h>

#include "FootStep.h"
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Infrastructure/RobotInfo.h"
#include "Representations/Motion/Request/MotionRequest.h"

#include "Representations/Motion/MotionStatus.h"
#include "Representations/Motion/Walk2018/CoMErrors.h"
#include "Representations/Motion/Walk2018/StepBuffer.h"

#include "Tools/Debug/DebugPlot.h"
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugParameterList.h"

BEGIN_DECLARE_MODULE(FootStepPlanner2018)
  PROVIDE(DebugPlot)
  PROVIDE(DebugRequest)
  PROVIDE(DebugParameterList)

  REQUIRE(FrameInfo)
  REQUIRE(RobotInfo)
  REQUIRE(MotionRequest)

  PROVIDE(MotionStatus) // reason: increasing stepControl.stepRequestID
  PROVIDE(CoMErrors)    // reason: clearing buffers after adaptStepSize or emergency stop
  PROVIDE(StepBuffer)
END_DECLARE_MODULE(FootStepPlanner2018)

class FootStepPlanner2018 : private FootStepPlanner2018Base
{
public:
  FootStepPlanner2018();
  virtual ~FootStepPlanner2018();

  virtual void execute();

  void init(int initial_number_of_cycles, InverseKinematic::FeetPose initialFeetPose);

private:
  void calculateNewStep(const Step& lastStep, Step& newStep, const WalkRequest& walkRequest);

  void adaptStepSize(FootStep& step) const;

  void updateParameters();

  FootStep nextStep(const FootStep& lastStep, const WalkRequest& req);

  FootStep controlStep(const FootStep& lastStep, const WalkRequest& req);
  
  FootStep finalStep(const FootStep& lastStep, const WalkRequest& req);

  FootStep zeroStep(const FootStep& lastStep) const;

  FootStep calculateNextWalkStep(const InverseKinematic::FeetPose& pose, const Pose2D& offset, const Pose2D& lastStepRequest, FootStep::Foot movingFoot, const WalkRequest& req, bool stepControl = false);

  FootStep firstStep(const InverseKinematic::FeetPose& pose, const Pose2D& offset, const Pose2D& lastStepRequest, const WalkRequest& req);

  void restrictStepSize(Pose2D& step, double character, bool stepControl) const;

  void restrictStepChange(Pose2D& step, const Pose2D& lastStep) const;

  class Parameters: public ParameterList{
    public:
      Parameters() : ParameterList("FootStepPlanner2018")
      {
          PARAMETER_ANGLE_REGISTER(limits.maxTurnInner) = 10;
          PARAMETER_ANGLE_REGISTER(limits.maxStepTurn)  = 30;
          PARAMETER_REGISTER(limits.maxStepLength)      = 50;
          PARAMETER_REGISTER(limits.maxStepLengthBack)  = 35;
          PARAMETER_REGISTER(limits.maxStepWidth)       = 60;
          PARAMETER_REGISTER(limits.maxStepChange)      = 0.3;

          PARAMETER_ANGLE_REGISTER(limits.maxCtrlTurn) = 30;
          PARAMETER_REGISTER(limits.maxCtrlLength) = 80;
          PARAMETER_REGISTER(limits.maxCtrlWidth)  = 50;

          PARAMETER_REGISTER(footOffsetY) = 0;

          PARAMETER_REGISTER(step.doubleSupportTime) = 0;
          PARAMETER_REGISTER(step.duration) = 260;
          PARAMETER_REGISTER(step.dynamicDuration) = true;

          PARAMETER_REGISTER(stabilization.dynamicStepSize)  = true;
          PARAMETER_REGISTER(stabilization.dynamicStepSizeP) = -0.1;
          PARAMETER_REGISTER(stabilization.dynamicStepSizeD) = 0.05;
          PARAMETER_REGISTER(stabilization.emergencyStopError)  = 250;
          PARAMETER_REGISTER(stabilization.maxEmergencyCounter) = 500;

          syncWithConfig();
      }

      struct Limits {
        double maxTurnInner;
        double maxStepTurn;
        double maxStepLength;
        double maxStepLengthBack;
        double maxStepWidth;
        double maxStepChange;

        double maxCtrlTurn;
        double maxCtrlLength;
        double maxCtrlWidth;
      } limits;

      struct Step {
        int  doubleSupportTime;
        int  duration;
        bool dynamicDuration;
      } step;

      struct Stabilization {
        bool   dynamicStepSize;
        double dynamicStepSizeP;
        double dynamicStepSizeD;

        double emergencyStopError;
        double maxEmergencyCounter;
      } stabilization;

      double footOffsetY;
  } parameters;

  double theFootOffsetY;

  double theMaxChangeTurn;
  double theMaxChangeX;
  double theMaxChangeY;

  unsigned int emergencyCounter;
};

#endif // _FOOT_STEP_PLANNER_2018_H
