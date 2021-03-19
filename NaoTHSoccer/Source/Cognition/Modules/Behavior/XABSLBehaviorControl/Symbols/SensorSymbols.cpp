/* 
 * File:   SensorSymbols.cpp
 * Author: thomas
 * 
 * Created on 9. April 2009, 18:10
 */

#include "SensorSymbols.h"

void SensorSymbols::registerSymbols(xabsl::Engine& engine)
{
  engine.registerDecimalInputSymbol("sensor.gyro.x", &getGyrometerData().data.x);
  engine.registerDecimalInputSymbol("sensor.gyro.y", &getGyrometerData().data.y);

  engine.registerDecimalInputSymbol("sensor.inertial.x", &getInertialSensorX);
  engine.registerDecimalInputSymbol("sensor.inertial.y", &getInertialSensorY);

  engine.registerBooleanInputSymbol("sensor.inertial.calibrated", &getCalibrationData().calibrated);

  engine.registerDecimalInputSymbol("sensor.ultrasound", &getUltraSoundReceiveData().rawdata);
  engine.registerDecimalInputSymbol("sensor.ultrasound.left", &simplePassLeftSensor);
  engine.registerDecimalInputSymbol("sensor.ultrasound.right", &simplePassRightSensor);

  //engine.registerDecimalInputSymbol("platform.battery", &getBatteryData().charge);
  engine.registerDecimalInputSymbol("platform.battery", &getBodyState().batteryCharge);

  engine.registerEnumElement("fall_down_state", "fall_down_state.undefined", BodyState::undefined);
  engine.registerEnumElement("fall_down_state", "fall_down_state.upright", BodyState::upright);
  engine.registerEnumElement("fall_down_state", "fall_down_state.lying_on_front", BodyState::lying_on_front);
  engine.registerEnumElement("fall_down_state", "fall_down_state.lying_on_back", BodyState::lying_on_back);
  engine.registerEnumElement("fall_down_state", "fall_down_state.lying_on_left_side", BodyState::lying_on_left_side);
  engine.registerEnumElement("fall_down_state", "fall_down_state.lying_on_right_side", BodyState::lying_on_right_side);

  engine.registerEnumeratedInputSymbol("fall_down_state", "fall_down_state", &getFallDownState);

  engine.registerDecimalInputSymbol("body.temperature.leg.left", &getBodyState().temperatureLeftLeg);
  engine.registerDecimalInputSymbol("body.temperature.leg.right", &getBodyState().temperatureRightLeg);

  engine.registerBooleanInputSymbol("body.foot.groundcontact.left", &getBodyState().standByLeftFoot);
  engine.registerBooleanInputSymbol("body.foot.groundcontact.right", &getBodyState().standByRightFoot);
  engine.registerDecimalInputSymbol("body.foot.groundcontact.statetime", &getFootStateTime);

  engine.registerBooleanInputSymbol("body.lifted_up", &getBodyState().isLiftedUp);

  engine.registerBooleanInputSymbol("body.collision.arm.left", &getCollisionArmLeft);
  engine.registerBooleanInputSymbol("body.collision.arm.right", &getCollisionArmRight);

  engine.registerDecimalInputSymbol("platform.frameNumber", &getFrameNumber);

  engine.registerDecimalInputSymbol("obstacle.ultrasound.distance", &getObstacleDistance);
  engine.registerDecimalInputSymbol("obstacle.ultrasound.left.distance", &getObstacleDistanceLeft);
  engine.registerDecimalInputSymbol("obstacle.ultrasound.right.distance", &getObstacleDistanceRight);
  engine.registerDecimalInputSymbol("obstacle.ultrasound.blockedtime", &getBlockedTime);

  engine.registerBooleanInputSymbol("button.bumper.pressed.left", &getBumperLeftPressed);
  engine.registerBooleanInputSymbol("button.bumper.pressed.right", &getBumperRightPressed);
  engine.registerBooleanInputSymbol("button.head.pressed.front", &getButtonHeadFront);
  engine.registerBooleanInputSymbol("button.head.pressed.middle", &getButtonHeadMiddle);
  engine.registerBooleanInputSymbol("button.head.pressed.rear", &getButtonHeadRear);

  // deprecated?
  engine.registerBooleanInputSymbol("battery.isDischarging", &getBodyState().isDischarging);
  engine.registerBooleanInputSymbol("battery.isCharging", &getBodyState().isCharging);

  engine.registerBooleanOutputSymbol("audio.capture", &getAudioControl().capture);

}//end registerSymbols

