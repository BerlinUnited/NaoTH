/**
* @file LolaDataConverter.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Mellmann, Heinrich</a>
*
* Convert between the LolaData to Representaton types.
*
*/

#ifndef _LolaDataConverter_h_
#define _LolaDataConverter_h_


#include "LolaData.h"

#include "DCMData.h"

#include "Representations/Infrastructure/JointData.h"
#include "Representations/Infrastructure/UltraSoundData.h"
#include "Representations/Infrastructure/LEDData.h"
#include "Representations/Infrastructure/FSRData.h"
#include "Representations/Infrastructure/AccelerometerData.h"
#include "Representations/Infrastructure/GyrometerData.h"
#include "Representations/Infrastructure/InertialSensorData.h"
#include "Representations/Infrastructure/ButtonData.h"
#include "Representations/Infrastructure/BatteryData.h"

namespace naoth
{
  
constexpr std::array<int,25> lolaJointIdx 
{
  JointData::HeadYaw,
  JointData::HeadPitch,

  JointData::LShoulderPitch,
  JointData::LShoulderRoll,
  JointData::LElbowYaw,
  JointData::LElbowRoll,
  JointData::LWristYaw,

  JointData::LHipYawPitch,

  JointData::LHipRoll,
  JointData::LHipPitch,
  JointData::LKneePitch,
  JointData::LAnklePitch,
  JointData::LAnkleRoll,

  JointData::RHipRoll,
  JointData::RHipPitch,
  JointData::RKneePitch,
  JointData::RAnklePitch,
  JointData::RAnkleRoll,

  JointData::RShoulderPitch,
  JointData::RShoulderRoll,
  JointData::RElbowYaw,
  JointData::RElbowRoll,
  JointData::RWristYaw,

  JointData::LHand,
  JointData::RHand
};
  
class LolaDataConverter
{
public:

  static void set(ActuatorData& actuators, const MotorJointData& motorJointData) 
  {
    for(size_t i = 0; i < lolaJointIdx.size(); ++i) {
      actuators.Position[i]  = static_cast<float>(motorJointData.position[lolaJointIdx[i]]);
      actuators.Stiffness[i] = static_cast<float>(motorJointData.stiffness[lolaJointIdx[i]]);
    }
  }

  static void get(const SensorData& sensorData, SensorJointData& sensorJointData) 
  {
    for(size_t i = 0; i < lolaJointIdx.size(); ++i) 
    {
      sensorJointData.position[lolaJointIdx[i]]        = sensorData.Position[i];
      sensorJointData.stiffness[lolaJointIdx[i]]       = sensorData.Stiffness[i];
      sensorJointData.electricCurrent[lolaJointIdx[i]] = sensorData.Current[i];
      sensorJointData.temperature[lolaJointIdx[i]]     = sensorData.Temperature[i];
    }
  }

