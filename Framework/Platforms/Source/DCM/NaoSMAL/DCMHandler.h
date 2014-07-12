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

#include <boost/shared_ptr.hpp>
#include <alvalue/alvalue.h>

#include <almemoryfastaccess/almemoryfastaccess.h>
#include <alproxies/almemoryproxy.h>
#include <alproxies/dcmproxy.h>

#include "Tools/IPCData.h"


//
// this is to suppress the following gcc warning 
// thrown because by the old version of boost used by naoqi
// albroker.h and alproxy.h 
// produce those:
//   boost/function/function_base.hpp:325: 
//   warning: dereferencing type-punned pointer will break strict-aliasing rules
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
#include <alcommon/albroker.h>


namespace naoth
{

class DCMHandler
{
private:
  boost::shared_ptr<AL::ALBroker> pBroker;
  boost::shared_ptr<AL::ALMemoryProxy> al_memory;
  boost::shared_ptr<AL::DCMProxy> al_dcmproxy;

  //Joints
  std::string DCMPath_MotorJointHardness[JointData::numOfJoint];
  std::string DCMPath_MotorJointPosition[JointData::numOfJoint];
  std::string DCMPath_SensorJointPosition[JointData::numOfJoint];
  std::string DCMPath_SensorJointElectricCurrent[JointData::numOfJoint];
  std::string DCMPath_SensorJointTemperature[JointData::numOfJoint];
    

  //LED
  std::string DCMPath_MonoLED[LEDData::numOfMonoLED];
  std::string DCMPath_MultiLED[LEDData::numOfMultiLED][LEDData::numOfLEDColor];

  //FSR
  std::string DCMPath_FSR[FSRData::numOfFSR];

  //Accelerometer
  std::string DCMPath_Accelerometer[3];

  //Gyrometer
  std::string DCMPath_Gyrometer[3+3+1];

  //Inertial Sensors
  std::string DCMPath_InertialSensor[2];

  //Buttons
  std::string DCMPath_Button[ButtonData::numOfButtons];

  //IR
  std::string DCMPath_IRSend[IRSendData::numOfIRSend];
  std::string DCMPath_IRReceive[IRReceiveData::numOfIRReceive];
    
  //UltraSound
  std::string DCMPath_UltraSoundReceive;
  std::string DCMPath_UltraSoundReceiveLeft[UltraSoundData::numOfUSEcho];
  std::string DCMPath_UltraSoundReceiveRight[UltraSoundData::numOfUSEcho];
  std::string DCMPath_UltraSoundSend;

  //Body-ID
  std::string DCMPath_BodyId;
  std::string DCMPath_BodyNickName;

  //Battery
  std::string DCMPath_BatteryCharge;

  //State of the devices
  std::string DCMPath_DeviceState;

  // 
  std::string allSensorsList[numOfSensors];


  //DCMCommand-Structures
  AL::ALValue allMotorPositionCommands;
  AL::ALValue allMotorHardnessCommands;
  AL::ALValue ledCommands;
  AL::ALValue singleLedCommand;
  AL::ALValue irCommands;
  AL::ALValue usSendCommands;

  /*
  * Retrieving a single value from ALMemory
  * For multiple data use al_memory->getListData() directly
  */
  AL::ALValue getFromALMemory(const std::string& path);
  
  void sendToDCM(const std::string& path, const double value, const int timestamp);

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
  void initDeviceState();

  void initAllSensorData();

public:
  // 
  float* sensorPtrs[numOfSensors];

  // remember last commands (needed by "smart" methods) 
  MotorJointData lastMotorJointData;
  LEDData lastLEDData;


  DCMHandler();
  ~DCMHandler();
  void init(boost::shared_ptr<AL::ALBroker> pB);

  void getJointPositionLimits(JointData& jointData);
  std::string getBodyID();
  std::string getBodyNickName();
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
  bool setSingleLED(const LEDData& data, int dcmTime);
  void setIRSend(const IRSendData& theIRSendData, int dcmTime);

  // smart set_methods
  bool setAllHardnessDataSmart(const MotorJointData& mjd, int dcmTime);
  bool setLEDSmart(const LEDData& data, int dcmTime);
};//end class DCMHandler

}//end namespace naoth
#endif	/* _DCMHANDLER_H */

