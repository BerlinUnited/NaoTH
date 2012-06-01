/**
 * @file IPCData.h
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 * @breief Inter-process communication data
 *
 */

#include "IPCData.h"
#include <cstring>
#include "Tools/NaoTime.h"

using namespace naoth;


void NaoSensorData::get(SensorJointData& data) const
{
  unsigned int currentIndex = theSensorJointDataIndex;
  for (int i = 0; i < JointData::RHipYawPitch; i++) 
  {
    data.electricCurrent[i] = sensorsValue[currentIndex++];
    data.temperature[i] = sensorsValue[currentIndex++];
    data.position[i] = sensorsValue[currentIndex++];
    data.stiffness[i] = sensorsValue[currentIndex++];
  }
  for (int i = JointData::RHipYawPitch + 1; i < JointData::numOfJoint; i++) 
  {
    data.electricCurrent[i] = sensorsValue[currentIndex++];
    data.temperature[i] = sensorsValue[currentIndex++];
    data.position[i] = sensorsValue[currentIndex++];
    data.stiffness[i] = sensorsValue[currentIndex++];
  }

  // copy the data for the fake joint RHipYawPitch
  data.electricCurrent[JointData::RHipYawPitch] = data.electricCurrent[JointData::LHipYawPitch];
  data.temperature[JointData::RHipYawPitch] = data.temperature[JointData::LHipYawPitch];
  data.position[JointData::RHipYawPitch] = data.position[JointData::LHipYawPitch];
  data.stiffness[JointData::RHipYawPitch] = data.stiffness[JointData::LHipYawPitch];

  data.timestamp = (unsigned int)(timeStamp - NaoTime::startingTimeInMilliSeconds);

}//end SensorJointData

void NaoSensorData::get(FSRData& data) const
{
  unsigned int currentIndex = theFSRDataIndex;
  for (int i = 0; i < FSRData::numOfFSR; i++) 
  {
    data.data[i] = sensorsValue[currentIndex++];
    // The value returned for each FSR is similar to Kg in NaoQi 1.3.17
    data.force[i] = data.data[i] * Math::g;
  }
}//end FSRData

void NaoSensorData::get(AccelerometerData& data) const
{
  //forum of Aldebaran scale = g/56.0
  //experiments Nao36 scale = g/60.0
  //wrong sign in Aldebaran
  static float scale_acc = Math::g/60.0;

  data.rawData.x = sensorsValue[theAccelerometerDataIndex + 0];
  data.rawData.y = sensorsValue[theAccelerometerDataIndex + 1];
  data.rawData.z = sensorsValue[theAccelerometerDataIndex + 2];

  //0.1532289 = 9.80665/64
  data.data = data.rawData * scale_acc;//* 0.1532289;
  //TODO: why?
  data.data.y *= -1; 
}//end AccelerometerData

void NaoSensorData::get(GyrometerData& data) const
{
  //data = (raw-zero) * 2.7 * PI/180 [rad/s]
  //static float scale_gyro = 2.7 * M_PI/180.0;
  const static double range = 4096; // 2^12
  const static double offset = range / 2;
  const static double scale_gyro = Math::fromDegrees(1000) / range; // +/- 500 deg/s

  data.rawData.x = sensorsValue[theGyrometerDataIndex + 0];
  data.rawData.y = sensorsValue[theGyrometerDataIndex + 1];
  data.ref = sensorsValue[theGyrometerDataIndex + 2];
  
  data.data.x = (data.rawData.x + offset)*scale_gyro;
  data.data.y = (data.rawData.y + offset)*scale_gyro;
}//end GyrometerData

void NaoSensorData::get(InertialSensorData& data) const
{
  data.data.x = sensorsValue[theInertialSensorDataIndex];
  data.data.y = sensorsValue[theInertialSensorDataIndex+1];
}//end InertialSensorData

void NaoSensorData::get(IRReceiveData& data) const
{
  unsigned int currentIndex = theIRReceiveDataIndex;
  for (int i = 0; i < IRReceiveData::numOfIRReceive; i++) {
    data.data[i] = sensorsValue[currentIndex++];
  }
}//end IRReceiveData

void NaoSensorData::get(ButtonData& data) const
{
  unsigned int currentIndex = theButtonDataIndex;
  for (int i = 0; i < ButtonData::numOfButtons; i++) 
  {
    float temp = sensorsValue[currentIndex++];
    bool wasAlreadPressed = data.isPressed[i];
    if (temp == 1.0f) 
    {
      data.isPressed[i] = true;
      data.numOfFramesPressed[i]++;
      if (!wasAlreadPressed) 
      {
        data.eventCounter[i]++;
      }
    } 
    else 
    {
      data.isPressed[i] = false;
      data.numOfFramesPressed[i] = 0;
    }
  }
}//end ButtonData

void NaoSensorData::get(UltraSoundReceiveData& data) const
{
  unsigned int currentIndex = theUltraSoundReceiveDataIndex;
  data.rawdata = sensorsValue[currentIndex++];

  for(int i = 0; i < UltraSoundData::numOfUSEcho;i++)
  {
    data.dataLeft[i] = sensorsValue[currentIndex++];
    data.dataRight[i] = sensorsValue[currentIndex++];
  }

}//end UltraSoundReceiveData

void NaoSensorData::get(BatteryData& data) const
{
  data.charge = sensorsValue[thBatteryDataIdex];
}//end BatteryData
