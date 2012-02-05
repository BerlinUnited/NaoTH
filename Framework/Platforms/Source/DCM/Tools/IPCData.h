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
  const unsigned int theGyrometerDataIndex = theAccelerometerDataIndex + 3; // 3 Axes
  const unsigned int theInertialSensorDataIndex = theGyrometerDataIndex + 2 + 1; // 2 axes + ref value
  const unsigned int theIRReceiveDataIndex = theInertialSensorDataIndex + 2; // 2 axes
  const unsigned int theButtonDataIndex = theIRReceiveDataIndex + IRReceiveData::numOfIRReceive;
  const unsigned int theUltraSoundReceiveDataIndex = theButtonDataIndex + ButtonData::numOfButtons;
  const unsigned int thBatteryDataIdex = theUltraSoundReceiveDataIndex + 1 + 2 * UltraSoundData::numOfIRSend;
  const unsigned int numOfSensors = thBatteryDataIdex + 1;
  
  // data written by libnaoth
  class NaoSensorData
  {
  public:
    unsigned int timeStamp;
    float sensorsValue[numOfSensors];
    
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
  

  template<class T>
  class Accessor
  {
  public:
    inline void set(const T& other) { data = other; }
    inline const T& get() const { return data; }
  private:
    T data;
  };

  /*
  // data written by naoth
  class NaoCommandData
  {
  public:
    void set(const LEDData& data) { theLEDData = data; }
    void set(const IRSendData& data) { theIRSendData = data; }
    void set(const UltraSoundSendData& data) { theUltraSoundSendData = data; }
    void set(const MotorJointData& data){ theMotorJointData = data; }
    
    const LEDData& lEDData() const { return theLEDData; }
    const IRSendData& iRSendData() const { return theIRSendData; }
    const UltraSoundSendData& ultraSoundSendData() const { return theUltraSoundSendData; }
    const MotorJointData& motorJointData() const { return theMotorJointData; }

  private:
    LEDData theLEDData;
    IRSendData theIRSendData;
    UltraSoundSendData theUltraSoundSendData;
    MotorJointData theMotorJointData;
  };
  */
}

#endif // _IPC_DATA_H_
