/**
 * @file IPCData.h
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Mellmann, Heinrich</a>
 * @breief Inter-process communication data
 *
 */
 
#ifndef _IPCData_H_
#define _IPCData_H_

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
const unsigned int theFSRDataIndex = theSensorJointDataIndex + 4 * (JointData::numOfJoint - 1); // RHipYawPitch and LHipYawPitch are the same joint
const unsigned int theAccelerometerDataIndex = theFSRDataIndex + FSRData::numOfFSR;
const unsigned int theGyrometerDataIndex = theAccelerometerDataIndex + 3; // 3 Axes acc
const unsigned int theInertialSensorDataIndex = theGyrometerDataIndex + 2 + 1; // 2 axes + ref value gyro
const unsigned int theIRReceiveDataIndex = theInertialSensorDataIndex + 2; // 2 axes inertial
const unsigned int theButtonDataIndex = theIRReceiveDataIndex + IRReceiveData::numOfIRReceive;
const unsigned int theUltraSoundReceiveDataIndex = theButtonDataIndex + ButtonData::numOfButtons;
const unsigned int thBatteryDataIdex = theUltraSoundReceiveDataIndex + 1 + 2 * UltraSoundData::numOfUSEcho; // 
const unsigned int numOfSensors = thBatteryDataIdex + 1;

/** 
* data written by libnaoth
*/
class NaoSensorData
{
public:
  unsigned long long timeStamp;
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

}// end namespace naoth

#endif // _IPCData_H_
