/**
 * @file Motion.h
 *
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 *
 */

#ifndef _Motion_h_
#define _Motion_h_

#include <PlatformInterface/Callable.h>
#include <PlatformInterface/PlatformInterface.h>

#include <ModuleFramework/Module.h>
#include <Tools/Debug/ModuleManagerWithDebug.h>

// motion modules
#include "MorphologyProcessor/OdometryCalculator.h"
//#include "MorphologyProcessor/FootTouchCalibrator.h"
#include "MorphologyProcessor/FootGroundContactDetector.h"
#include "MorphologyProcessor/KinematicChainProviderMotion.h"
#include "SensorFilter/InertiaSensorCalibrator.h"
#include "SensorFilter/InertiaSensorFilter.h"
#include "SensorFilter/IMUModel.h"
#include "SensorFilter/ArmCollisionDetector.h"
#include "SensorFilter/ArmCollisionDetector2018.h"
#include "SensorFilter/BodyCollisionDetector.h"
#include "SensorFilter/BumperCollisionDetector.h"
#include "SensorFilter/CoPProvider.h"
#include "SensorFilter/SensorLogger.h"

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
#include "Representations/Motion/Walk2018/Walk2018Parameters.h"

#include "Representations/Perception/CameraMatrix.h"
#include <Representations/Modeling/CameraMatrixOffset.h>
#include <Representations/Modeling/BodyStatus.h>
#include <Representations/Modeling/BodyState.h>

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

BEGIN_DECLARE_MODULE(Motion)
  REQUIRE(GroundContactModel)

  PROVIDE(StopwatchManager)
  PROVIDE(DebugDrawings)
  PROVIDE(DebugImageDrawings)
  PROVIDE(DebugImageDrawingsTop)
  PROVIDE(DebugPlot)
  PROVIDE(DebugDrawings3D)
  PROVIDE(DebugParameterList)
  PROVIDE(DebugModify)

  REQUIRE(MotionStatus)
  PROVIDE(OdometryData) // hack
  PROVIDE(InertialModel) // need to overwrite the old filter value by IMUModel
  REQUIRE(CalibrationData)
  REQUIRE(IMUData)
  REQUIRE(CentreOfPressure) // logging

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

  PROVIDE(Walk2018Parameters) // provide parameters for walk2018 modules, allows for modifying parameters without walking
END_DECLARE_MODULE(Motion)


class Motion : public naoth::Callable, private MotionBase, public ModuleManagerWithDebug
{
public:
  Motion();
  virtual ~Motion();

  virtual void call();

  // TODO: unify with Callable
  void execute() {}

  /**
  *
  */
  void init(naoth::ProcessInterface& platformInterface, const naoth::PlatformBase& platform);

private:
  void processSensorData();

  void postProcess();

  void modifyJointOffsets();

private:

  class Parameter : public ParameterList
  {
  public:
    Parameter() : ParameterList("Motion")
    {
      PARAMETER_REGISTER(letIMUModelProvideInertialModel) = true;
      //PARAMETER_REGISTER(useInertiaSensorCalibration) = true;

      PARAMETER_REGISTER(recordSensorData) = false;
      syncWithConfig();
    }

    bool letIMUModelProvideInertialModel;
    //bool useInertiaSensorCalibration;

    bool recordSensorData;
  } parameter;


private:
  void debugPlots();
  void updateCameraMatrix();
  void drawRobot3D(const KinematicChain& kinematicChain);

private: // motion modules
  // HACK: needs a better solution
  AbstractModuleCreator* theLogProvider;

  ModuleCreator<InertiaSensorCalibrator>* theInertiaSensorCalibrator;
  ModuleCreator<InertiaSensorFilter>* theInertiaSensorFilterBH;
  ModuleCreator<FootGroundContactDetector>* theFootGroundContactDetector;
  ModuleCreator<OdometryCalculator>* theOdometryCalculator;
  ModuleCreator<KinematicChainProviderMotion>* theKinematicChainProvider;
  ModuleCreator<IMUModel>* theIMUModel;

  ModuleCreator<ArmCollisionDetector>* theArmCollisionDetector;
  ModuleCreator<ArmCollisionDetector2018>* theArmCollisionDetector2018;
  ModuleCreator<BodyCollisionDetector>* theBodyCollisionDetector;
  ModuleCreator<BumperCollisionDetector>* theBumperCollisionDetector;
   
  ModuleCreator<CoPProvider>* theCoPProvider;
  ModuleCreator<MotionEngine>* theMotionEngine;

  ModuleCreator<SensorLogger>* theSensorLogger;

  naoth::MotorJointData theLastMotorJointData;

  Logger motionLogger;
  Stopwatch cycleStopwatch;

private:
  std::stringstream debug_answer_stream;

private:
  RingBuffer<double,100> currentsRingBuffer[naoth::JointData::numOfJoint];
  RingBuffer<double,4> motorJointDataBuffer[naoth::JointData::numOfJoint];

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


#endif  // _Motion_h_

