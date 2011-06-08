/**
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 */

#include "Sensor.h"

Sensor::Sensor():
theMotionStatusMsgQueue(NULL),
theOdometryDataMsgQueue(NULL)
{
}

Sensor::~Sensor()
{
}


#define REG_INPUT(R) \
  platformInterface.registerCognitionInput(get##R())

void Sensor::init(naoth::PlatformInterfaceBase& platformInterface)
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
  
  theMotionStatusMsgQueue = platformInterface.getMessageQueue("MotionStatus");
  theOdometryDataMsgQueue = platformInterface.getMessageQueue("OdometryData");
}//end init


void Sensor::execute()
{
  // data from motion
  if ( !theMotionStatusMsgQueue->empty() )
  {
    string msg = theMotionStatusMsgQueue->read();
    // drop old message, TODO: use them!
    while ( !theMotionStatusMsgQueue->empty() )
    {
      msg = theMotionStatusMsgQueue->read();
    }
    stringstream ss(msg);
    Serializer<MotionStatus>::deserialize(ss, getMotionStatus());
  }
  
  if ( !theOdometryDataMsgQueue->empty() )
  {
    string msg = theOdometryDataMsgQueue->read();
    // drop old message, TODO: use them!
    while ( !theOdometryDataMsgQueue->empty() )
    {
      msg = theOdometryDataMsgQueue->read();
    }
    stringstream ss(msg);
    Serializer<OdometryData>::deserialize(ss, getOdometryData());
  }

}//end execute

