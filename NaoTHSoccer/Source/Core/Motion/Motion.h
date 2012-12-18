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

#include "MotionBlackBoard.h"
//#include "AbstractMotion.h"
//#include "Engine/HeadMotion/HeadMotionEngine.h"
//#include "Engine/MotionFactory.h"

#include "MorphologyProcessor/SupportPolygonGenerator.h"
#include "MorphologyProcessor/OdometryCalculator.h"
#include "MorphologyProcessor/FootTouchCalibrator.h"
#include "SensorFilter/InertiaSensorCalibrator.h"
#include "SensorFilter/InertiaSensorFilter.h"
#include "MorphologyProcessor/FootGroundContactDetector.h"

#include "Tools/Debug/Logger.h"
#include "Engine/MotionEngine.h"


BEGIN_DECLARE_MODULE(Motion)
  REQUIRE(MotionStatus)
  REQUIRE(OdometryData)
  REQUIRE(InertialModel)
  REQUIRE(CalibrationData)

  PROVIDE(FSRPositions)// TODO:strange...
  PROVIDE(CameraMatrix)// TODO:strange...

  PROVIDE(MotorJointData) // TODO: check
  
  PROVIDE(RobotInfo)
  PROVIDE(KinematicChainSensor)
  PROVIDE(KinematicChainMotor)

  // platform input
  PROVIDE(SensorJointData)
  PROVIDE(FrameInfo)
  PROVIDE(InertialSensorData)
  PROVIDE(FSRData)
  PROVIDE(AccelerometerData)
  PROVIDE(GyrometerData)
  

  // from cognition
  PROVIDE(CameraInfo)
  PROVIDE(HeadMotionRequest)
  PROVIDE(MotionRequest)
END_DECLARE_MODULE(Motion)


class Motion : public naoth::Callable, private MotionBase, public ModuleManager//WithDebug
{
public:
  Motion();
  virtual ~Motion();


  /**
  *
  */
  virtual void call();

  // TODO: unify with Callable
  void execute() {}

  /**
  *
  */
  void init(naoth::ProcessInterface& platformInterface, const naoth::PlatformBase& platform);
  
  //bool exit();
  
protected:
  
  void processSensorData();
  
  void postProcess();
  
private:

  void updateCameraMatrix();
  void guard_cognition();

private:

  //MotionBlackBoard& theBlackBoard;


  ModuleCreator<InertiaSensorCalibrator>* theInertiaSensorCalibrator;
  ModuleCreator<InertiaSensorFilter>* theInertiaSensorFilterBH;
  ModuleCreator<FootGroundContactDetector>* theFootGroundContactDetector;
  ModuleCreator<SupportPolygonGenerator>* theSupportPolygonGenerator;
  ModuleCreator<OdometryCalculator>* theOdometryCalculator;

  ModuleCreator<MotionEngine>* theMotionEngine;

  //FootTouchCalibrator theFootTouchCalibrator;


  naoth::MotorJointData theLastMotorJointData;

  Logger motionLogger;
};


#endif  // _Motion_h_ 

