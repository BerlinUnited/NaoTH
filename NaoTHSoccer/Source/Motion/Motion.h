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

#include <ModuleFramework/ModuleManager.h>
#include <ModuleFramework/Module.h>
#include <Tools/Debug/ModuleManagerWithDebug.h>

//#include "MorphologyProcessor/SupportPolygonGenerator.h"
//#include "MorphologyProcessor/OdometryCalculator.h"
//#include "MorphologyProcessor/FootTouchCalibrator.h"
//#include "MorphologyProcessor/FootGroundContactDetector.h"
//#include "MorphologyProcessor/KinematicChainProviderMotion.h"
//#include "SensorFilter/InertiaSensorCalibrator.h"
//#include "SensorFilter/InertiaSensorFilter.h"

//#include <Representations/Modeling/CameraMatrixOffset.h>

#include "Tools/Debug/Logger.h"
//#include "Engine/MotionEngine.h"


// representations
#include <Representations/Infrastructure/JointData.h>
#include <Representations/Infrastructure/FrameInfo.h>
#include <Representations/Infrastructure/InertialSensorData.h>
#include <Representations/Infrastructure/FSRData.h>
#include <Representations/Infrastructure/AccelerometerData.h>
#include <Representations/Infrastructure/GyrometerData.h>

// debug
#include <Representations/Debug/Stopwatch.h>
#include <Representations/Infrastructure/FrameInfo.h>
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugDrawings.h"
#include "Tools/Debug/DebugImageDrawings.h"
#include "Tools/Debug/DebugPlot.h"
#include "Tools/Debug/DebugDrawings3D.h"

BEGIN_DECLARE_MODULE(Motion)
  PROVIDE(StopwatchManager)
  PROVIDE(DebugDrawings)
  PROVIDE(DebugImageDrawings)
  PROVIDE(DebugImageDrawingsTop)
  PROVIDE(DebugPlot)
  PROVIDE(DebugDrawings3D)

//  REQUIRE(MotionStatus)
//  REQUIRE(OdometryData)
//  REQUIRE(InertialModel)
//  REQUIRE(CalibrationData)

//  PROVIDE(CameraMatrix)// TODO:strange...
//  PROVIDE(CameraMatrixTop)// TODO:strange...

  // PROVIDE is needed to update the speed and acceleration
  PROVIDE(MotorJointData) // TODO: check
  
//  PROVIDE(RobotInfo)
//  PROVIDE(KinematicChainSensor)
//  PROVIDE(KinematicChainMotor)

  // platform input
  REQUIRE(SensorJointData)
  PROVIDE(FrameInfo)
  PROVIDE(InertialSensorData)
  PROVIDE(FSRData)
  PROVIDE(AccelerometerData)
  PROVIDE(GyrometerData)
  
//  PROVIDE(CameraMatrixOffset)

  // from cognition
//  PROVIDE(CameraInfo)
//  PROVIDE(CameraInfoTop)
//  PROVIDE(HeadMotionRequest)
//  PROVIDE(MotionRequest)
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
  

private:
  void debugPlots();
//  void updateCameraMatrix();

private:
//  ModuleCreator<InertiaSensorCalibrator>* theInertiaSensorCalibrator;
//  ModuleCreator<InertiaSensorFilter>* theInertiaSensorFilterBH;
//  ModuleCreator<FootGroundContactDetector>* theFootGroundContactDetector;
//  ModuleCreator<SupportPolygonGenerator>* theSupportPolygonGenerator;
//  ModuleCreator<OdometryCalculator>* theOdometryCalculator;
//  ModuleCreator<KinematicChainProviderMotion>* theKinematicChainProvider;

//  ModuleCreator<MotionEngine>* theMotionEngine;

//  naoth::MotorJointData theLastMotorJointData;

  Logger motionLogger;
};


#endif  // _Motion_h_ 

