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
#include "Representations/Motion/Walk2018/Walk2018Parameters.h"

#include "Representations/Modeling/CentreOfPressure.h"

#include "Tools/Debug/DebugPlot.h"
#include "Tools/Debug/DebugRequest.h"

BEGIN_DECLARE_MODULE(FootStepPlanner2018)
  PROVIDE(DebugPlot)
  PROVIDE(DebugRequest)

  REQUIRE(Walk2018Parameters)
  REQUIRE(FrameInfo)
  REQUIRE(RobotInfo)
  REQUIRE(MotionRequest)
  REQUIRE(CentreOfPressure)

  PROVIDE(MotionStatus) // reason: increasing stepControl.stepRequestID
  PROVIDE(CoMErrors)    // reason: clearing buffers after adaptStepSize or emergency stop
  PROVIDE(StepBuffer)
END_DECLARE_MODULE(FootStepPlanner2018)

class FootStepPlanner2018 : private FootStepPlanner2018Base
{
public:
  FootStepPlanner2018();

  virtual void execute();

  void init(size_t initial_number_of_cycles, InverseKinematic::FeetPose initialFeetPose);

  const FootStepPlanner2018Parameters& parameters;

private:
  void calculateNewStep(const Step& lastStep, Step& newStep, const WalkRequest& walkRequest); // const;

  void adaptStepSize(FootStep& step) const;

  void updateParameters();

  FootStep nextStep(const FootStep& lastStep, const WalkRequest& req) const;

  FootStep controlStep(const FootStep& lastStep, const WalkRequest& req) const;

  FootStep finalStep(const FootStep& lastStep, const WalkRequest& req) const;

  FootStep zeroStep(const FootStep& lastStep) const;

  FootStep calculateNextWalkStep(const InverseKinematic::FeetPose& pose, const Pose2D& offset, const Pose2D& lastStepRequest, FootStep::Foot movingFoot, const WalkRequest& req, bool stepControl = false) const;

  FootStep firstStep(const InverseKinematic::FeetPose& pose, const Pose2D& offset, const Pose2D& lastStepRequest, const WalkRequest& req) const;

  void restrictStepSize(Pose2D& step, double character, bool stepControl) const;

  void restrictStepChange(Pose2D& step, const Pose2D& lastStep, bool stepControl) const;

public:

  double theFootOffsetY;

  unsigned int emergencyCounter;
};

#endif // _FOOT_STEP_PLANNER_2018_H
