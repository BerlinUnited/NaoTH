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


void Motion::init(naoth::PlatformDataInterface& platformInterface)
{
  std::cout << "Motion register start" << std::endl;
  /*
  REGISTER_INPUT(SensorJointData);
  REGISTER_INPUT(FrameInfo);
  REGISTER_INPUT(InertialSensorData);
  REGISTER_INPUT(FSRData);
  REGISTER_INPUT(AccelerometerData);
  REGISTER_INPUT(GyrometerData);
  */

  std::cout << "Motion register end" << std::endl;
}//end init


void Motion::call()
{
  
}//end call
