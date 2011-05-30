/**
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 */

#include "Sensor.h"
#include "Tools/SwapSpace/SwapSpace.h"

Sensor::Sensor()
{
}

Sensor::~Sensor()
{
}


#define REG_INPUT(R) \
  platformInterface.registerCognitionInput(get##R())

void Sensor::init(naoth::PlatformDataInterface& platformInterface)
{
  REG_INPUT(AccelerometerData);
  REG_INPUT(BatteryData);
  REG_INPUT(ButtonData);
  REG_INPUT(GyrometerData);
  REG_INPUT(IRReceiveData);
  REG_INPUT(Image);
  REG_INPUT(InertialSensorData);
  REG_INPUT(FSRData);
  REG_INPUT(SensorJointData);
  REG_INPUT(UltraSoundReceiveData);
  
  REG_INPUT(FrameInfo);
  REG_INPUT(CurrentCameraSettings);
}//end init


void Sensor::execute()
{
  //unsigned int lastTime = theFrameInfo.time;

  // data from motion
  //STOPWATCH_START("Cognition-PullSwap");
  SwapSpace::getInstance().theMotionCache.pull(
    getMotionStatus(),
    getOdometryData());
  //STOPWATCH_STOP("Cognition-PullSwap");
  
  //PLOT("Cognition-Time", theFrameInfo.getTimeSince(lastTime));
  //STOPWATCH_START("Cognition-Main");
}//end execute

