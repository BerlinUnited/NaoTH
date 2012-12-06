/* 
 * File:   SensorSymbols.h
 * Author: thomas
 *
 * Created on 9. April 2009, 18:10
 */

#ifndef _SENSORSYMBOLS_H
#define _SENSORSYMBOLS_H

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
#include "Representations/Modeling/CollisionModel.h"
#include "Representations/Modeling/Path.h"
#include "Representations/Infrastructure/Image.h"

#include <Representations/Infrastructure/ButtonData.h>

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
  REQUIRE(ObstacleModel)
  REQUIRE(RadarGrid)
  PROVIDE(Path)
  REQUIRE(Image)
  REQUIRE(CollisionModel)

  REQUIRE(ButtonData)
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
    obstacleModel(getObstacleModel()),
    collisionModel(getCollisionModel()),
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
  ObstacleModel const& obstacleModel;
  CollisionModel const& collisionModel;
  RadarGrid const& radarGrid;
  Path& path;
  Image const& image;


  bool enableInertialSensorCalibrate;
  bool forceGetCameraSettings;
  bool forceGetCameraSettingsOldValue;

  static SensorSymbols* theInstance;

  //get-method
  static double getIRButtonNumber();
  static double getFrameNumber();
  static double simplePassLeftSensor();
  static double simplePassRightSensor();

  static int getFallDownState();

  static double getObstacleDistance();
  static double getObstacleDistanceLeft();
  static double getObstacleDistanceRight();
  static double getBlockedTime();

  static double getTargetPointX();
  static double getTargetPointY();

  static double getTimeNoNodeExpandable();

  static void setTargetpointX(double targetX);
  static void setTargetpointY(double targetY);

  static double getCameraBufferFailedCount();
  bool resetingCamera;
  bool isCameraReseting;
  static double getInertialSensorX();
  static double getInertialSensorY();

  static bool getBumberLeftPressed();
};

#endif  /* _SENSORSYMBOLS_H */

