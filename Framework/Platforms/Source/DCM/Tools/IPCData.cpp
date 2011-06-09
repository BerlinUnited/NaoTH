/**
 * @file IPCData.h
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 * @breief Inter-process communication data
 *
 */

#include "IPCData.h"

using namespace naoth;

string LibNaothData::getBodyID() const
{
  return string(bodyID);
}

string LibNaothData::getNickName() const
{
  return string(nickName);
}

void LibNaothData::get(MotorJointData& data) const
{
  data = theMotorJointData;
}

void LibNaothData::get(SensorJointData& data) const
{
  unsigned int currentIndex = theSensorJointDataIndex;
    for (int i = 0; i < JointData::RHipYawPitch; i++) {
      data.electricCurrent[i] = sensorsValue[currentIndex++];
      data.temperature[i] = sensorsValue[currentIndex++];
      data.position[i] = sensorsValue[currentIndex++];
      data.stiffness[i] = sensorsValue[currentIndex++];
    }
    for (int i = JointData::RHipYawPitch + 1; i < JointData::numOfJoint; i++) {
      data.electricCurrent[i] = sensorsValue[currentIndex++];
      data.temperature[i] = sensorsValue[currentIndex++];
      data.position[i] = sensorsValue[currentIndex++];
      data.stiffness[i] = sensorsValue[currentIndex++];
    }
    data.electricCurrent[JointData::RHipYawPitch] = data.electricCurrent[JointData::LHipYawPitch];
    data.temperature[JointData::RHipYawPitch] = data.temperature[JointData::LHipYawPitch];
    data.position[JointData::RHipYawPitch] = data.position[JointData::LHipYawPitch];
    data.stiffness[JointData::RHipYawPitch] = data.stiffness[JointData::LHipYawPitch];
}

void LibNaothData::get(FSRData& data) const
{
  unsigned int currentIndex = theFSRDataIndex;
    for (int i = 0; i < FSRData::numOfFSR; i++) {
      data.data[i] = sensorsValue[currentIndex++];
      data.force[i] = data.data[i] * 9.81; // The value returned for each FSR is similar to Kg in NaoQi 1.3.17
    }
}

void LibNaothData::get(AccelerometerData& data) const
{
  //forum of Aldebaran scale = g/56.0
    //experiments Nao36 scale = g/60.0
    //wrong sign in Aldebaran
    static float scale_acc = -9.81/60.0;

    unsigned int currentIndex = theAccelerometerDataIndex;
    for (int i = 0; i < AccelerometerData::numOfAccelerometer; i++) {
      data.rawData[i] = sensorsValue[currentIndex++];
      //0.1532289 = 9.80665/64
      data.data[i] = data.rawData[i] * scale_acc;//* 0.1532289;
    }
}

void LibNaothData::get(GyrometerData& data) const
{
  //data = (raw-zero) * 2.7 * PI/180 [rad/s]
    static float scale_gyro = 2.7 * M_PI/180.0;

    unsigned int currentIndex = theGyrometerDataIndex;
    for (int i = 0; i < GyrometerData::numOfGyrometer + 1; i++) {
      data.rawData[i] = sensorsValue[currentIndex++];
    }
    for (int i = 0; i < GyrometerData::numOfGyrometer; i++) {
      data.data[i] = (data.rawData[i] - data.rawData[2]) * scale_gyro;
    }
}

void LibNaothData::get(InertialSensorData& data) const
{
  unsigned int currentIndex = theInertialSensorDataIndex;
    for (int i = 0; i < InertialSensorData::numOfInertialSensor; i++)
    {
      data.data[i] = sensorsValue[currentIndex++] + data.offset[i];
    }
}

void LibNaothData::get(IRReceiveData& data) const
{
  unsigned int currentIndex = theIRReceiveDataIndex;
    for (int i = 0; i < IRReceiveData::numOfIRReceive; i++) {
      data.data[i] = sensorsValue[currentIndex++];
    }
}

void LibNaothData::get(ButtonData& data) const
{
  unsigned int currentIndex = theButtonDataIndex;
    for (int i = 0; i < ButtonData::numOfButtons; i++) {
      float temp = sensorsValue[currentIndex++];
      bool wasAlreadPressed = data.isPressed[i];
      if (temp == 1.0f) {
        data.isPressed[i] = true;
        data.numOfFramesPressed[i]++;
        if (!wasAlreadPressed) {
          data.eventCounter[i]++;
        }
      } else {
        data.isPressed[i] = false;
        data.numOfFramesPressed[i] = 0;
      }
    }
}

void LibNaothData::get(UltraSoundReceiveData& data) const
{
  unsigned int currentIndex = theUltraSoundReceiveDataIndex;
    if(data.ultraSoundTimeStep != 100) //Hack:is only 100 if mode 4 or 12 etc were the third bit is set
    {
      data.rawdata = sensorsValue[currentIndex++];
      currentIndex += UltraSoundData::numOfIRSend * 2;
    }
    else
    {
      currentIndex++;
      for(int i = 0; i < UltraSoundData::numOfIRSend;i++)
      {
        data.dataLeft[i] = sensorsValue[currentIndex++];
        data.dataRight[i] = sensorsValue[currentIndex++];
      }
    }
}

void LibNaothData::get(BatteryData& data) const
{
  data.charge = sensorsValue[thBatteryDataIdex];
}
