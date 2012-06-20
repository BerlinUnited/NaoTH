/* 
 * File:   SensorSymbols.cpp
 * Author: thomas
 * 
 * Created on 9. April 2009, 18:10
 */

#include "SensorSymbols.h"
//#include "Representations/Infrastructure/CameraSettings.h"
#include "Tools/Debug/DebugRequest.h"

void SensorSymbols::registerSymbols(xabsl::Engine& engine)
{
  engine.registerDecimalInputSymbol("sensor.gyro.x", &gyrometerData.data.x);
  engine.registerDecimalInputSymbol("sensor.gyro.y", &gyrometerData.data.y);

  engine.registerDecimalInputSymbol("sensor.inertial.x", &getInertialSensorX);
  engine.registerDecimalInputSymbol("sensor.inertial.y", &getInertialSensorY);

  // not used now
  engine.registerBooleanOutputSymbol("sensor.inertial.calibrate", &enableInertialSensorCalibrate);

  engine.registerDecimalInputSymbol("sensor.ultrasound", &ultraSoundReceiveData.rawdata);
  engine.registerDecimalInputSymbol("sensor.ultrasound.left", &simplePassLeftSensor);
  engine.registerDecimalInputSymbol("sensor.ultrasound.right", &simplePassRightSensor);

  engine.registerDecimalInputSymbol("button.remote", &getIRButtonNumber);

  engine.registerDecimalInputSymbol("platform.battery", &batteryData.charge);

  engine.registerEnumElement("fall_down_state", "fall_down_state.undefined", BodyState::undefined);
  engine.registerEnumElement("fall_down_state", "fall_down_state.upright", BodyState::upright);
  engine.registerEnumElement("fall_down_state", "fall_down_state.lying_on_front", BodyState::lying_on_front);
  engine.registerEnumElement("fall_down_state", "fall_down_state.lying_on_back", BodyState::lying_on_back);
  engine.registerEnumElement("fall_down_state", "fall_down_state.lying_on_left_side", BodyState::lying_on_left_side);
  engine.registerEnumElement("fall_down_state", "fall_down_state.lying_on_right_side", BodyState::lying_on_right_side);

  engine.registerEnumeratedInputSymbol("fall_down_state", "fall_down_state", &getFallDownState);

  engine.registerDecimalInputSymbol("platform.frameNumber", &getFrameNumber);

  engine.registerDecimalInputSymbol("obstacle.ultrasound.distance", &getObstacleDistance);
  engine.registerDecimalInputSymbol("obstacle.ultrasound.left.distance", &getObstacleDistanceLeft);
  engine.registerDecimalInputSymbol("obstacle.ultrasound.right.distance", &getObstacleDistanceRight);
  engine.registerDecimalInputSymbol("obstacle.ultrasound.blockedtime", &getBlockedTime);

  engine.registerBooleanInputSymbol("collision.colliding", &collisionModel.isColliding);

  // integrated obstacle model
  engine.registerDecimalInputSymbol("path.next_point_to_go_x", &path.nextPointToGo.x);
  engine.registerDecimalInputSymbol("path.next_point_to_go_y", &path.nextPointToGo.y);
  engine.registerDecimalInputSymbol("path.time_since_not_valid", &getTimeNoNodeExpandable);

  // target to control the path
  engine.registerDecimalOutputSymbol("path.target_x", &setTargetpointX, &getTargetPointX);
  engine.registerDecimalOutputSymbol("path.target_y", &setTargetpointY, &getTargetPointY);

  /*engine.registerDecimalInputSymbol("obstacle.radial.direction_free", &getObstacleRadialDirection_free);
  engine.registerDecimalInputSymbolDecimalParameter("obstacle.radial.direction_free", "obstacle.radial.direction_free.deg", &directionParameter);
  engine.registerDecimalInputSymbol("obstacle.radial.passageway_free", &getObstacleRadialPassageway_free);
  engine.registerDecimalInputSymbolDecimalParameter("obstacle.radial.passageway_free", "obstacle.radial.passageway_free.deg1", &passagewayParameter1);
  engine.registerDecimalInputSymbolDecimalParameter("obstacle.radial.passageway_free", "obstacle.radial.passageway_free.deg2", &passagewayParameter2);
  engine.registerDecimalInputSymbol("obstacle.radial.obstacle_left", &getObstacleRadialObstacle_left);
  engine.registerDecimalInputSymbol("obstacle.radial.obstacle_left", &getObstacleRadialObstacle_right);
  engine.registerDecimalInputSymbol("obstacle.radial.obstacle_left", &getObstacleRadialObstacle_front);*/
  
  engine.registerDecimalInputSymbol("sensor.camera.buffer_failed_count", &getCameraBufferFailedCount);
  engine.registerBooleanOutputSymbol("sensor.camera.reset", &resetingCamera);

  engine.registerBooleanInputSymbol("button.bumper.pressed.left", &getBumberLeftPressed);
  

  isCameraReseting = false;
  resetingCamera = false;
}//end registerSymbols