SensorSymbols* SensorSymbols::theInstance = NULL;

SensorSymbols::~SensorSymbols()
{
}

void SensorSymbols::execute()
{

}//end execute

bool SensorSymbols::getBumperLeftPressed()
{
    return theInstance->getButtonData().isPressed[ButtonData::LeftFootLeft] || theInstance->getButtonData().isPressed[ButtonData::LeftFootRight];
  /*return (theInstance->getButtonData().eventCounter[ButtonData::LeftFootLeft] > 0) ||
         (theInstance->getButtonData().eventCounter[ButtonData::LeftFootRight] > 0);*/
}
bool SensorSymbols::getBumperRightPressed()
{
    return theInstance->getButtonData().isPressed[ButtonData::RightFootLeft] || theInstance->getButtonData().isPressed[ButtonData::RightFootRight];
    /*return (theInstance->getButtonData().eventCounter[ButtonData::LeftFootLeft] > 0) ||
    (theInstance->getButtonData().eventCounter[ButtonData::LeftFootRight] > 0);*/
}
bool SensorSymbols::getButtonHeadFront()
{
  return (theInstance->getButtonData().isPressed[ButtonData::HeadFront]);
}
bool SensorSymbols::getButtonHeadMiddle()
{
  return (theInstance->getButtonData().isPressed[ButtonData::HeadMiddle]);
}
bool SensorSymbols::getButtonHeadRear()
{
  return (theInstance->getButtonData().isPressed[ButtonData::HeadRear]);
}

double SensorSymbols::getFrameNumber()
{
  return (double) (theInstance->getFrameInfo().getFrameNumber());
}

double SensorSymbols::getFootStateTime()
{
  return (double) theInstance->getFrameInfo().getTimeSince(theInstance->getBodyState().foot_state_time);
}

int SensorSymbols::getFallDownState()
{
  return (int)(theInstance->getBodyState().fall_down_state);
}

// returns whether some data is received or not
double SensorSymbols::simplePassLeftSensor()
{
  double r = 2550.0;
  // return minimum measurement = closest object
  for(size_t i = 0; i < UltraSoundReceiveData::numOfUSEcho; i++)
  {
    if((theInstance->getUltraSoundReceiveData().dataLeft[i] * 1000) < r && theInstance->getUltraSoundReceiveData().dataLeft[i] > 0.2)
    {
      r = theInstance->getUltraSoundReceiveData().dataLeft[i] * 1000;
    }
  }
  return r;
} // end simpleLeftUSSensor

double SensorSymbols::simplePassRightSensor()
{
  double r = 2550.0;
  // return minimum measurement = closest object
  for(size_t i = 0; i < UltraSoundReceiveData::numOfUSEcho; i++)
  {
    if((theInstance->getUltraSoundReceiveData().dataRight[i] * 1000) < r && theInstance->getUltraSoundReceiveData().dataRight[i] > 0.2)
    {
      r = theInstance->getUltraSoundReceiveData().dataRight[i] * 1000;
    }
  }
  return r;
} // end simpleRightUSSensor


double SensorSymbols::getObstacleDistance()
{
  return theInstance->getObstacleModel().frontDistance;
}

double SensorSymbols::getObstacleDistanceLeft()
{
  return theInstance->getObstacleModel().leftDistance;
}

double SensorSymbols::getObstacleDistanceRight()
{
  return theInstance->getObstacleModel().rightDistance;
}

double SensorSymbols::getBlockedTime()
{
  return theInstance->getObstacleModel().blockedTime;
}

double SensorSymbols::getInertialSensorX()
{
  return Math::toDegrees(theInstance->getInertialSensorData().data.x);
}

double SensorSymbols::getInertialSensorY()
{
  return Math::toDegrees(theInstance->getInertialSensorData().data.y);
}

bool SensorSymbols::getCollisionArmLeft()
{
  return
    theInstance->getCollisionPercept().timeCollisionArmLeft > 0 &&
    theInstance->getFrameInfo().getTimeSince(theInstance->getCollisionPercept().timeCollisionArmLeft) < 100;
}

bool SensorSymbols::getCollisionArmRight()
{
  return
    theInstance->getCollisionPercept().timeCollisionArmRight > 0 &&
    theInstance->getFrameInfo().getTimeSince(theInstance->getCollisionPercept().timeCollisionArmRight) < 100;
}
