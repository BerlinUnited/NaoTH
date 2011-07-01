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
  engine.registerDecimalInputSymbol("sensor.gyro.x", &gyrometerData.data[GyrometerData::X]);
  engine.registerDecimalInputSymbol("sensor.gyro.y", &gyrometerData.data[GyrometerData::Y]);

  engine.registerDecimalInputSymbol("sensor.inertial.x", &getInertialSensorX);
  engine.registerDecimalInputSymbol("sensor.inertial.y", &getInertialSensorY);

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

  engine.registerDecimalInputSymbol("obstacle.ultrasound.time_since_seen", &getTimeSinceObstacleWasSeen);
  engine.registerDecimalInputSymbol("obstacle.ultrasound.distance", &getObstacleDistance);

  engine.registerBooleanInputSymbol("obstacle.visual.was_seen", &obstacleModel.visualObstacleWasSeen);
  engine.registerDecimalInputSymbol("obstacle.visual.x", &obstacleModel.posVisualObstacle.x);
  engine.registerDecimalInputSymbol("obstacle.visual.y", &obstacleModel.posVisualObstacle.y);

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

  resetCamera(resetingCamera);
}//end update

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
  for(unsigned int i = 0; i < UltraSoundData::numOfIRSend; i++)
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
  for(unsigned int i = 0; i < UltraSoundData::numOfIRSend; i++)
  {
    if((theInstance->ultraSoundReceiveData.dataRight[i] * 1000) < r && theInstance->ultraSoundReceiveData.dataRight[i] > 0.2)
    {
      r = theInstance->ultraSoundReceiveData.dataRight[i] * 1000;
    }
  }
  return r;
} // end simpleRightUSSensor


double SensorSymbols::getTimeSinceObstacleWasSeen()
{
  return (double)theInstance->frameInfo.getTimeSince(
    theInstance->obstacleModel.frameWhenObstacleWasSeen.getTime());
}//end getFrameWhenObstacleWasSeen

double SensorSymbols::getObstacleAngle()
{
  return Math::toDegrees(theInstance->obstacleModel.ultraSoundObstacleEstimation.angle());
}//end getObstacleAngle

double SensorSymbols::getObstacleDistance()
{
  return theInstance->obstacleModel.ultraSoundObstacleEstimation.abs();
}//end getObstacleDistance

double SensorSymbols::getCameraBufferFailedCount()
{
  return (theInstance->image.possibleImageStuck)?1:0;
}

void SensorSymbols::resetCamera(bool b)
{
  if (isCameraReseting != b)
  {
    stringstream answer;
    map<string, string> args;
    if (b)
    {
      args["on"] = "";
    } else
    {
      args["off"] = "";
    }
    DebugRequest::getInstance().executeDebugCommand("V4lCameraHandler:reset_camera", args, answer);
    isCameraReseting = b;
  }
}

  double SensorSymbols::getInertialSensorX()
  {
    return Math::toDegrees(theInstance->inertialSensorData.data.x);
  }

  double SensorSymbols::getInertialSensorY()
  {
    return Math::toDegrees(theInstance->inertialSensorData.data.y);
  }