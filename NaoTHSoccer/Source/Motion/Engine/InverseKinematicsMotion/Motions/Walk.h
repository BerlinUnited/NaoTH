/**
* @file Walk.h
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich, Mellmann</a>
* @author <a href="mailto:kaden@informatik.hu-berlin.de">Steffen, Kaden</a>
*
*/

#ifndef _Walk_H_
#define _Walk_H_

#include "IKMotion.h"
#include <ModuleFramework/Module.h>

// representations
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Motion/Request/MotionRequest.h"
#include "Representations/Motion/MotionStatus.h"
#include <Representations/Infrastructure/JointData.h>

// debug
#include "Tools/Debug/DebugModify.h"
#include "Tools/Debug/DebugPlot.h"
#include "Tools/Debug/DebugRequest.h"

// tools
#include <queue>
#include "Walk/FootStep.h"
#include "Walk/FootStepPlanner.h"
#include "IKPose.h"

BEGIN_DECLARE_MODULE(Walk)
  PROVIDE(DebugModify)
  PROVIDE(DebugPlot)
  PROVIDE(DebugRequest)

  REQUIRE(FrameInfo)
  REQUIRE(InverseKinematicsMotionEngineService)
  REQUIRE(MotionRequest)

  PROVIDE(MotionLock)
  PROVIDE(MotionStatus)
  PROVIDE(MotorJointData)
END_DECLARE_MODULE(Walk)

class Walk: private WalkBase, public IKMotion
{
public:
  Walk();
  
  virtual void execute();

private:
  /** class describing a single step */
  class Step 
  {
  private:
    unsigned int _id;

  public:
    Step(unsigned int _id)
      :
      _id(_id),

      planningCycle(0),
      executingCycle(0)
    {}

    FootStep footStep;

    // step duration
    int numberOfCycles;

    // running parameters indicating how far the step is processed
    int planningCycle;
    int executingCycle;

    unsigned int id() const { return _id; }
    bool isPlanned() const { return planningCycle >= numberOfCycles; }
    bool isExecuted() const { return executingCycle >= numberOfCycles; }
  };

  class StepBuffer
  {
  private:
    unsigned int id;
    std::queue<Step> steps;
  
  public:
    StepBuffer() : id(0) {}

    inline Step& add() {
      steps.push(Step(id++));
      return steps.back();
    }

    // deligated accessors
    inline const Step& first() const { return steps.front(); }
    inline Step& first() { return steps.front(); }
    inline const Step& last() const { return steps.back(); }
    inline Step& last() { return steps.back(); }
    inline bool empty() const { return steps.empty(); }
    inline void pop() { return steps.pop(); }
    inline unsigned int stepId() { return id; }
  };

private:
  StepBuffer stepBuffer;
  FootStepPlanner theFootStepPlanner;

private:

  void manageSteps(const MotionRequest& motionRequest);
  // step creators
  void newLastStep();
  void newZeroStep();
  void newStep(const WalkRequest& motionRequest);

};

#endif // _Walk_H_
