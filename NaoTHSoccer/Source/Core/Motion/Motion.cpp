/**
 * @file Motion.cpp
 *
  * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
 *
 */

#include "Motion.h"


Motion::Motion()
{
}

Motion::~Motion()
{
}

#define REG_INPUT(R) \
  platformInterface.registerMotionInput(get##R())

#define REG_OUTPUT(R) \
  platformInterface.registerMotionOutput(get##R())

void Motion::init(naoth::PlatformDataInterface& platformInterface)
{
  g_message("Motion register begin");
  REG_INPUT(SensorJointData);
  REG_INPUT(FrameInfo);
  REG_INPUT(InertialSensorData);
  REG_INPUT(FSRData);
  REG_INPUT(AccelerometerData);
  REG_INPUT(GyrometerData);
  g_message("Motion register end");
}//end init


void Motion::call()
{
  
}//end call
