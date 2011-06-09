/**
 * @file IPCData.h
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 * @breief Inter-process communication data
 *
 */
 
#ifndef _IPC_DATA_H_
#define _IPC_DATA_H_

#include "Representations/Infrastructure/JointData.h"
#include "Representations/Infrastructure/IRData.h"
#include "Representations/Infrastructure/UltraSoundData.h"
#include "Representations/Infrastructure/LEDData.h"

namespace naoth
{
  const int numOfSensors = 4 * (JointData::numOfJoint - 1)
                  + FSRData::numOfFSR
                  + AccelerometerData::numOfAccelerometer
                  + GyrometerData::numOfGyrometer + 1
                  + InertialSensorData::numOfInertialSensor
                  + IRReceiveData::numOfIRReceive
                  + ButtonData::numOfButtons
                  + 1 + 2 * UltraSoundData::numOfIRSend//3 // ultrasound
                  + 1; // BatteryCharge
  
  // data written by libnaoth
  struct LibNaothData
  {
    float currentAllSensorsValue[numOfSensors];
    MotorJointData theMotorJointData;
  };
  
  // data written by naoth
  struct NaothData
  {
    LEDData theLEDData;
    IRSendData theIRSendData;
    UltraSoundSendData theUltraSoundSendData;
  };
}

#endif // _IPC_DATA_H_