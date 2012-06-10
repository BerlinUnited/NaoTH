/**
 * @file Motion.h
 *
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 *
 */

#ifndef _MOTION_H
#define _MOTION_H

#include <PlatformInterface/Callable.h>
#include <PlatformInterface/PlatformInterface.h>

#include "MotionBlackBoard.h"
#include "AbstractMotion.h"
#include "Engine/HeadMotion/HeadMotionEngine.h"
#include "Engine/MotionFactory.h"

#include "MorphologyProcessor/SupportPolygonGenerator.h"
#include "MorphologyProcessor/OdometryCalculator.h"
#include "MorphologyProcessor/FootTouchCalibrator.h"
#include "SensorFilter/InertiaSensorCalibrator.h"

#include "Tools/Debug/Logger.h"

#ifdef NAO_OLD
#include <Representations/Infrastructure/DebugMessage.h>
#include <Cognition/Modules/Infrastructure/Debug/StopwatchSender.h>
#endif

class Motion : public naoth::Callable
{
public:
  Motion();
  virtual ~Motion();

  virtual void call();

  void init(naoth::PlatformInterfaceBase& platformInterface);
  
  bool exit();
  
protected:
  
  void processSensorData();
  
  void postProcess();
  
  void selectMotion();
  
  void changeMotion(AbstractMotion* m);

private:
  MotionBlackBoard& theBlackBoard;

  EmptyMotion theEmptyMotion;
  
  InertiaSensorCalibrator theInertiaSensorCalibrator;
  SupportPolygonGenerator theSupportPolygonGenerator;
  OdometryCalculator theOdometryCalculator;
  FootTouchCalibrator theFootTouchCalibrator;
  
  HeadMotionEngine theHeadMotionEngine;
  std::list<MotionFactory*> theMotionFactories;
  
  unsigned int frameNumSinceLastMotionRequest;
  unsigned int lastCognitionFrameNumber;

  enum State
  {
    initial,
    running,
    exiting
  } state;

  Logger motionLogger;
#ifdef NAO_OLD
  naoth::DebugMessageIn theDebugMessageIn;
  naoth::DebugMessageOut theDebugMessageOut;
  StopwatchSender theStopwatchSender;
#endif

  unsigned int oldMotionRequestTime;
};

#endif  /* MOTION_H */

