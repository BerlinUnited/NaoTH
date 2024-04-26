/**
* @file LolaDataConverter.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Mellmann, Heinrich</a>
*
* Convert between the LolaData to Representaton types.
*
*/

#ifndef LOLA_DATA_CONVERTER_H
#define LOLA_DATA_CONVERTER_H


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

// Joint ID order in a lola package.
enum LolaJointID {
  HeadYaw,
  HeadPitch,

  LShoulderPitch,
  LShoulderRoll,
  LElbowYaw,
  LElbowRoll,
  LWristYaw,

  LHipYawPitch,

  LHipRoll,
  LHipPitch,
  LKneePitch,
  LAnklePitch,
  LAnkleRoll,

  RHipRoll,
  RHipPitch,
  RKneePitch,
  RAnklePitch,
  RAnkleRoll,

  RShoulderPitch,
  RShoulderRoll,
  RElbowYaw,
  RElbowRoll,
  RWristYaw,

  LHand,
  RHand
};

// Map the order of joints in Lola to the joint IDs in JointData.
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


// Map the order of joints in JointData to the joint IDs in Lola.
constexpr std::array<int,25> jointMap_Naoth2Lola
{
  LolaJointID::HeadPitch,
  LolaJointID::HeadYaw,

  LolaJointID::RShoulderRoll,
  LolaJointID::LShoulderRoll,
  LolaJointID::RShoulderPitch,
  LolaJointID::LShoulderPitch,

  LolaJointID::RElbowRoll,
  LolaJointID::LElbowRoll,
  LolaJointID::RElbowYaw,
  LolaJointID::LElbowYaw,

  // Lola does not list this joint separately and DCMData as well.
  //LolaJointID::LHipYawPitch, // JointData::RHipYawPitch, same as LHipYawPitch.
  LolaJointID::LHipYawPitch,
  
  LolaJointID::RHipPitch,
  LolaJointID::LHipPitch,
  LolaJointID::RHipRoll,
  LolaJointID::LHipRoll,
  LolaJointID::RKneePitch,
  LolaJointID::LKneePitch,
  LolaJointID::RAnklePitch,
  LolaJointID::LAnklePitch,
  LolaJointID::RAnkleRoll,
  LolaJointID::LAnkleRoll,

  // NOTE: those values don't exist on the old V3.2/V3.3 robots
  //       so, we put them at the end for easier support for the old format
  LolaJointID::LWristYaw,
  LolaJointID::RWristYaw,
  LolaJointID::LHand,
  LolaJointID::RHand,
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
    // index used to iterate over appays in actuators
    int idx = 0;
    
    // REar
    idx = 0;
    for(int i = LEDData::EarRight0; i <= LEDData::EarRight324; ++i) {
      actuators.REar[idx++] = static_cast<float>(ledData.theMonoLED[LEDData::EarRight324 - i]);
    }

    // LEar
    idx = 0;
    for(int i = LEDData::EarLeft0; i <= LEDData::EarLeft324; ++i) {
      actuators.LEar[idx++] = static_cast<float>(ledData.theMonoLED[i]);
    }

    // Chest
    actuators.Chest[0] = static_cast<float>(ledData.theMultiLED[LEDData::ChestButton][LEDData::RED]);
    actuators.Chest[1] = static_cast<float>(ledData.theMultiLED[LEDData::ChestButton][LEDData::GREEN]);
    actuators.Chest[2] = static_cast<float>(ledData.theMultiLED[LEDData::ChestButton][LEDData::BLUE]);

    // REye
    idx = 0;
    for(int color = LEDData::RED; color < LEDData::numOfLEDColor; ++color) {
      for(int led = LEDData::FaceRight0; led <= LEDData::FaceRight315; ++led) {
        actuators.REye[idx++] = static_cast<float>(ledData.theMultiLED[LEDData::FaceRight315 - led][color]);
      }
    }
    
    // LEye
    idx = 0;
    for(int color = LEDData::RED; color < LEDData::numOfLEDColor; ++color) {
      for(int led = LEDData::FaceLeft0; led <= LEDData::FaceLeft315; ++led) {
        actuators.LEye[idx++] = static_cast<float>(ledData.theMultiLED[led][color]);
      }
    }

    // Skull
    actuators.Skull[ 0] = static_cast<float>(ledData.theMonoLED[LEDData::HeadFrontLeft1]);
    actuators.Skull[ 1] = static_cast<float>(ledData.theMonoLED[LEDData::HeadFrontLeft0]);
    actuators.Skull[ 2] = static_cast<float>(ledData.theMonoLED[LEDData::HeadMiddleLeft0]);
    actuators.Skull[ 3] = static_cast<float>(ledData.theMonoLED[LEDData::HeadRearLeft0]);
    actuators.Skull[ 4] = static_cast<float>(ledData.theMonoLED[LEDData::HeadRearLeft1]);
    actuators.Skull[ 5] = static_cast<float>(ledData.theMonoLED[LEDData::HeadRearLeft2]);
    actuators.Skull[ 6] = static_cast<float>(ledData.theMonoLED[LEDData::HeadRearRight2]);
    actuators.Skull[ 7] = static_cast<float>(ledData.theMonoLED[LEDData::HeadRearRight1]);
    actuators.Skull[ 8] = static_cast<float>(ledData.theMonoLED[LEDData::HeadRearRight0]);
    actuators.Skull[ 9] = static_cast<float>(ledData.theMonoLED[LEDData::HeadMiddleRight0]);
    actuators.Skull[10] = static_cast<float>(ledData.theMonoLED[LEDData::HeadFrontRight0]);
    actuators.Skull[11] = static_cast<float>(ledData.theMonoLED[LEDData::HeadFrontRight1]);

    // LFoot
    actuators.LFoot[0] = static_cast<float>(ledData.theMultiLED[LEDData::FootLeft][LEDData::RED]);
    actuators.LFoot[1] = static_cast<float>(ledData.theMultiLED[LEDData::FootLeft][LEDData::GREEN]);
    actuators.LFoot[2] = static_cast<float>(ledData.theMultiLED[LEDData::FootLeft][LEDData::BLUE]);

    // RFoot
    actuators.RFoot[0] = static_cast<float>(ledData.theMultiLED[LEDData::FootRight][LEDData::RED]);
    actuators.RFoot[1] = static_cast<float>(ledData.theMultiLED[LEDData::FootRight][LEDData::GREEN]);
    actuators.RFoot[2] = static_cast<float>(ledData.theMultiLED[LEDData::FootRight][LEDData::BLUE]);
  }

  // copy to DCMSensorData for compatibility
  static void readSensorData(const SensorData& sensorData, DCMSensorData& dcmSensorData) 
  {
    float* sensorsValue = dcmSensorData.sensorsValue;
    
    { // SensorJointData
      unsigned int currentIndex = theSensorJointDataIndex;
      for(size_t i = 0; i < jointMap_Naoth2Lola.size(); ++i) 
      {
        //NOTE: ignore the JointData::RHipYawPitch
        //const size_t j = theSensorJointDataIndex + ((lolaJointIdx[i] >= JointData::RHipYawPitch)?lolaJointIdx[i]-1:lolaJointIdx[i])*4;
        
        const size_t lolaJointId = jointMap_Naoth2Lola[i];
        
        sensorsValue[currentIndex++] = sensorData.Current[lolaJointId];
        sensorsValue[currentIndex++] = sensorData.Temperature[lolaJointId];
        sensorsValue[currentIndex++] = sensorData.Position[lolaJointId];
        sensorsValue[currentIndex++] = sensorData.Stiffness[lolaJointId];
      }
      //assert(currentIndex == theFSRDataIndex);
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
    sensorsValue[theAccelerometerDataIndex + 0] =  sensorData.Accelerometer.x;
    sensorsValue[theAccelerometerDataIndex + 1] = -sensorData.Accelerometer.y; // y-axis of v6 robots is mirrored compared to v5 and earlier
    sensorsValue[theAccelerometerDataIndex + 2] =  sensorData.Accelerometer.z;

    sensorsValue[theAccelerometerDataIndex + 3] =  sensorData.Accelerometer.x;
    sensorsValue[theAccelerometerDataIndex + 4] = -sensorData.Accelerometer.y; // y-axis of v6 robots is mirrored compared to v5 and earlier
    sensorsValue[theAccelerometerDataIndex + 5] =  sensorData.Accelerometer.z;
    }

    { // GyrometerData
    sensorsValue[theGyrometerDataIndex + 0] =  sensorData.Gyroscope.x;
    sensorsValue[theGyrometerDataIndex + 1] =  sensorData.Gyroscope.y;
    sensorsValue[theGyrometerDataIndex + 2] = -sensorData.Gyroscope.z;// z-axis of v6 robots is mirrored compared to v5 and earlier

    sensorsValue[theGyrometerDataIndex + 3] =  sensorData.Gyroscope.x;
    sensorsValue[theGyrometerDataIndex + 4] =  sensorData.Gyroscope.y;
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

#endif //LOLA_DATA_CONVERTER_H