SensorSymbols* SensorSymbols::theInstance = NULL;

SensorSymbols::~SensorSymbols()
{
}

void SensorSymbols::execute()
{
  /*
  if(forceGetCameraSettings)
  {
    cameraSettingsRequest.queryCameraSettings = true;
    forceGetCameraSettingsOldValue = true;
  }
  else if(forceGetCameraSettingsOldValue == true)
  {
    cameraSettingsRequest.queryCameraSettings = false;
    forceGetCameraSettingsOldValue = false;
  }
  */
}//end update


bool SensorSymbols::getBumberLeftPressed()
{
  return (theInstance->getButtonData().eventCounter[ButtonData::LeftFootLeft] > 0) ||
         (theInstance->getButtonData().eventCounter[ButtonData::LeftFootRight] > 0);
}

double SensorSymbols::getIRButtonNumber()
{
    return (double)(theInstance->irreceiveData.data[IRReceiveData::RightRCByte2]);
}//end getIRButtonNumber

double SensorSymbols::getFrameNumber()
{
  return (double) (theInstance->frameInfo.getFrameNumber());
}


int SensorSymbols::getFallDownState()
{
  return (int)(theInstance->bodyState.fall_down_state);
}//end getFallDownState

// returns whether some data is received or not
double SensorSymbols::simplePassLeftSensor()
{
  double r = 2550.0;
  // return minimum measurement = closest object
  for(unsigned int i = 0; i < UltraSoundData::numOfUSEcho; i++)
  {
    if((theInstance->ultraSoundReceiveData.dataLeft[i] * 1000) < r && theInstance->ultraSoundReceiveData.dataLeft[i] > 0.2)
    {
      r = theInstance->ultraSoundReceiveData.dataLeft[i] * 1000;
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
    if((theInstance->ultraSoundReceiveData.dataRight[i] * 1000) < r && theInstance->ultraSoundReceiveData.dataRight[i] > 0.2)
    {
      r = theInstance->ultraSoundReceiveData.dataRight[i] * 1000;
    }
  }
  return r;
} // end simpleRightUSSensor


double SensorSymbols::getObstacleDistance()
{
  return theInstance->obstacleModel.frontDistance;
}//end getObstacleDistance

double SensorSymbols::getObstacleDistanceLeft()
{
  return theInstance->obstacleModel.leftDistance;
}//end getObstacleDistanceLeft

double SensorSymbols::getObstacleDistanceRight()
{
  return theInstance->obstacleModel.rightDistance;
}//end getObstacleDistanceRight

double SensorSymbols::getBlockedTime()
{
  return theInstance->obstacleModel.blockedTime;
}//end getBlockedTime

double SensorSymbols::getCameraBufferFailedCount()
{
  return (theInstance->image.possibleImageStuck)?1:0;
}

double SensorSymbols::getInertialSensorX()
{
  return Math::toDegrees(theInstance->inertialSensorData.data.x);
}

double SensorSymbols::getInertialSensorY()
{
  return Math::toDegrees(theInstance->inertialSensorData.data.y);
}

double SensorSymbols::getTargetPointX()
{
  return theInstance->path.targetPoint.x;
}

double SensorSymbols::getTargetPointY()
{
  return theInstance->path.targetPoint.y;
}

double SensorSymbols::getTimeNoNodeExpandable()
{
  return theInstance->path.getTimeNoNodeExpandable();
}//end getBallTimeSeen

void SensorSymbols::setTargetpointX(double targetX)
{
  theInstance->path.targetPoint.x = targetX;
}

void SensorSymbols::setTargetpointY(double targetY)
{
  theInstance->path.targetPoint.y = targetY;
}
