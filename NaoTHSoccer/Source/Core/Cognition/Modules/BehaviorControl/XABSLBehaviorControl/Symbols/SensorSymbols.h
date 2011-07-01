/* 
 * File:   SensorSymbols.h
 * Author: thomas
 *
 * Created on 9. April 2009, 18:10
 */

#ifndef _SENSORSYMBOLS_H
#define	_SENSORSYMBOLS_H

#include <ModuleFramework/Module.h>
#include <XabslEngine/XabslEngine.h>

#include "Representations/Infrastructure/CameraSettings.h"
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Infrastructure/UltraSoundData.h"
#include "Representations/Infrastructure/GyrometerData.h"
#include "Representations/Infrastructure/InertialSensorData.h"
#include "Representations/Infrastructure/IRData.h"
#include "Representations/Infrastructure/BatteryData.h"
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Modeling/BodyState.h"
#include "Representations/Modeling/ObstacleModel.h"
#include "Representations/Infrastructure/Image.h"

// tools
#include "Tools/Math/Common.h"

BEGIN_DECLARE_MODULE(SensorSymbols)
  REQUIRE(UltraSoundReceiveData)
  REQUIRE(GyrometerData)
  REQUIRE(InertialSensorData)
  REQUIRE(IRReceiveData)
  REQUIRE(BatteryData)
  REQUIRE(FrameInfo)
  REQUIRE(BodyState)
  REQUIRE(LocalObstacleModel)
  REQUIRE(Image)
END_DECLARE_MODULE(SensorSymbols)

class SensorSymbols: public SensorSymbolsBase
{
public:

  SensorSymbols()
  :
    ultraSoundReceiveData(getUltraSoundReceiveData()),
    gyrometerData(getGyrometerData()),
    inertialSensorData(getInertialSensorData()),
    irreceiveData(getIRReceiveData()),
    batteryData(getBatteryData()),
    frameInfo(getFrameInfo()),
    bodyState(getBodyState()),
    obstacleModel(getLocalObstacleModel()),
    image(getImage())
  {
    theInstance = this;
    forceGetCameraSettings = false;
    forceGetCameraSettingsOldValue = false;
  }

  /** registers the symbols at an engine */
  void registerSymbols(xabsl::Engine& engine);

  /** update internal symbol state */
  virtual void execute();

  virtual ~SensorSymbols();

private:
  UltraSoundReceiveData const& ultraSoundReceiveData;
  GyrometerData const& gyrometerData;
  InertialSensorData const& inertialSensorData;
  IRReceiveData const& irreceiveData;
  BatteryData const& batteryData;
  FrameInfo const& frameInfo;
  BodyState const& bodyState;
  LocalObstacleModel const& obstacleModel;
  Image const& image;


  bool forceGetCameraSettings;
  bool forceGetCameraSettingsOldValue;

  static SensorSymbols* theInstance;

  /*double directionParameter;
  double passagewayParameter1;
  double passagewayParameter2;
  static double getObstacleRadialDirection_free();
  static double getObstacleRadialPassageway_free();
  static double getObstacleRadialObstacle_left();
  static double getObstacleRadialObstacle_right();
  static double getObstacleRadialObstacle_front();*/

  //get-method
  static double getIRButtonNumber();
  static double getFrameNumber();
  static double simplePassLeftSensor();
  static double simplePassRightSensor();

  static int getFallDownState();

  static double getTimeSinceObstacleWasSeen();
  static double getObstacleAngle();
  static double getObstacleDistance();

  static double getCameraBufferFailedCount();
  bool resetingCamera;
  bool isCameraReseting;
  void resetCamera(bool b);

  static double getInertialSensorX();
  static double getInertialSensorY();
};

#endif	/* _SENSORSYMBOLS_H */

