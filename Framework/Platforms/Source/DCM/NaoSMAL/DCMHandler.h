 /*
 * File:   DCMHandler.h
 * Author: Oliver Welter (welter@informatik.hu-berlin.de)
 *
 * Created on 14. Juli 2008, 13:16
 */

#ifndef _DCMHANDLER_H
#define	_DCMHANDLER_H

#include <string>

#include <boost/shared_ptr.hpp>
#include <alvalue/alvalue.h>

#include <alcommon/albroker.h>
#include <alproxies/dcmproxy.h>

#include <alproxies/almemoryproxy.h>
// TODO: why don't we use fastmemoryaccess anymore?
//http://doc.aldebaran.com/2-1/dev/cpp/examples/sensors/fastgetsetdcm/fastgetsetexample.html?highlight=almemoryfastaccess
//#include <almemoryfastaccess/almemoryfastaccess.h>

#include "Tools/DCMData.h"
#include "DCM_led.h"

namespace naoth
{

class DCMHandler
{
private:
  DCM_led ledHandler;

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

  //FSR
  std::string DCMPath_FSR_Left[FSRData::numOfFSR];
  std::string DCMPath_FSR_Right[FSRData::numOfFSR];

  //Accelerometer
  std::string DCMPath_Accelerometer[3+3];

  //Gyrometer
  std::string DCMPath_Gyrometer[3+3+1];

  //Inertial Sensors
  std::string DCMPath_InertialSensor[2];

  //Buttons
  std::string DCMPath_Button[ButtonData::numOfButtons];

  //UltraSound
  std::string DCMPath_UltraSoundReceive;
  std::string DCMPath_UltraSoundReceiveLeft[UltraSoundReceiveData::numOfUSEcho];
  std::string DCMPath_UltraSoundReceiveRight[UltraSoundReceiveData::numOfUSEcho];
  std::string DCMPath_UltraSoundSend;

  //Body-ID
  std::string DCMPath_BodyId;
  std::string DCMPath_BodyNickName;

  //Battery
  std::string DCMPath_BatteryCharge;
  std::string DCMPath_BatteryCurrent;
  std::string DCMPath_BatteryTemperature;

  //State of the devices
  std::string DCMPath_DeviceState;

  //
  std::string allSensorsList[numOfSensors];


  //DCMCommand-Structures
  AL::ALValue allMotorPositionCommands;
  AL::ALValue allMotorHardnessCommands;
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
  void initAccelerometer();
  void initGyrometer();
  void initInertialSensor();
  void initButton();
  void initUltraSoundReceive();
  void initUltraSoundSend();
  void initDeviceState();

  void initAllSensorData();

public:
  //
  float* sensorPtrs[numOfSensors];

  // remember last commands (needed by "smart" methods)
  MotorJointData lastMotorJointData;


  DCMHandler();
  ~DCMHandler();
  void init(boost::shared_ptr<AL::ALBroker> pB);

  void getJointPositionLimits(JointData& jointData);
  std::string getBodyID();
  std::string getBodyNickName();
  int getTime(unsigned int time_delay);

  // read sensor data from AL memory
  void readSensorData(float* dest);

  void setSingleMotorData(const JointData::JointID jointID, const MotorJointData *theMotorJointData, int dcmTime);

  void setAllPositionData(const MotorJointData& mjd, int dcmTime);
  void setAllHardnessData(const MotorJointData& mjd, int dcmTime);
  void setAllHardnessData(double value, int dcmTime);
  void setUltraSoundSend(const UltraSoundSendData& data, int dcmTime);

  void setLED(const LEDData& data, int dcmTime) { ledHandler.setAllLED(data, dcmTime); }
  bool setSingleLED(const LEDData& data, int dcmTime) { return ledHandler.setSingleLED(data, dcmTime);}

  // smart set_methods
  bool setAllHardnessDataSmart(const MotorJointData& mjd, int dcmTime);
};//end class DCMHandler

}//end namespace naoth
#endif	/* _DCMHANDLER_H */
