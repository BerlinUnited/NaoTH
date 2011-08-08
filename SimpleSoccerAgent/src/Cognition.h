/**
 * @file Cognition.h
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu Yuan</a>
 *
 */

#ifndef COGNITION_H
#define	COGNITION_H

#include <iostream>

//
#include <PlatformInterface/Callable.h>
#include <PlatformInterface/PlatformInterface.h>

// sensors
#include <Representations/Infrastructure/JointData.h>
#include <Representations/Infrastructure/InertialSensorData.h>
#include <Representations/Infrastructure/FSRData.h>
#include <Representations/Infrastructure/AccelerometerData.h>
#include <Representations/Infrastructure/GyrometerData.h>

// vision
#include <Representations/Infrastructure/VirtualVision.h>
#include <Representations/Infrastructure/Image.h>

// infrastructure
#include <Representations/Infrastructure/FrameInfo.h>
#include <Representations/Infrastructure/GameData.h>

//
#include "BlackBoard.h"


class Cognition : public naoth::Callable
{
public:
  Cognition();
  virtual ~Cognition();


  /** register the data at the platform for the input */
  void init(naoth::PlatformDataInterface& platformInterface)
  {
    platformInterface.registerCognitionInput(theSensorJointData);
    platformInterface.registerCognitionInput(theInertialSensorData);
    platformInterface.registerCognitionInput(theFSRData);
    platformInterface.registerCognitionInput(theAccelerometerData);
    platformInterface.registerCognitionInput(theGyrometerData);

    // virtual vision for the simulation 3D
    platformInterface.registerCognitionInput(theVirtualVision);
    // image for the SPL
    platformInterface.registerCognitionInput(theImage);

    platformInterface.registerCognitionInput(theFrameInfo);
    platformInterface.registerCognitionInput(theGameInfo);
    std::cout << "Cognition register end" << std::endl;
  }//end init


  virtual void call();

private:
  void perception();
  void detect_ball_in_image();
  void detect_ball_in_virtual_vision();

  void decide();

private:
  // representations
  naoth::SensorJointData theSensorJointData;
  naoth::InertialSensorData theInertialSensorData;
  naoth::FSRData theFSRData;
  naoth::AccelerometerData theAccelerometerData;
  naoth::GyrometerData theGyrometerData;
  
  naoth::VirtualVision theVirtualVision;
  naoth::Image theImage;

  naoth::FrameInfo theFrameInfo;
  naoth::GameData theGameInfo;


  bool robotIsUpright;
};

#endif	/* COGNITION_H */

