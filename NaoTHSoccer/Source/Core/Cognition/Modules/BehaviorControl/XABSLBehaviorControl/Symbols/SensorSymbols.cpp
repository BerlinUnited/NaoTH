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

  engine.registerDecimalInputSymbol("button.remote", &getIRButtonNumber);

  engine.registerDecimalInputSymbol("platform.battery", &getBatteryData().charge);

  engine.registerEnumElement("fall_down_state", "fall_down_state.undefined", BodyState::undefined);
  engine.registerEnumElement("fall_down_state", "fall_down_state.upright", BodyState::upright);
  engine.registerEnumElement("fall_down_state", "fall_down_state.lying_on_front", BodyState::lying_on_front);
  engine.registerEnumElement("fall_down_state", "fall_down_state.lying_on_back", BodyState::lying_on_back);
  engine.registerEnumElement("fall_down_state", "fall_down_state.lying_on_left_side", BodyState::lying_on_left_side);
  engine.registerEnumElement("fall_down_state", "fall_down_state.lying_on_right_side", BodyState::lying_on_right_side);

  engine.registerEnumeratedInputSymbol("fall_down_state", "fall_down_state", &getFallDownState);


  engine.registerDecimalInputSymbol("body.temperature.leg.left", &getBodyState().temperatureLeftLeg);
  engine.registerDecimalInputSymbol("body.temperature.leg.right", &getBodyState().temperatureRightLeg);


  engine.registerDecimalInputSymbol("platform.frameNumber", &getFrameNumber);

  engine.registerDecimalInputSymbol("obstacle.ultrasound.distance", &getObstacleDistance);
  engine.registerDecimalInputSymbol("obstacle.ultrasound.left.distance", &getObstacleDistanceLeft);
  engine.registerDecimalInputSymbol("obstacle.ultrasound.right.distance", &getObstacleDistanceRight);
  engine.registerDecimalInputSymbol("obstacle.ultrasound.blockedtime", &getBlockedTime);

  engine.registerBooleanInputSymbol("collision.colliding", &getCollisionModel().isColliding);

  // integrated obstacle model
  engine.registerDecimalInputSymbol("path.next_point_to_go_x", &getPath().nextPointToGo.x);
  engine.registerDecimalInputSymbol("path.next_point_to_go_y", &getPath().nextPointToGo.y);
  engine.registerDecimalInputSymbol("path.time_since_not_valid", &getTimeNoNodeExpandable);

  // target to control the path
  engine.registerDecimalOutputSymbol("path.target_x", &setTargetpointX, &getTargetPointX);
  engine.registerDecimalOutputSymbol("path.target_y", &setTargetpointY, &getTargetPointY);
  
  engine.registerBooleanInputSymbol("button.bumper.pressed.left", &getBumberLeftPressed);
}//end registerSymbols

SensorSymbols* SensorSymbols::theInstance = NULL;

SensorSymbols::~SensorSymbols()
{
}

void SensorSymbols::execute()
{

}//end execute


bool SensorSymbols::getBumberLeftPressed()
{
  return (theInstance->getButtonData().eventCounter[ButtonData::LeftFootLeft] > 0) ||
         (theInstance->getButtonData().eventCounter[ButtonData::LeftFootRight] > 0);
}

double SensorSymbols::getIRButtonNumber()
{
  return (double)(theInstance->getIRReceiveData().data[IRReceiveData::RightRCByte2]);
}

double SensorSymbols::getFrameNumber()
{
  return (double) (theInstance->getFrameInfo().getFrameNumber());
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
  for(unsigned int i = 0; i < UltraSoundData::numOfUSEcho; i++)
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
  for(unsigned int i = 0; i < UltraSoundData::numOfUSEcho; i++)
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

double SensorSymbols::getTargetPointX()
{
  return theInstance->getPath().targetPoint.x;
}

double SensorSymbols::getTargetPointY()
{
  return theInstance->getPath().targetPoint.y;
}

double SensorSymbols::getTimeNoNodeExpandable()
{
  return theInstance->getPath().getTimeNoNodeExpandable();
}

void SensorSymbols::setTargetpointX(double targetX)
{
  theInstance->getPath().targetPoint.x = targetX;
}

void SensorSymbols::setTargetpointY(double targetY)
{
  theInstance->getPath().targetPoint.y = targetY;
}
