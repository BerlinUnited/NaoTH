/**
 * @file DCMData.h
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Mellmann, Heinrich</a>
 *
 * Sensor data obtained from the DCM (Device Communication Manager).
 * The data is read from the ALMemory as a block into an array of float-values and can be unpacked 
 * into separate representations by `get()` methods.
 * Used for communicating sensor data NaoSMAL to the module architecture (NaoRobot) on NAO versions <= V5.
 *
 */

#ifndef DCMData_H
#define DCMData_H

#include "Representations/Infrastructure/JointData.h"
#include "Representations/Infrastructure/UltraSoundData.h"
#include "Representations/Infrastructure/LEDData.h"
#include "Representations/Infrastructure/FSRData.h"
#include "Representations/Infrastructure/AccelerometerData.h"
#include "Representations/Infrastructure/GyrometerData.h"
#include "Representations/Infrastructure/InertialSensorData.h"
#include "Representations/Infrastructure/ButtonData.h"
#include "Representations/Infrastructure/BatteryData.h"

#include <algorithm>

namespace naoth
{

const unsigned int theSensorJointDataIndex       = 0;
const unsigned int theFSRDataIndex               = theSensorJointDataIndex       + 4 * (JointData::numOfJoint - 1); // RHipYawPitch and LHipYawPitch are the same joint
const unsigned int theAccelerometerDataIndex     = theFSRDataIndex               + 2*FSRData::numOfFSR; // Left + Right
const unsigned int theGyrometerDataIndex         = theAccelerometerDataIndex     + 3 + 3; // acc: 3 axes raw + 2 axes m.s-2
const unsigned int theInertialSensorDataIndex    = theGyrometerDataIndex         + 3 + 3 + 1; // gyro: 3 axes raw + 3 axes rad-s + ref value
const unsigned int theButtonDataIndex            = theInertialSensorDataIndex    + 2; // inertial: 2 axes
const unsigned int theUltraSoundReceiveDataIndex = theButtonDataIndex            + ButtonData::numOfButtons;
const unsigned int theBatteryDataIdex            = theUltraSoundReceiveDataIndex + 1 + 2 * UltraSoundReceiveData::numOfUSEcho; //
const unsigned int numOfSensors                  = theBatteryDataIdex            + 3; // charge, current, temperature

/**
* Data written by NaoSMAL and read by NaoRobot.
*/
class DCMSensorData
{
public:
  DCMSensorData() {
    std::fill_n(sensorsValue, numOfSensors, 0.0f);
  }

  // TODO: maybe use some time_t type?
  unsigned long long timeStamp;
  
  // memory block holding all sensory data. This is written by DCM.
  float sensorsValue[numOfSensors];

  // methods to unpack data to representations
  // NOTE: methos can perform some data conversion, not simply copy (!)
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

#endif // DCMData_H
