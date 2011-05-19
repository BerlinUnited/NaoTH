/**
 * @file Motion.cpp
 *
  * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
 *
 */

#include "Motion.h"
#include <glib.h>

using namespace naoth;
using namespace motion;

Motion::Motion()
{
}

Motion::~Motion()
{
}

void Motion::init(naoth::PlatformDataInterface& platformInterface)
{
  theBlackBoard.init();
  
  g_message("Motion register begin");
#define REG_INPUT(R)                                                    \
  platformInterface.registerMotionInput(theBlackBoard.the##R)
  
  REG_INPUT(SensorJointData);
  REG_INPUT(FrameInfo);
  REG_INPUT(InertialSensorData);
  REG_INPUT(FSRData);
  REG_INPUT(AccelerometerData);
  REG_INPUT(GyrometerData);
  
#define REG_OUTPUT(R)                                                   \
  platformInterface.registerMotionOutput(theBlackBoard.the##R)

  REG_OUTPUT(MotorJointData);
  g_message("Motion register end");
}//end init


void Motion::call()
{
  
}//end call
