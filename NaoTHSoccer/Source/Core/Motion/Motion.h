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
#include "Engine/MotionEngine.h"

class Motion : public naoth::Callable
{
public:
  Motion();
  virtual ~Motion();

  virtual void call();

  void init(naoth::ProcessInterface& platformInterface, const naoth::PlatformBase& platform);
  
  //bool exit();
  
protected:
  
  void processSensorData();
  
  void postProcess();
  
private:

  void updateCameraMatrix();

private:
  MotionBlackBoard& theBlackBoard;

  InertiaSensorCalibrator theInertiaSensorCalibrator;
  SupportPolygonGenerator theSupportPolygonGenerator;
  OdometryCalculator theOdometryCalculator;
  FootTouchCalibrator theFootTouchCalibrator;
  
  MotionEngine theMotionEngine;
  
  unsigned int frameNumSinceLastMotionRequest;
  unsigned int lastCognitionFrameNumber;

  Logger motionLogger;
};

#endif  /* MOTION_H */

