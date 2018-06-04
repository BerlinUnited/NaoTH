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

#include "Representations/Infrastructure/CalibrationData.h"
#include <Representations/Infrastructure/GyrometerData.h>
#include "Representations/Modeling/InertialModel.h"
#include <Representations/Infrastructure/InertialSensorData.h>

// debug
#include "Tools/Debug/DebugModify.h"
#include "Tools/Debug/DebugPlot.h"
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugDrawings.h"
#include "Representations/Debug/Stopwatch.h"

// tools
#include <queue>
#include "Walk/FootStep.h"
#include "Walk/FootStepPlanner.h"
#include "IKPose.h"
#include "Tools/DataStructures/RingBufferWithSum.h"

using namespace naoth;

BEGIN_DECLARE_MODULE(Walk)
  PROVIDE(DebugModify)
  PROVIDE(DebugPlot)
  PROVIDE(DebugRequest)
  PROVIDE(DebugDrawings)

  REQUIRE(FrameInfo)
  REQUIRE(RobotInfo)
  REQUIRE(InverseKinematicsMotionEngineService)
  REQUIRE(MotionRequest)

  REQUIRE(InertialSensorData)
  REQUIRE(IMUData)
  REQUIRE(CalibrationData)
  REQUIRE(InertialModel)
  REQUIRE(GyrometerData)
  REQUIRE(KinematicChainSensor)

  PROVIDE(MotionLock)
  PROVIDE(MotionStatus)
  PROVIDE(MotorJointData)
END_DECLARE_MODULE(Walk)

class Walk: private WalkBase, public IKMotion
{
public:
  Walk();
  
  virtual void execute();

  enum StepType {
    STEP_WALK,
    STEP_CONTROL
  };

  Stopwatch stopwatch;

  Stopwatch stopwatch0;
  Stopwatch stopwatch1;
  Stopwatch stopwatch2;
  Stopwatch stopwatch3;

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
      executingCycle(0),

      type(STEP_WALK)
    {}

    FootStep footStep;

    // step duration
    int numberOfCycles;

    // running parameters indicating how far the step is processed
    int planningCycle;
    int executingCycle;

    StepType type;
    // store the request which has been used to create this step
    WalkRequest walkRequest;

    unsigned int id() const { return _id; }
    bool isPlanned() const { return planningCycle >= numberOfCycles; }
    bool isExecuted() const { return executingCycle >= numberOfCycles; }
  };

  class StepBuffer
  {
  private:
    unsigned int id;
    std::list<Step> steps;
  
  public:
    StepBuffer() : id(0) {}

    inline Step& add() {
      steps.push_back(Step(id++));
      return steps.back();
    }

    // deligated accessors
    inline const Step& first() const { return steps.front(); }
    inline Step& first() { return steps.front(); }
    inline const Step& last() const { return steps.back(); }
    inline Step& last() { return steps.back(); }
    inline bool empty() const { return steps.empty(); }
    inline void pop() { return steps.pop_front(); }
    inline unsigned int stepId() const { return id; }

    void draw(DrawingCanvas2D& canvas) const
    {
      for(std::list<Step>::const_iterator i = steps.begin(); i != steps.end(); ++i) {
        i->footStep.draw(canvas);
      }
    }
  };

private:
  StepBuffer stepBuffer;
  FootStepPlanner theFootStepPlanner;
  InverseKinematic::CoMFeetPose theCoMFeetPose;

  const IKParameters::Walk& parameters() const {
    return getEngine().getParameters().walk;
  }

private:

  void manageSteps(const MotionRequest& motionRequest);
  // step creators
  void calculateNewStep(const Step& lastStep, Step& step, const WalkRequest& motionRequest);// const;


  void planZMP();
  void executeStep();

  Pose3D calculateLiftingFootPos(const Step& step) const;
  RotationMatrix calculateBodyRotation(const InverseKinematic::FeetPose& feet, double pitch) const;
  Pose3D calculateStableCoMByFeet(InverseKinematic::FeetPose feet, double pitch) const;

  void updateMotionStatus(MotionStatus& motionStatus) const;


private: // stabilization

  // this is updated by feetStabilize()
  mutable Vector3d filteredGyro;

  // observe the com error
  RingBufferWithSum<double, 100> com_errors;
  Vector3d currentComError;
  RingBufferWithSum<Vector3d, 100> currentComErrorBuffer;

  // a buffer of CoMFeetPoses requested in the past
  // needed by stabilization
  RingBuffer<InverseKinematic::CoMFeetPose, 10> commandPoseBuffer;
  RingBuffer<FootStep::Foot, 10> commandFootIdBuffer;

  unsigned int emergencyCounter;

  void adaptStepSize(FootStep& step) const;
  void calculateError();
  void feetStabilize(const Step& executingStep, double (&position)[naoth::JointData::numOfJoint]) const;
};

#endif // _Walk_H_
