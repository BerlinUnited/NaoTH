/**
 * @file Cognition.h
 *
  * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
 *
 */

#ifndef COGNITION_H
#define	COGNITION_H

#include <iostream>

#include <PlatformInterface/Callable.h>
#include <PlatformInterface/PlatformInterface.h>


#include <ModuleFramework/ModuleManager.h>

// representations
#include <Representations/Infrastructure/Image.h>
#include <Representations/Infrastructure/JointData.h>
#include <Representations/Infrastructure/AccelerometerData.h>
#include <Representations/Infrastructure/FrameInfo.h>
#include <Representations/Infrastructure/GyrometerData.h>
#include <Representations/Infrastructure/FSRData.h>
#include <Representations/Infrastructure/InertialSensorData.h>
#include <Representations/Infrastructure/BumperData.h>
#include <Representations/Infrastructure/IRData.h>
#include <Representations/Infrastructure/CameraSettings.h>
#include <Representations/Infrastructure/LEDData.h>
#include <Representations/Infrastructure/UltraSoundData.h>
#include <Representations/Infrastructure/SoundData.h>
#include <Representations/Infrastructure/ButtonData.h>
#include <Representations/Infrastructure/BatteryData.h>



class Cognition : public naoth::Callable, public ModuleManager
{
public:
  Cognition();
  virtual ~Cognition();


  virtual void call();




  #define REG_INPUT(name) \
  platformInterface.registerCognitionInput(getBlackBoard().getRepresentation<naoth::name>(#name))
  #define REG_OUTPUT(name) \
  platformInterface.registerCognitionOutput(getBlackBoard().getConstRepresentation<naoth::name>(#name))

  template<class PlatformType>
  void init(naoth::PlatformInterface<PlatformType>& platformInterface)
  {
    std::cout << "Cognition register start" << std::endl;
    
    // input
    REG_INPUT(SensorJointData);
    REG_INPUT(FrameInfo);
    REG_INPUT(AccelerometerData);
    REG_INPUT(Image);
    REG_INPUT(FSRData);
    REG_INPUT(GyrometerData);
    REG_INPUT(InertialSensorData);
    REG_INPUT(BumperData);
    REG_INPUT(IRReceiveData);
    REG_INPUT(CurrentCameraSettings);
    REG_INPUT(ButtonData);
    REG_INPUT(BatteryData);
    REG_INPUT(UltraSoundReceiveData);
    

    // output
    REG_OUTPUT(CameraSettingsRequest);
    REG_OUTPUT(LEDData);
    REG_OUTPUT(IRSendData);
    REG_OUTPUT(UltraSoundSendData);
    REG_OUTPUT(SoundData);

    std::cout << "Cognition register end" << std::endl;
  }//end init

};

#endif	/* COGNITION_H */

