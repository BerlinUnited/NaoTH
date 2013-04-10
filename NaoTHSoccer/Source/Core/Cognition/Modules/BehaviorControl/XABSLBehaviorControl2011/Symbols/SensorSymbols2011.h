/* 
 * File:   SensorSymbols.h
 * Author: thomas
 *
 * Created on 9. April 2009, 18:10
 */

#ifndef _SENSORSYMBOLS2011_H
#define  _SENSORSYMBOLS2011_H

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
#include "Representations/Modeling/RadarGrid.h"
#include "Representations/Modeling/Path.h"
#include "Representations/Infrastructure/Image.h"

#include <Representations/Infrastructure/ButtonData.h>

// tools
#include "Tools/Math/Common.h"

BEGIN_DECLARE_MODULE(SensorSymbols2011)
  REQUIRE(UltraSoundReceiveData)
  REQUIRE(GyrometerData)
  REQUIRE(InertialSensorData)
  REQUIRE(IRReceiveData)
  REQUIRE(BatteryData)
  REQUIRE(FrameInfo)
  REQUIRE(BodyState)
  REQUIRE(ObstacleModel)
  REQUIRE(RadarGrid)
  REQUIRE(Image)
  REQUIRE(Path)

  REQUIRE(ButtonData)
END_DECLARE_MODULE(SensorSymbols2011)

class SensorSymbols2011: public SensorSymbols2011Base
{
public:

  SensorSymbols2011()
  :
    ultraSoundReceiveData(getUltraSoundReceiveData()),
    gyrometerData(getGyrometerData()),
    inertialSensorData(getInertialSensorData()),
    irreceiveData(getIRReceiveData()),
    batteryData(getBatteryData()),
    frameInfo(getFrameInfo()),
    bodyState(getBodyState()),
    obstacleModel(getObstacleModel()),
    radarGrid(getRadarGrid()),
    path(getPath()),
    image(getImage()),
    enableInertialSensorCalibrate(false),
    forceGetCameraSettings(false),
    forceGetCameraSettingsOldValue(false),
    resetingCamera(false),
    isCameraReseting(false)
  {
    theInstance = this;
    parameter_obstDistByAngle_angle = 0;
  }

  /** registers the symbols at an engine */
  void registerSymbols(xabsl::Engine& engine);

  /** update internal symbol state */
  virtual void execute();

  virtual ~SensorSymbols2011();

private:
  UltraSoundReceiveData const& ultraSoundReceiveData;
  GyrometerData const& gyrometerData;
  InertialSensorData const& inertialSensorData;
  IRReceiveData const& irreceiveData;
  BatteryData const& batteryData;
  FrameInfo const& frameInfo;
  BodyState const& bodyState;
  ObstacleModel const& obstacleModel;
  RadarGrid const& radarGrid;
  Path const& path;
  Image const& image;


  bool enableInertialSensorCalibrate;
  bool forceGetCameraSettings;
  bool forceGetCameraSettingsOldValue;

  static SensorSymbols2011* theInstance;

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

  //Radar Obstacle
  static bool getIfObstacleSeen();
  static double getObstDistByAngle();
  double parameter_obstDistByAngle_angle;

  static double getTimeSinceObstacleWasSeen();
  static double getObstacleDistance();

  static double getCameraBufferFailedCount();
  bool resetingCamera;
  bool isCameraReseting;
  static double getInertialSensorX();
  static double getInertialSensorY();

  static bool getBumberLeftPressed();
};

#endif  /* _SENSORSYMBOLS2011_H */

