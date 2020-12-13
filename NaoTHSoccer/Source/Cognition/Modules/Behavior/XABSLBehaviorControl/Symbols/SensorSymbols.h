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

#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Infrastructure/UltraSoundData.h"
#include "Representations/Infrastructure/GyrometerData.h"
#include "Representations/Infrastructure/InertialSensorData.h"
#include "Representations/Infrastructure/BatteryData.h"
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Modeling/BodyState.h"
#include "Representations/Modeling/ObstacleModel.h"
#include "Representations/Infrastructure/Image.h"
#include "Representations/Infrastructure/CalibrationData.h"
#include "Representations/Motion/CollisionPercept.h"

#include <Representations/Infrastructure/ButtonData.h>
#include <Representations/Infrastructure/AudioControl.h>
// tools
#include "Tools/Math/Common.h"

BEGIN_DECLARE_MODULE(SensorSymbols)
  REQUIRE(UltraSoundReceiveData)
  REQUIRE(GyrometerData)
  REQUIRE(InertialSensorData)
  REQUIRE(BatteryData)
  REQUIRE(FrameInfo)
  REQUIRE(BodyState)
  REQUIRE(ObstacleModel)
  REQUIRE(Image)
  REQUIRE(CalibrationData)
  REQUIRE(ButtonData)
  REQUIRE(CollisionPercept)

  PROVIDE(AudioControl)
END_DECLARE_MODULE(SensorSymbols)

class SensorSymbols: public SensorSymbolsBase
{
public:

  SensorSymbols()
  {
    theInstance = this;
  }

  /** registers the symbols at an engine */
  void registerSymbols(xabsl::Engine& engine);

  /** update internal symbol state */
  virtual void execute();

  virtual ~SensorSymbols();

private:
  static SensorSymbols* theInstance;

  //getters
  static double getFrameNumber();
  static double simplePassLeftSensor();
  static double simplePassRightSensor();

  static int getFallDownState();

  static double getObstacleDistance();
  static double getObstacleDistanceLeft();
  static double getObstacleDistanceRight();
  static double getBlockedTime();
  static double getFootStateTime();
  static double getTargetPointX();
  static double getTargetPointY();

  static double getTimeNoNodeExpandable();

  static void setTargetpointX(double targetX);
  static void setTargetpointY(double targetY);

  static double getInertialSensorX();
  static double getInertialSensorY();

  static bool getBumperLeftPressed();
  static bool getBumperRightPressed();
  static bool getButtonHeadMiddle();
  static bool getButtonHeadFront();
  static bool getButtonHeadRear();

  static bool getCollisionArmLeft();
  static bool getCollisionArmRight();
};

#endif  /* _SENSORSYMBOLS_H */