  static void set(ActuatorData& actuators, const LEDData& ledData) 
  {
    // REar
    for(size_t i = 0; i <= LEDData::EarRight324; ++i) {
      actuators.REar[i] = (float)ledData.theMonoLED[LEDData::EarRight324 - i];
    }

    // LEar
    for(size_t i = 0; i <= LEDData::EarRight324; ++i) {
      actuators.LEar[i] = (float)ledData.theMonoLED[LEDData::EarLeft0 + i];
    }

    // Chest
    actuators.Chest[0] = (float)ledData.theMultiLED[LEDData::ChestButton][LEDData::RED];
    actuators.Chest[1] = (float)ledData.theMultiLED[LEDData::ChestButton][LEDData::GREEN];
    actuators.Chest[2] = (float)ledData.theMultiLED[LEDData::ChestButton][LEDData::BLUE];

    // LEye
    {
      actuators.LEye[0] = static_cast<float>(ledData.theMultiLED[LEDData::MultiLEDID::FaceLeft45][LEDData::RED]);
      actuators.LEye[1] = static_cast<float>(ledData.theMultiLED[LEDData::MultiLEDID::FaceLeft0][LEDData::RED]);
      actuators.LEye[2] = static_cast<float>(ledData.theMultiLED[LEDData::MultiLEDID::FaceLeft315][LEDData::RED]);
      actuators.LEye[3] = static_cast<float>(ledData.theMultiLED[LEDData::MultiLEDID::FaceLeft270][LEDData::RED]);
      actuators.LEye[4] = static_cast<float>(ledData.theMultiLED[LEDData::MultiLEDID::FaceLeft225][LEDData::RED]);
      actuators.LEye[5] = static_cast<float>(ledData.theMultiLED[LEDData::MultiLEDID::FaceLeft180][LEDData::RED]);
      actuators.LEye[6] = static_cast<float>(ledData.theMultiLED[LEDData::MultiLEDID::FaceLeft135][LEDData::RED]);
      actuators.LEye[7] = static_cast<float>(ledData.theMultiLED[LEDData::MultiLEDID::FaceLeft90][LEDData::RED]);

      actuators.LEye[8] = static_cast<float>(ledData.theMultiLED[LEDData::MultiLEDID::FaceLeft45][LEDData::GREEN]);
      actuators.LEye[9] = static_cast<float>(ledData.theMultiLED[LEDData::MultiLEDID::FaceLeft0][LEDData::GREEN]);
      actuators.LEye[10] = static_cast<float>(ledData.theMultiLED[LEDData::MultiLEDID::FaceLeft315][LEDData::GREEN]);
      actuators.LEye[11] = static_cast<float>(ledData.theMultiLED[LEDData::MultiLEDID::FaceLeft270][LEDData::GREEN]);
      actuators.LEye[12] = static_cast<float>(ledData.theMultiLED[LEDData::MultiLEDID::FaceLeft225][LEDData::GREEN]);
      actuators.LEye[13] = static_cast<float>(ledData.theMultiLED[LEDData::MultiLEDID::FaceLeft180][LEDData::GREEN]);
      actuators.LEye[14] = static_cast<float>(ledData.theMultiLED[LEDData::MultiLEDID::FaceLeft135][LEDData::GREEN]);
      actuators.LEye[15] = static_cast<float>(ledData.theMultiLED[LEDData::MultiLEDID::FaceLeft90][LEDData::GREEN]);

      actuators.LEye[16] = static_cast<float>(ledData.theMultiLED[LEDData::MultiLEDID::FaceLeft45][LEDData::BLUE]);
      actuators.LEye[17] = static_cast<float>(ledData.theMultiLED[LEDData::MultiLEDID::FaceLeft0][LEDData::BLUE]);
      actuators.LEye[18] = static_cast<float>(ledData.theMultiLED[LEDData::MultiLEDID::FaceLeft315][LEDData::BLUE]);
      actuators.LEye[19] = static_cast<float>(ledData.theMultiLED[LEDData::MultiLEDID::FaceLeft270][LEDData::BLUE]);
      actuators.LEye[20] = static_cast<float>(ledData.theMultiLED[LEDData::MultiLEDID::FaceLeft225][LEDData::BLUE]);
      actuators.LEye[21] = static_cast<float>(ledData.theMultiLED[LEDData::MultiLEDID::FaceLeft180][LEDData::BLUE]);
      actuators.LEye[22] = static_cast<float>(ledData.theMultiLED[LEDData::MultiLEDID::FaceLeft135][LEDData::BLUE]);
      actuators.LEye[23] = static_cast<float>(ledData.theMultiLED[LEDData::MultiLEDID::FaceLeft90][LEDData::BLUE]);
    }

    // REye
    {
      actuators.LEye[0] = static_cast<float>(ledData.theMultiLED[LEDData::MultiLEDID::FaceLeft0][LEDData::RED]);
      actuators.LEye[1] = static_cast<float>(ledData.theMultiLED[LEDData::MultiLEDID::FaceLeft45][LEDData::RED]);
      actuators.LEye[2] = static_cast<float>(ledData.theMultiLED[LEDData::MultiLEDID::FaceLeft90][LEDData::RED]);
      actuators.LEye[3] = static_cast<float>(ledData.theMultiLED[LEDData::MultiLEDID::FaceRight135][LEDData::RED]);
      actuators.LEye[4] = static_cast<float>(ledData.theMultiLED[LEDData::MultiLEDID::FaceLeft180][LEDData::RED]);
      actuators.LEye[5] = static_cast<float>(ledData.theMultiLED[LEDData::MultiLEDID::FaceLeft225][LEDData::RED]);
      actuators.LEye[6] = static_cast<float>(ledData.theMultiLED[LEDData::MultiLEDID::FaceLeft270][LEDData::RED]);
      actuators.LEye[7] = static_cast<float>(ledData.theMultiLED[LEDData::MultiLEDID::FaceLeft315][LEDData::RED]);

      actuators.LEye[8] = static_cast<float>(ledData.theMultiLED[LEDData::MultiLEDID::FaceLeft0][LEDData::GREEN]);
      actuators.LEye[9] = static_cast<float>(ledData.theMultiLED[LEDData::MultiLEDID::FaceLeft45][LEDData::GREEN]);
      actuators.LEye[10] = static_cast<float>(ledData.theMultiLED[LEDData::MultiLEDID::FaceLeft90][LEDData::GREEN]);
      actuators.LEye[11] = static_cast<float>(ledData.theMultiLED[LEDData::MultiLEDID::FaceRight135][LEDData::GREEN]);
      actuators.LEye[12] = static_cast<float>(ledData.theMultiLED[LEDData::MultiLEDID::FaceLeft180][LEDData::GREEN]);
      actuators.LEye[13] = static_cast<float>(ledData.theMultiLED[LEDData::MultiLEDID::FaceLeft225][LEDData::GREEN]);
      actuators.LEye[14] = static_cast<float>(ledData.theMultiLED[LEDData::MultiLEDID::FaceLeft270][LEDData::GREEN]);
      actuators.LEye[15] = static_cast<float>(ledData.theMultiLED[LEDData::MultiLEDID::FaceLeft315][LEDData::GREEN]);

      actuators.LEye[16] = static_cast<float>(ledData.theMultiLED[LEDData::MultiLEDID::FaceLeft0][LEDData::BLUE]);
      actuators.LEye[17] = static_cast<float>(ledData.theMultiLED[LEDData::MultiLEDID::FaceLeft45][LEDData::BLUE]);
      actuators.LEye[18] = static_cast<float>(ledData.theMultiLED[LEDData::MultiLEDID::FaceLeft90][LEDData::BLUE]);
      actuators.LEye[19] = static_cast<float>(ledData.theMultiLED[LEDData::MultiLEDID::FaceRight135][LEDData::BLUE]);
      actuators.LEye[20] = static_cast<float>(ledData.theMultiLED[LEDData::MultiLEDID::FaceLeft180][LEDData::BLUE]);
      actuators.LEye[21] = static_cast<float>(ledData.theMultiLED[LEDData::MultiLEDID::FaceLeft225][LEDData::BLUE]);
      actuators.LEye[22] = static_cast<float>(ledData.theMultiLED[LEDData::MultiLEDID::FaceLeft270][LEDData::BLUE]);
      actuators.LEye[23] = static_cast<float>(ledData.theMultiLED[LEDData::MultiLEDID::FaceLeft315][LEDData::BLUE]);
    }

    // Skull
    actuators.Skull[ 0] = (float)ledData.theMonoLED[LEDData::HeadFrontLeft1];
    actuators.Skull[ 1] = (float)ledData.theMonoLED[LEDData::HeadFrontLeft0];
    actuators.Skull[ 2] = (float)ledData.theMonoLED[LEDData::HeadMiddleLeft0];
    actuators.Skull[ 3] = (float)ledData.theMonoLED[LEDData::HeadRearLeft0];
    actuators.Skull[ 4] = (float)ledData.theMonoLED[LEDData::HeadRearLeft1];
    actuators.Skull[ 5] = (float)ledData.theMonoLED[LEDData::HeadRearLeft2];
    actuators.Skull[ 6] = (float)ledData.theMonoLED[LEDData::HeadRearRight2];
    actuators.Skull[ 7] = (float)ledData.theMonoLED[LEDData::HeadRearRight1];
    actuators.Skull[ 8] = (float)ledData.theMonoLED[LEDData::HeadRearRight0];
    actuators.Skull[ 9] = (float)ledData.theMonoLED[LEDData::HeadMiddleRight0];
    actuators.Skull[10] = (float)ledData.theMonoLED[LEDData::HeadFrontRight0];
    actuators.Skull[11] = (float)ledData.theMonoLED[LEDData::HeadFrontRight1];

    // LFoot
    actuators.LFoot[0] = (float)ledData.theMultiLED[LEDData::FootLeft][LEDData::RED];
    actuators.LFoot[1] = (float)ledData.theMultiLED[LEDData::FootLeft][LEDData::GREEN];
    actuators.LFoot[2] = (float)ledData.theMultiLED[LEDData::FootLeft][LEDData::BLUE];

    // RFoot
    actuators.LFoot[0] = (float)ledData.theMultiLED[LEDData::FootRight][LEDData::RED];
    actuators.LFoot[1] = (float)ledData.theMultiLED[LEDData::FootRight][LEDData::GREEN];
    actuators.LFoot[2] = (float)ledData.theMultiLED[LEDData::FootRight][LEDData::BLUE];
  }

