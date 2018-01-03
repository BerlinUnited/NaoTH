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
#include "../IKParameters.h"
#include "Representations/Motion/Request/MotionRequest.h"
#include "Motion/Engine/InverseKinematicsMotion/InverseKinematicsMotionEngine.h"

#include "Representations/Motion/Walk2018/CoMErrors.h"
#include "Representations/Motion/Walk2018/StepBuffer.h"

#include "Tools/Debug/DebugPlot.h"
#include "Tools/Debug/DebugRequest.h"

BEGIN_DECLARE_MODULE(FootStepPlanner2018)
  PROVIDE(DebugPlot)
  PROVIDE(DebugRequest)

  REQUIRE(FrameInfo)
  REQUIRE(RobotInfo)
  REQUIRE(InverseKinematicsMotionEngineService)
  REQUIRE(MotionRequest)

  PROVIDE(CoMErrors)    // reason: clearing buffers after adaptStepSize or emergency stop
  PROVIDE(MotionStatus) // reason: increasing stepControl.stepRequestID
  PROVIDE(StepBuffer)

END_DECLARE_MODULE(FootStepPlanner2018)

class FootStepPlanner2018 : private FootStepPlanner2018Base
{
public:
  FootStepPlanner2018();

  virtual void execute();

  void calculateNewStep(const Step& lastStep, Step& newStep, const WalkRequest& walkRequest);

  void adaptStepSize(FootStep& step) const;

  void updateParameters(const IKParameters& parameters);

  FootStep nextStep(const FootStep& lastStep, const WalkRequest& req);

  FootStep controlStep(const FootStep& lastStep, const WalkRequest& req);
  
  FootStep finalStep(const FootStep& lastStep, const WalkRequest& req);

  FootStep zeroStep(const FootStep& lastStep) const;

private:
  FootStep calculateNextWalkStep(const InverseKinematic::FeetPose& pose, const Pose2D& offset, const Pose2D& lastStepRequest, FootStep::Foot movingFoot, const WalkRequest& req, bool stepControl = false);
  FootStep firstStep(const InverseKinematic::FeetPose& pose, const Pose2D& offset, const Pose2D& lastStepRequest, const WalkRequest& req);

  void restrictStepSize(Pose2D& step, double character) const;
  void restrictStepSizeControlStep(Pose2D& step, double character) const;
  void restrictStepChange(Pose2D& step, const Pose2D& lastStep) const;

private:

  // parameters
  double theMaxTurnInner;
  double theMaxStepTurn;
  double theMaxStepLength;
  double theMaxStepLengthBack;
  double theMaxStepWidth;
  double theFootOffsetY;
  double theMaxChangeTurn;
  double theMaxChangeX;
  double theMaxChangeY;

  double theMaxCtrlTurn;
  double theMaxCtrlLength;
  double theMaxCtrlWidth;

  unsigned int emergencyCounter;
};

#endif // _FOOT_STEP_PLANNER_2018_H
