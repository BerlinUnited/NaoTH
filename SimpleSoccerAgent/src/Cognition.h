/**
 * @file Cognition.h
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu Yuan</a>
 *
 */

#ifndef COGNITION_H
#define	COGNITION_H

#include <iostream>
#include <PlatformInterface/Callable.h>

#include <Representations/Infrastructure/JointData.h>
#include <Representations/Infrastructure/VirtualVision.h>
#include <Representations/Infrastructure/FrameInfo.h>
#include <Representations/Infrastructure/InertialSensorData.h>
#include <Representations/Infrastructure/FSRData.h>
#include <Representations/Infrastructure/AccelerometerData.h>
#include <Representations/Infrastructure/GyrometerData.h>
#include <Representations/Infrastructure/SimSparkGameInfo.h>

#include "BallPercept.h"

class Cognition : public naoth::Callable
{
public:
  Cognition();
  virtual ~Cognition();


  template<class PlatformType>
  void init(PlatformType& platformInterface)
  {
    std::cout << "Cognition register start" << std::endl;
#define REGISTER_INPUT(R) platformInterface.registerCognitionInput(the##R, #R)
    REGISTER_INPUT(SensorJointData);
    REGISTER_INPUT(VirtualVision);
    REGISTER_INPUT(SimSparkGameInfo);
    REGISTER_INPUT(FrameInfo);
    REGISTER_INPUT(InertialSensorData);
    REGISTER_INPUT(FSRData);
    REGISTER_INPUT(AccelerometerData);
    REGISTER_INPUT(GyrometerData);

#undef REGISTER_INPUT
    std::cout << "Cognition register end" << std::endl;
  }


  virtual void call();

private:
  void perception();

  void decide();

private:
  naoth::SensorJointData theSensorJointData;
  naoth::VirtualVision theVirtualVision;
  naoth::FrameInfo theFrameInfo;
  naoth::InertialSensorData theInertialSensorData;
  naoth::FSRData theFSRData;
  naoth::AccelerometerData theAccelerometerData;
  naoth::GyrometerData theGyrometerData;
  SimSparkGameInfo theSimSparkGameInfo;

  BallPercept theBall;
  bool isStandingUp;
};

#endif	/* COGNITION_H */

