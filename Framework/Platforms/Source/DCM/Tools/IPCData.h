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
#include "Representations/Infrastructure/UltraSoundData.h"
#include "Representations/Infrastructure/LEDData.h"
#include "Representations/Infrastructure/FSRData.h"
#include "Representations/Infrastructure/AccelerometerData.h"
#include "Representations/Infrastructure/GyrometerData.h"
#include "Representations/Infrastructure/InertialSensorData.h"
#include "Representations/Infrastructure/ButtonData.h"
#include "Representations/Infrastructure/BatteryData.h"
#include "Representations/Infrastructure/CpuData.h"

namespace naoth
{

const unsigned int theSensorJointDataIndex = 0;
const unsigned int theFSRDataIndex = theSensorJointDataIndex + 4 * (JointData::numOfJoint - 1); // RHipYawPitch and LHipYawPitch are the same joint
const unsigned int theAccelerometerDataIndex = theFSRDataIndex + 2*FSRData::numOfFSR; // Left + Right
const unsigned int theGyrometerDataIndex = theAccelerometerDataIndex + 3 + 3; // acc: 3 axes raw + 2 axes m.s-2
const unsigned int theInertialSensorDataIndex = theGyrometerDataIndex + 3 + 3 + 1; // gyro: 3 axes raw + 3 axes rad-s + ref value
const unsigned int theButtonDataIndex = theInertialSensorDataIndex + 2; // inertial: 2 axes
const unsigned int theUltraSoundReceiveDataIndex = theButtonDataIndex + ButtonData::numOfButtons;
const unsigned int theBatteryDataIdex = theUltraSoundReceiveDataIndex + 1 + 2 * UltraSoundReceiveData::numOfUSEcho; //
const unsigned int numOfSensors = theBatteryDataIdex + 3; // charge, current, temperature

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
  void get(ButtonData& data) const;
  void get(BatteryData& data) const;
  void get(UltraSoundReceiveData& data) const;
};

}// end namespace naoth

#endif // _IPCData_H_
