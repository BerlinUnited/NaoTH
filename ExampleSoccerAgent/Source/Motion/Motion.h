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
#include <PlatformInterface/Callable.h>
#include <PlatformInterface/PlatformInterface.h>
#include <ModuleFramework/ModuleManager.h>
#include <ModuleFramework/Module.h>


// representations
#include <Representations/Infrastructure/FrameInfo.h>
#include <Representations/Infrastructure/FSRData.h>
#include <Representations/Infrastructure/JointData.h>
#include <Representations/Infrastructure/AccelerometerData.h>
#include <Representations/Infrastructure/GyrometerData.h>
#include <Representations/Infrastructure/InertialSensorData.h>
#include <Representations/Infrastructure/LEDData.h>
#include <Representations/Infrastructure/RobotInfo.h>

#include "Representations/Motion/MotionRequest.h"
#include "Representations/Motion/MotionStatus.h"
#include "Representations/Modeling/OdometryData.h"

BEGIN_DECLARE_MODULE(Motion)
  PROVIDE(MotionRequest)
  REQUIRE(LEDData)
  
  PROVIDE(RobotInfo)
  PROVIDE(MotionStatus)
  PROVIDE(MotorJointData)

  PROVIDE(SensorJointData)
  PROVIDE(FrameInfo)
  PROVIDE(InertialSensorData)
  PROVIDE(FSRData)
  PROVIDE(AccelerometerData)
  PROVIDE(GyrometerData)
END_DECLARE_MODULE(Motion)

class Motion : public naoth::Callable, public MotionBase, public ModuleManager
{
public:

  Motion();
  virtual ~Motion();

  /** */
  void init(naoth::ProcessInterface& platformInterface, const naoth::PlatformBase& platform);

  virtual void call();
  void execute(){}

  class KeyFrame
  {
  public:
    KeyFrame();
    
    double position[naoth::JointData::numOfJoint];
    double time;
  };

  std::list<KeyFrame> loadKeyFrames(const std::string& filename);

private:
  int theTimeStep;

  std::list<KeyFrame> theKeyFrame[MotionRequest::numOfMotionID];

  std::list<KeyFrame> activeKeyFrame;
};

std::istream& operator>>(std::istream& in, Motion::KeyFrame& kf);

#endif	/* MOTION_H */

