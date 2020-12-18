/**
 * @file DCMData.h
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 * @breief Data communicated from DCM to NaoRobot
 *
 */

#include "DCMData.h"

#include <cstring>
#include "Tools/NaoTime.h"

using namespace naoth;

void DCMSensorData::get(SensorJointData& data) const
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

void DCMSensorData::get(FSRData& data) const
{
  unsigned int currentIndex = theFSRDataIndex;
  for (int i = 0; i < FSRData::numOfFSR; i++) {
    data.dataLeft[i] = sensorsValue[currentIndex++];
  }
  for (int i = 0; i < FSRData::numOfFSR; i++) {
    data.dataRight[i] = sensorsValue[currentIndex++];
  }
}

void DCMSensorData::get(AccelerometerData& data) const
{
  data.rawData.x = sensorsValue[theAccelerometerDataIndex + 0];
  data.rawData.y = sensorsValue[theAccelerometerDataIndex + 1];
  data.rawData.z = sensorsValue[theAccelerometerDataIndex + 2];

  data.data.x = sensorsValue[theAccelerometerDataIndex + 3];
  data.data.y = sensorsValue[theAccelerometerDataIndex + 4];
  data.data.z = sensorsValue[theAccelerometerDataIndex + 5];

  //forum of Aldebaran scale = g/56.0
  //experiments Nao36 scale = g/60.0
  //wrong sign in Aldebaran
  //static float scale_acc = static_cast<float>(Math::g/60.0);
  //0.1532289 = 9.80665/64
  //data.data = data.rawData * scale_acc;//* 0.1532289;

  // measure acceleration relative to a falling body or measure the acceleration of the robots torso
  // e.g. z"=0    => robot is falling
  //      z"=9.81 => robot is at rest relative to the earth or accelerated opposite to the gravitational force relative to a falling body
  // y axis seems to be in other direction... how? it's an integrated circuit 3d accelerometer...
  // possible explanation:
  // the sensor's x axis shows to the head, the z axis infront of the robot and y axis to the right arm
  // now aldebaran switches the "naming"/interpretation of x and z axis (so z becomes their x and x becomes their z)
  // we now have some kind of left handed coordinate system
  // to get a right handed coordinate system back we have to negate y in respect to x and z

  data.data.x *= -1;
  //data.data.y *= 1;
  data.data.z *= -1;
}

void DCMSensorData::get(GyrometerData& data) const
{
  data.rawData.x = sensorsValue[theGyrometerDataIndex + 0];
  data.rawData.y = sensorsValue[theGyrometerDataIndex + 1];
  data.rawData.z = sensorsValue[theGyrometerDataIndex + 2];

  data.data.x = sensorsValue[theGyrometerDataIndex + 3];
  data.data.y = sensorsValue[theGyrometerDataIndex + 4];
  data.data.z = sensorsValue[theGyrometerDataIndex + 5];

  data.ref = sensorsValue[theGyrometerDataIndex + 6];

  // if the robot is turned to left around its z axis the measured angular velocity is negative but it should be positiv
  // this problem is related to that mentioned above in DCMSensorData::get(AccelerometerData& data)
  // to fix this the values for z have to be negated
  data.data.z *= -1;
  data.rawData.z *= -1;

  //data = (raw-zero) * 2.7 * PI/180 [rad/s]
  //static float scale_gyro = 2.7 * M_PI/180.0;
  /*
  const static double range = 4096; // 2^12
  const static double offset = range / 2;
  const static double scale_gyro = Math::fromDegrees(1000) / range; // +/- 500 deg/s

  data.data.x = (data.rawData.x + offset)*scale_gyro;
  data.data.y = (data.rawData.y + offset)*scale_gyro;
  data.data.z = (data.rawData.z + offset)*scale_gyro;
  */
}

void DCMSensorData::get(InertialSensorData& data) const
{
  data.data.x = sensorsValue[theInertialSensorDataIndex];
  data.data.y = sensorsValue[theInertialSensorDataIndex+1];
}

// HACK: this method applies iterative updates, so the result depends on the last state (!)
void DCMSensorData::get(ButtonData& data) const
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
}

void DCMSensorData::get(UltraSoundReceiveData& data) const
{
  unsigned int currentIndex = theUltraSoundReceiveDataIndex;
  data.rawdata = sensorsValue[currentIndex++];

  for(int i = 0; i < UltraSoundReceiveData::numOfUSEcho;i++)
  {
    data.dataLeft[i] = sensorsValue[currentIndex++];
    data.dataRight[i] = sensorsValue[currentIndex++];
  }
}

void DCMSensorData::get(BatteryData& data) const
{
  unsigned int currentIndex = theBatteryDataIdex;
  data.charge = sensorsValue[currentIndex++];
  data.current = sensorsValue[currentIndex++];
  data.temperature = sensorsValue[currentIndex++];
}
