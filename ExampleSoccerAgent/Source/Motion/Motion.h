/**
 * @file Motion.h
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu Yuan</a>
 *
 */

#ifndef MOTION_H
#define	MOTION_H

#include <iostream>
#include <list>

//
#include <PlatformInterface/PlatformInterface.h>
#include <PlatformInterface/Callable.h>

//
#include <Representations/Infrastructure/JointData.h>
#include <Representations/Infrastructure/FrameInfo.h>
#include <Representations/Infrastructure/InertialSensorData.h>
#include <Representations/Infrastructure/FSRData.h>
#include <Representations/Infrastructure/AccelerometerData.h>
#include <Representations/Infrastructure/GyrometerData.h>
#include "Representations/Motion/MotionRequest.h"

#include "MotionBlackBoard.h"

class Motion : public naoth::Callable
{
public:

  Motion();
  virtual ~Motion();

  /** */
  void init(naoth::PlatformInterfaceBase& platformInterface);

  virtual void call();

  class KeyFrame
  {
  public:
    KeyFrame();
    
    double position[naoth::JointData::numOfJoint];
    double time;
  };

  std::list<KeyFrame> loadKeyFrames(const std::string& filename);

private:
  MotionBlackBoard& theBlackBoard;

  int theTimeStep;

  std::list<KeyFrame> theKeyFrame[MotionRequest::numOfMotionID];

  std::list<KeyFrame> activeKeyFrame;

  MessageReader* theMotionRequestReader;
};

std::istream& operator>>(std::istream& in, Motion::KeyFrame& kf);

#endif	/* MOTION_H */

