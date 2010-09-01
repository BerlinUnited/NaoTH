/**
 * @file Cognition.h
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu Yuan</a>
 *
 */

#ifndef COGNITION_H
#define	COGNITION_H

#include <iostream>
#include <Interface/PlatformInterface/Callable.h>

#include <Interface/Representations/Infrastructure/JointData.h>
#include <Interface/Representations/Infrastructure/VirtualVision.h>
#include <Interface/Representations/Infrastructure/FrameInfo.h>
#include <Interface/Representations/Infrastructure/InertialSensorData.h>
#include <SimSpark/SimSparkGameInfo.h>

#include "BallPercept.h"

class Cognition : public naorunner::Callable
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

#undef REGISTER_INPUT
    std::cout << "Cognition register end" << std::endl;
  }


  virtual void call();

private:
  void perception();

  void decide();

private:
  naorunner::SensorJointData theSensorJointData;
  naorunner::VirtualVision theVirtualVision;
  naorunner::FrameInfo theFrameInfo;
  naorunner::InertialSensorData theInertialSensorData;
  SimSparkGameInfo theSimSparkGameInfo;

  BallPercept theBall;
  bool isStandingUp;
};

#endif	/* COGNITION_H */

