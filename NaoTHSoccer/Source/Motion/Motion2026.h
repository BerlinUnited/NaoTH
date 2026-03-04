/**
 * @file Motion.h
 *
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
 *
 */

#ifndef MOTION_2026_H
#define MOTION_2026_H

#include <PlatformInterface/Callable.h>
#include <PlatformInterface/PlatformInterface.h>

#include <ModuleFramework/Module.h>
#include <Tools/Debug/ModuleManagerWithDebug.h>


// modules
#include "Engine/HeadMotion/HeadMotionEngine.h"


// representations
#include <Representations/Infrastructure/FrameInfo.h>
#include <Representations/Infrastructure/JointData.h>
#include <Representations/Infrastructure/InertialSensorData.h>
#include <Representations/Infrastructure/FSRData.h>
#include <Representations/Infrastructure/AccelerometerData.h>
#include <Representations/Infrastructure/GyrometerData.h>
#include <Representations/Infrastructure/ButtonData.h>
#include <Representations/Infrastructure/DebugMessage.h>
#include <Representations/Modeling/IMUData.h>
#include "Representations/Modeling/GroundContactModel.h"
#include "Representations/Motion/CollisionPercept.h"

#include "Representations/Perception/CameraMatrix.h"
#include <Representations/Modeling/CameraMatrixOffset.h>
#include <Representations/Modeling/BodyStatus.h>
#include <Representations/Modeling/BodyState.h>
#include <Representations/Modeling/OdometryData.h>

// debug
#include <Representations/Debug/Stopwatch.h>
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugDrawings.h"
#include "Tools/Debug/DebugImageDrawings.h"
#include "Tools/Debug/DebugPlot.h"
#include "Tools/Debug/DebugDrawings3D.h"
#include "Tools/Debug/DebugParameterList.h"
#include "Tools/Debug/DebugModify.h"

// tools
#include <Tools/DataStructures/ParameterList.h>
#include <Tools/DataStructures/RingBufferWithSum.h>

#include "Tools/Debug/Logger.h"
#include "Engine/MotionEngine.h"

BEGIN_DECLARE_MODULE(Motion2026)
  REQUIRE(GroundContactModel)

  PROVIDE(StopwatchManager)
  PROVIDE(DebugDrawings)
  PROVIDE(DebugImageDrawings)
  PROVIDE(DebugImageDrawingsTop)
  PROVIDE(DebugPlot)
  PROVIDE(DebugDrawings3D)
  PROVIDE(DebugParameterList)
  PROVIDE(DebugModify)

  //REQUIRE(StepBuffer) // hack
  REQUIRE(MotionStatus)
  PROVIDE(OdometryData) // hack
  PROVIDE(InertialModel) // need to overwrite the old filter value by IMUModel
  //REQUIRE(CalibrationData)
  REQUIRE(IMUData)

  PROVIDE(CameraMatrix)// TODO:strange...
  PROVIDE(CameraMatrixTop)// TODO:strange...

  // PROVIDE is needed to update the speed and acceleration
  PROVIDE(MotorJointData) // TODO: check

  PROVIDE(OffsetJointData)

  PROVIDE(RobotInfo)
  PROVIDE(KinematicChainSensor)
  PROVIDE(KinematicChainMotor)

  // platform input
  PROVIDE(SensorJointData) //REQUIRE(SensorJointData)
  PROVIDE(FrameInfo)
  PROVIDE(InertialSensorData)
  PROVIDE(FSRData)
  PROVIDE(AccelerometerData)
  PROVIDE(GyrometerData)
  PROVIDE(ButtonData)

  PROVIDE(DebugMessageInMotion)
  PROVIDE(DebugMessageOut)

  PROVIDE(CameraMatrixOffset)
  REQUIRE(CollisionPercept)

  // from cognition
  PROVIDE(CameraInfo)
  PROVIDE(CameraInfoTop)
  PROVIDE(HeadMotionRequest)
  PROVIDE(MotionRequest)
  PROVIDE(BodyStatus)
  PROVIDE(BodyState)

END_DECLARE_MODULE(Motion2026)


class Motion2026 : public naoth::Callable, private Motion2026Base, public ModuleManagerWithDebug
{
public:
  Motion2026();
  virtual ~Motion2026();

  virtual void call();

  // TODO: unify with Callable
  void execute() {}

  /**
  *
  */
  void init(naoth::ProcessInterface& platformInterface, const naoth::PlatformBase& platform);

private:

  class Parameter : public ParameterList
  {
  public:
    Parameter() : ParameterList("Motion")
    {
      PARAMETER_REGISTER(test) = true;
      syncWithConfig();
    }

    bool test;
  } parameter;



private: // motion modules
  // HACK: needs a better solution
  AbstractModuleCreator* theLogProvider;

  ModuleCreator<HeadMotionEngine>* theHeadMotionEngine;

  Logger motionLogger;
  Stopwatch cycleStopwatch;

private:
  std::stringstream debug_answer_stream;

private:
  // NOTE: copy from Debug.h
  // TODO: generalize attaching the logger
  void registerLogableRepresentationList()
  {
    const BlackBoard& blackBoard = BlackBoardInterface::getBlackBoard();
    BlackBoard::Registry::const_iterator iter;

    for(iter = blackBoard.getRegistry().begin(); iter != blackBoard.getRegistry().end(); ++iter)
    {
      const Representation& theRepresentation = iter->second->getRepresentation();
      if(theRepresentation.serializable()) {
        motionLogger.addRepresentation(&theRepresentation, iter->first);
      }
    }
  }
};


#endif  // MOTION_2026_H

