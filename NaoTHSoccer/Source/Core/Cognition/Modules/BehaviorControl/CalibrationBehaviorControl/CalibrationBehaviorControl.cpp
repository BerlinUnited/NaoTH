#include "CalibrationBehaviorControl.h"

#include "Tools/Debug/DebugRequest.h"

CalibrationBehaviorControl::CalibrationBehaviorControl()
  : lastChestButtonEventCounter(0), state(wait)
{
  DEBUG_REQUEST_REGISTER("CalibrationBehaviorControl:restart", "Restart the calibration process", false);
  DEBUG_REQUEST_REGISTER("CalibrationBehaviorControl:end", "End the calibration process", false);
  DEBUG_REQUEST_REGISTER("CalibrationBehaviorControl:fsrTouch", "Force FSR calibration", false);
}


void CalibrationBehaviorControl::execute()
{
  MotionRequest& motionRequest = getMotionRequest();
  LEDRequest& ledRequest = getBehaviorLEDRequest();

  getMotionRequest().calibrateFootTouchDetector = false;
  getHeadMotionRequest().id = HeadMotionRequest::look_straight_ahead;

  for(int i=0; i < LEDData::numOfMonoLED; i++)
  {
    ledRequest.request.theMonoLED[i] = 0.0;
  }

  DEBUG_REQUEST("CalibrationBehaviorControl:restart",
    state = inertial;
    stateChanged = getFrameInfo();
  );
  DEBUG_REQUEST("CalibrationBehaviorControl:end",
    state = wait;
    stateChanged = getFrameInfo();
  );
  DEBUG_REQUEST("CalibrationBehaviorControl:fsrTouch",
    state = fsrTouch;
    stateChanged = getFrameInfo();
  );

  switch(state)
  {
  case inertial:

    setLED(1);

    calibrateInertial();


    // wait 15 seconds in this state
    if(getButtonData().eventCounter[ButtonData::Chest] > lastChestButtonEventCounter)
    {
      state = wait;
      stateChanged = getFrameInfo();
    }
    else if(getFrameInfo().getTimeSince(stateChanged.getTime()) > 15000)
    {
      state = fsrTouch;
      stateChanged = getFrameInfo();
    }

    break;
  case fsrTouch:
    setLED(2);

    calibrateFSRTouch();

    // wait 30 seconds in this state
    if(getButtonData().eventCounter[ButtonData::Chest] > lastChestButtonEventCounter)
    {
      state = wait;
      stateChanged = getFrameInfo();
    }
    else if(getFrameInfo().getTimeSince(stateChanged.getTime()) > 30000)
    {
      state = wait;
      stateChanged = getFrameInfo();
    }

    break;
  default:
    setLED(0);

    motionRequest.id = motion::init;

    if(getButtonData().eventCounter[ButtonData::Chest] > lastChestButtonEventCounter)
    {
      state = inertial;
      stateChanged = getFrameInfo();
    }
  }

  lastChestButtonEventCounter = getButtonData().eventCounter[ButtonData::Chest];
}

void CalibrationBehaviorControl::setLED(unsigned int step)
{
  step = std::min((unsigned int) 9, step);

  if(step == 0)
  {
    setChest(1.0, 0.0, 0.0);
    for(int i=LEDData::FaceLeft0; i <= LEDData::FaceLeft315; i++)
    {
      getBehaviorLEDRequest().request.theMultiLED[i][LEDData::GREEN] = 0.0;
      getBehaviorLEDRequest().request.theMultiLED[i][LEDData::RED] = 0.0;
      getBehaviorLEDRequest().request.theMultiLED[i][LEDData::BLUE] =  0.0;
    }
    for(int i=LEDData::FaceRight0; i <= LEDData::FaceRight315; i++)
    {
      getBehaviorLEDRequest().request.theMultiLED[i][LEDData::GREEN] =  0.0;
      getBehaviorLEDRequest().request.theMultiLED[i][LEDData::RED] = 0.0;
      getBehaviorLEDRequest().request.theMultiLED[i][LEDData::BLUE] =  0.0;
    }

  }
  else
  {
    setChest(0.0, 1.0, 0.0);

    for(unsigned int i=LEDData::EarLeft0; i <=LEDData::EarLeft324; i++)
    {
      getBehaviorLEDRequest().request.theMonoLED[i] = 0.0;
    }
    for(unsigned int i=LEDData::EarRight0; i <=LEDData::EarRight324; i++)
    {
      getBehaviorLEDRequest().request.theMonoLED[i] = 0.0;
    }

    if(getFrameInfo().getFrameNumber() % 2 == 0)
    {
      getBehaviorLEDRequest().request.theMonoLED[LEDData::EarLeft0 + step-1] = 1.0;
      getBehaviorLEDRequest().request.theMonoLED[LEDData::EarRight0 + step-1] = 1.0;
    }

    for(int i=LEDData::FaceLeft0; i <= LEDData::FaceLeft315; i++)
    {
      getBehaviorLEDRequest().request.theMultiLED[i][LEDData::GREEN] =  1.0;
      getBehaviorLEDRequest().request.theMultiLED[i][LEDData::RED] = 0.0;
      getBehaviorLEDRequest().request.theMultiLED[i][LEDData::BLUE] =  0.0;
    }
    for(int i=LEDData::FaceRight0; i <= LEDData::FaceRight315; i++)
    {
      getBehaviorLEDRequest().request.theMultiLED[i][LEDData::GREEN] = 1.0;
      getBehaviorLEDRequest().request.theMultiLED[i][LEDData::RED] = 0.0;
      getBehaviorLEDRequest().request.theMultiLED[i][LEDData::BLUE] =  0.0;
    }
  }
}

void CalibrationBehaviorControl::setChest(double red, double green, double blue)
{
  getGameControllerLEDRequest().request.theMultiLED[LEDData::ChestButton][LEDData::RED] = red;
  getGameControllerLEDRequest().request.theMultiLED[LEDData::ChestButton][LEDData::GREEN] = green;
  getGameControllerLEDRequest().request.theMultiLED[LEDData::ChestButton][LEDData::BLUE] = blue;
}

void CalibrationBehaviorControl::calibrateInertial()
{
  // calibration is done always when standing
  getMotionRequest().id = motion::stand;
}

void CalibrationBehaviorControl::calibrateFSRTouch()
{
  getMotionRequest().id = motion::walk;
  getMotionRequest().walkRequest.target.translation.x = 0;
  getMotionRequest().walkRequest.target.translation.y = 0;
  getMotionRequest().walkRequest.target.rotation = 0;

  getMotionRequest().calibrateFootTouchDetector = true;
}
