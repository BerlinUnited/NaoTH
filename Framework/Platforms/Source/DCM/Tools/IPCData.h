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
#include "Representations/Infrastructure/FSRData.h"
#include "Representations/Infrastructure/AccelerometerData.h"
#include "Representations/Infrastructure/GyrometerData.h"
#include "Representations/Infrastructure/InertialSensorData.h"
#include "Representations/Infrastructure/ButtonData.h"
#include "Representations/Infrastructure/BatteryData.h"

namespace naoth
{
  const unsigned int theSensorJointDataIndex = 0;
  const unsigned int theFSRDataIndex = theSensorJointDataIndex + 4 * (JointData::numOfJoint - 1);
  const unsigned int theAccelerometerDataIndex = theFSRDataIndex + FSRData::numOfFSR;
  const unsigned int theGyrometerDataIndex = theAccelerometerDataIndex + AccelerometerData::numOfAccelerometer;
  const unsigned int theInertialSensorDataIndex = theGyrometerDataIndex + GyrometerData::numOfGyrometer + 1;
  const unsigned int theIRReceiveDataIndex = theInertialSensorDataIndex + InertialSensorData::numOfInertialSensor;
  const unsigned int theButtonDataIndex = theIRReceiveDataIndex + IRReceiveData::numOfIRReceive;
  const unsigned int theUltraSoundReceiveDataIndex = theButtonDataIndex + ButtonData::numOfButtons;
  const unsigned int thBatteryDataIdex = theUltraSoundReceiveDataIndex + 1 + 2 * UltraSoundData::numOfIRSend;
  const unsigned int numOfSensors = thBatteryDataIdex + 1;
  
  // data written by libnaoth
  class LibNaothData
  {
  public:
    char bodyID[24];
    char nickName[24];
    unsigned int timeStamp;
    float sensorsValue[numOfSensors];
    MotorJointData theMotorJointData;
    
    string getBodyID() const;
    string getNickName() const;
    void get(MotorJointData& data) const;
    void get(SensorJointData& data) const;
    void get(FSRData& data) const;
    void get(AccelerometerData& data) const;
    void get(GyrometerData& data) const;
    void get(InertialSensorData& data) const;
    void get(IRReceiveData& data) const;
    void get(ButtonData& data) const;
    void get(UltraSoundReceiveData& data) const;
    void get(BatteryData& data) const;
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