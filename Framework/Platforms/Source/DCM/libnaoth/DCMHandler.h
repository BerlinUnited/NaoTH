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

#include "Tools/SharedMemory.h"
#include "Tools/IPCData.h"

using namespace AL;
using namespace std;

namespace naoth 
{
 
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
  string DCMPath_Accelerometer[3];

  //Gyrometer
  string DCMPath_Gyrometer[2+1];

  //Inertial Sensors
  string DCMPath_InertialSensor[2];

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

  // 
  string allSensorsList[numOfSensors];


  //DCMCommand-Structures
  ALValue allMotorPositionCommands;
  ALValue allMotorHardnessCommands;
  ALValue ledCommands;
  ALValue singleLedCommand;
  ALValue irCommands;
  ALValue usSendCommands;

    
  ALValue getFromALMemory(const string& path);
  
  void sendToDCM(const string path,const double value,const int timestamp);

  void initFSR();
  void initSensorJoint();
  void initMotorJoint();
  void initLED();
  void initSingleLED();
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
  // 
  float* sensorPtrs[numOfSensors];

  // remember last commands (needed by "smart" methods) 
  MotorJointData lastMotorJointData;
  unsigned int last_us_mode;
  LEDData lastLEDData;


  DCMHandler();
  ~DCMHandler();
  void init(ALPtr<ALBroker> pB);

  string getBodyID();
  string getBodyNickName();
  int getTime(unsigned int time_delay);

  // read sensor data from AL memory
  void readSensorData(float* dest);
    
  //void set(const LEDData& data);
  //void set(const IRSendData& data);
  //void set(const UltraSoundSendData& data);

  void setSingleMotorData(const JointData::JointID jointID, const MotorJointData *theMotorJointData, int dcmTime);
    
  void setAllPositionData(const MotorJointData& mjd, int dcmTime);
  void setAllHardnessData(const MotorJointData& mjd, int dcmTime);
  void setAllHardnessData(double value, int dcmTime);

  void setUltraSoundSend(const UltraSoundSendData& data, int dcmTime);
  void setLED(const LEDData& data, int dcmTime);
  void setSingleLED(const LEDData& data, int dcmTime);
  void setIRSend(const IRSendData& theIRSendData, int dcmTime);

  // smart set_methods
  bool setAllHardnessDataSmart(const MotorJointData& mjd, int dcmTime);
  bool setUltraSoundSendSmart(const UltraSoundSendData& data, int dcmTime);
  bool setLEDSmart(const LEDData& data, int dcmTime);
};//end class DCMHandler

} // end namespace naoth

#endif	/* _DCMHANDLER_H */