  // copy to DCMSensorData for compatibility
  static void readSensorData(const SensorData& sensorData, DCMSensorData& dcmSensorData) 
  {
    float* sensorsValue = dcmSensorData.sensorsValue;
    
    // SensorJointData
    for(size_t i = 0; i < lolaJointIdx.size(); ++i) 
    {
      //NOTE: ignore the JointData::RHipYawPitch
      size_t j = theSensorJointDataIndex + ((lolaJointIdx[i] >= JointData::RHipYawPitch)?lolaJointIdx[i]-1:lolaJointIdx[i])*4;
      sensorsValue[j  ] = sensorData.Current[i];
      sensorsValue[j+1] = sensorData.Temperature[i];
      sensorsValue[j+2] = sensorData.Position[i];
      sensorsValue[j+3] = sensorData.Stiffness[i];
    }

    
    { // FSRData
    unsigned int currentIndex = theFSRDataIndex;
    sensorsValue[currentIndex++] = sensorData.FSR.LFoot.FrontLeft;
    sensorsValue[currentIndex++] = sensorData.FSR.LFoot.FrontRight;
    sensorsValue[currentIndex++] = sensorData.FSR.LFoot.RearLeft;
    sensorsValue[currentIndex++] = sensorData.FSR.LFoot.RearRight;

    sensorsValue[currentIndex++] = sensorData.FSR.RFoot.FrontLeft;
    sensorsValue[currentIndex++] = sensorData.FSR.RFoot.FrontRight;
    sensorsValue[currentIndex++] = sensorData.FSR.RFoot.RearLeft;
    sensorsValue[currentIndex++] = sensorData.FSR.RFoot.RearRight;
    }

    { // AccelerometerData
    sensorsValue[theAccelerometerDataIndex + 0] = sensorData.Accelerometer.x;
    sensorsValue[theAccelerometerDataIndex + 1] = -sensorData.Accelerometer.y; // y-axis of v6 robots is mirrored compared to v5 and earlier
    sensorsValue[theAccelerometerDataIndex + 2] = sensorData.Accelerometer.z;

    sensorsValue[theAccelerometerDataIndex + 3] = sensorData.Accelerometer.x;
    sensorsValue[theAccelerometerDataIndex + 4] = -sensorData.Accelerometer.y; // y-axis of v6 robots is mirrored compared to v5 and earlier
    sensorsValue[theAccelerometerDataIndex + 5] = sensorData.Accelerometer.z;
    }

    { // GyrometerData
    sensorsValue[theGyrometerDataIndex + 0] = sensorData.Gyroscope.x;
    sensorsValue[theGyrometerDataIndex + 1] = sensorData.Gyroscope.y;
    sensorsValue[theGyrometerDataIndex + 2] = -sensorData.Gyroscope.z;// z-axis of v6 robots is mirrored compared to v5 and earlier

    sensorsValue[theGyrometerDataIndex + 3] = sensorData.Gyroscope.x;
    sensorsValue[theGyrometerDataIndex + 4] = sensorData.Gyroscope.y;
    sensorsValue[theGyrometerDataIndex + 5] = -sensorData.Gyroscope.z;// z-axis of v6 robots is mirrored compared to v5 and earlier

    //sensorsValue[theGyrometerDataIndex + 6] = 0;
    }

    { // InertialSensorData
    sensorsValue[theInertialSensorDataIndex    ] = sensorData.Angles.x;
    sensorsValue[theInertialSensorDataIndex + 1] = sensorData.Angles.y;
    }

    // IRReceiveData
    //for (int i = 0; i < IRReceiveData::numOfIRReceive; i++) {
    //  sensorsValue[theIRReceiveDataIndex + i] = 0;
    //}

    { // ButtonData
    unsigned int currentIndex = theButtonDataIndex;
    sensorsValue[currentIndex++] = sensorData.Touch.ChestBoard.Button;

    sensorsValue[currentIndex++] = sensorData.Touch.LFoot.Bumper.Left;
    sensorsValue[currentIndex++] = sensorData.Touch.LFoot.Bumper.Right;
    sensorsValue[currentIndex++] = sensorData.Touch.RFoot.Bumper.Left;
    sensorsValue[currentIndex++] = sensorData.Touch.RFoot.Bumper.Right;

    sensorsValue[currentIndex++] = sensorData.Touch.Head.Touch.Front;
    sensorsValue[currentIndex++] = sensorData.Touch.Head.Touch.Middle;
    sensorsValue[currentIndex++] = sensorData.Touch.Head.Touch.Rear;

    sensorsValue[currentIndex++] = sensorData.Touch.LHand.Touch.Back;
    sensorsValue[currentIndex++] = sensorData.Touch.LHand.Touch.Left;
    sensorsValue[currentIndex++] = sensorData.Touch.LHand.Touch.Right;

    sensorsValue[currentIndex++] = sensorData.Touch.RHand.Touch.Back;
    sensorsValue[currentIndex++] = sensorData.Touch.RHand.Touch.Left;
    sensorsValue[currentIndex++] = sensorData.Touch.RHand.Touch.Right;
    }

    { // UltraSoundReceiveData 
      unsigned int currentIndex = theUltraSoundReceiveDataIndex;
      sensorsValue[currentIndex++] = 0.0f;
      sensorsValue[currentIndex++] = sensorData.Sonar.Left;
      sensorsValue[currentIndex++] = sensorData.Sonar.Right;

      // set echos to zero because we dont have them on the NAO v6
      for(int i = 2; i < UltraSoundReceiveData::numOfUSEcho; i++)
      {
        sensorsValue[currentIndex++] = 0.0f;
        sensorsValue[currentIndex++] = 0.0f;
      }
    }

    { // BatteryData
    sensorsValue[theBatteryDataIndex    ] = sensorData.Battery.Charge;
    sensorsValue[theBatteryDataIndex + 1] = sensorData.Battery.Current;
    sensorsValue[theBatteryDataIndex + 2] = sensorData.Battery.Temperature;
    }
  }

}; // end class LolaDataConverter
} // end namespace naoth

#endif //_LolaDataConverter_h_
