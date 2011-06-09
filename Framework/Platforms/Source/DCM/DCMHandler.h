 /* 
 * File:   DCMHandler.h
 * Author: Oliver Welter (welter@informatik.hu-berlin.de)
 *
 * Created on 14. Juli 2008, 13:16
 */

#ifndef _DCMHANDLER_H
#define	_DCMHANDLER_H

#include <string>
#include <sstream>
#include <iostream>

#include <alcore/alptr.h>
#include <alcommon/albroker.h>
#include <alvalue/alvalue.h>
#include <alcommon/alproxy.h>
#include <almemoryfastaccess/almemoryfastaccess.h>
#include <alproxies/almemoryproxy.h>
#include <alproxies/alledsproxy.h>
#include <alproxies/dcmproxy.h>

#include "Representations/Infrastructure/JointData.h"
#include "Representations/Infrastructure/FSRData.h"
#include "Representations/Infrastructure/LEDData.h"
#include "Representations/Infrastructure/AccelerometerData.h"
#include "Representations/Infrastructure/GyrometerData.h"
#include "Representations/Infrastructure/InertialSensorData.h"
#include "Representations/Infrastructure/ButtonData.h"
#include "Representations/Infrastructure/IRData.h"
#include "Representations/Infrastructure/UltraSoundData.h"
#include "Representations/Infrastructure/BatteryData.h"

using namespace AL;
using namespace std;

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
                  
class DCMHandler
{
  private:
    ALPtr<ALBroker> pBroker;
    ALPtr<ALMemoryProxy> al_memory;
    ALMemoryFastAccess al_memoryfast;
    DCMProxy * al_dcmproxy;
    
    //Joints
    string DCMPath_MotorJointHardness[JointData::numOfJoint];
    string DCMPath_MotorJointPosition[JointData::numOfJoint];
    string DCMPath_SensorJointPosition[JointData::numOfJoint];
    string DCMPath_SensorJointElectricCurrent[JointData::numOfJoint];
    string DCMPath_SensorJointTemperature[JointData::numOfJoint];
    

    //LED
    string DCMPath_MonoLED[LEDData::numOfMonoLED];
    string DCMPath_MultiLED[LEDData::numOfMultiLED][LEDData::numOfLEDColor];

    //FSR
    string DCMPath_FSR[FSRData::numOfFSR];

    //Accelerometer
    string DCMPath_Accelerometer[AccelerometerData::numOfAccelerometer];

    //Gyrometer
    string DCMPath_Gyrometer[GyrometerData::numOfGyrometer+1];

    //Inertial Sensors
    string DCMPath_InertialSensor[InertialSensorData::numOfInertialSensor];

    //Buttons
    string DCMPath_Button[ButtonData::numOfButtons];

    //IR
    string DCMPath_IRSend[IRSendData::numOfIRSend];
    string DCMPath_IRReceive[IRReceiveData::numOfIRReceive];
    
    //UltraSound
    string DCMPath_UltraSoundReceive;
    string DCMPath_UltraSoundReceiveLeft[UltraSoundData::numOfIRSend];
    string DCMPath_UltraSoundReceiveRight[UltraSoundData::numOfIRSend];
    string DCMPath_UltraSoundSend;


    //Body-ID
    string DCMPath_BodyId;
    string DCMPath_BodyNickName;

    //Battery
    string DCMPath_BatteryCharge;

     //DCMCommand-Structures
    ALValue allMotorPositionCommands;
    ALValue allMotorHardnessCommands;
    ALValue ledCommands;
    ALValue irCommands;
    ALValue usSendCommands;

    string allSensorsList[numOfSensors];
    float* sensorPtrs[numOfSensors];
    float currentAllSensorsValue[numOfSensors];
    
    // Actuators data
    bool theMotorJointDataRequested;
    MotorJointData theMotorJointData;
    bool theLEDDataRequested;
    LEDData theLEDData;
    bool theIRSendDataRequested;
    IRSendData theIRSendData;
    bool theUltraSoundSendDataRequested;
    UltraSoundSendData theUltraSoundSendData;
    
    unsigned int currentTimestamp;

    // index
    unsigned int theSensorJointDataIndex;
    unsigned int theFSRDataIndex;
    unsigned int theAccelerometerDataIndex;
    unsigned int theGyrometerDataIndex;
    unsigned int theInertialSensorDataIndex;
    unsigned int theIRReceiveDataIndex;
    unsigned int theButtonDataIndex;
    unsigned int theUltraSoundReceiveDataIndex;
    unsigned int thBatteryDataIdex;


  ALValue getFromALMemory(const string& path);
  
  void sendToDCM(const string path,const double value,const int timestamp);

  void initFSR();
  void initSensorJoint();
  void initMotorJoint();
  void initLED();
  void initAccelerometer();
  void initGyrometer();
  void initInertialSensor();
  void initButton();
  void initIRSend();
  void initIRReceive();
  void initUltraSoundReceive();
  void initUltraSoundSend();

  void initAllSensorData();

public:
    int time_delay;

    DCMHandler();
    ~DCMHandler();
    void init(ALPtr<ALBroker> pB);

    string getBodyID();
    string getBodyNickName();

    // read sensor data from AL memory
    void readSensorData();
    
    // send acturator data to DCM
    void sendActuatorData();

    void get(SensorJointData& data) const;
    void get(FSRData& data) const;
    void get(AccelerometerData& data) const;
    void get(GyrometerData& data) const;
    void get(InertialSensorData& data) const;
    void get(IRReceiveData& data) const;
    void get(ButtonData& data) const;
    void get(UltraSoundReceiveData& data) const;
    void get(BatteryData& data) const;
    
    void set(const MotorJointData& data) { theMotorJointData = data; theMotorJointDataRequested = true; }
    void set(const LEDData& data) { theLEDData = data; theLEDDataRequested = true; }
    void set(const IRSendData& data) { theIRSendData = data; theIRSendDataRequested = true; }
    void set(const UltraSoundSendData& data) { theUltraSoundSendData = data; theUltraSoundSendDataRequested = true; }

    unsigned int getCurrentTimeStamp() const { return currentTimestamp; }

private:
    void setSingleMotorData(const JointData::JointID jointID,const MotorJointData *theMotorJointData);
    void setAllMotorData(const MotorJointData& mjd);
    
    void setLED(const LEDData& data);

    void setIRSend(const IRSendData& theIRSendData);
    void setUltraSoundSend(const UltraSoundSendData& data);
};

} // end namespace naoth

#endif	/* _DCMHANDLER_H */

