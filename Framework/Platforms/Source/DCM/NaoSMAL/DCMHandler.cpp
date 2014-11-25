

#include "DCMHandler.h"

//#include <Tools/NaoInfo.h>
//#include <Tools/NaoTime.h>

using namespace naoth;
using namespace std;
using namespace AL;


DCMHandler::DCMHandler()
{}

DCMHandler::~DCMHandler()
{}

void DCMHandler::init(boost::shared_ptr<ALBroker> pB)
{  
  pBroker = pB;

  //connect to DCM
  try
  {
    al_dcmproxy = boost::shared_ptr<AL::DCMProxy>(new DCMProxy(pB));
    al_memory = boost::shared_ptr<AL::ALMemoryProxy>(new AL::ALMemoryProxy(pB));
  }
  catch(ALError e) {
    std::cerr << "Failed to init DCMHandler: " << e.toString() << endl;
  }

  //Generate all strings for interaction

   // init sensors
  initSensorJoint();
  initFSR();
  initAccelerometer();
  initGyrometer();
  initInertialSensor();
  initIRReceive();
  initButton();
  initUltraSoundReceive();

  // init actuators
  initMotorJoint();
  initLED();
  initSingleLED();
  initIRSend();
  initUltraSoundSend();


  DCMPath_BodyId = "Device/DeviceList/ChestBoard/BodyId";
  DCMPath_BodyNickName = "Device/DeviceList/ChestBoard/BodyNickName";
  DCMPath_BatteryCharge = "Device/SubDeviceList/Battery/Charge/Sensor/Value";

  initAllSensorData();
}//end init


string DCMHandler::getBodyID()
{
  return getFromALMemory(DCMPath_BodyId);
}

string DCMHandler::getBodyNickName()
{
  return getFromALMemory(DCMPath_BodyNickName);
}

void DCMHandler::getJointPositionLimits(JointData& jointData)
{
  for(int i=0;i<JointData::numOfJoint;i++)
  {
    if(i == JointData::RHipYawPitch) {
      continue;
    }
    string joint_max_path = "Device/SubDeviceList/" + JointData::getJointName((JointData::JointID) i) + "/Position/Actuator/Max";
    string joint_min_path = "Device/SubDeviceList/" + JointData::getJointName((JointData::JointID) i) + "/Position/Actuator/Min";

    const ALValue al_joint_max = getFromALMemory(joint_max_path);
    const ALValue al_joint_min = getFromALMemory(joint_min_path);
    
    jointData.min[i] = float(al_joint_min);
    jointData.max[i] = float(al_joint_max);
  }

  jointData.min[JointData::RHipYawPitch] = jointData.min[JointData::LHipYawPitch];
  jointData.max[JointData::RHipYawPitch] = jointData.max[JointData::LHipYawPitch];
}

int DCMHandler::getTime(unsigned int time_delay)
{
  return al_dcmproxy->getTime(time_delay);
}

ALValue DCMHandler::getFromALMemory(const string& path)
{
  ALValue value;
  try
  {
    value = al_memory->getData(path.c_str(), 0);
  } catch (ALError e)
  {
    std::cerr << "Failed to get Information from: " << e.toString() << endl;
  }
  return value;
}//end getFromALMemory

/*
 * function for sending a single value to DCM
 * For multiple data use Alias together with DCM directly
 */
inline void DCMHandler::sendToDCM(const string& path, const double value, const int timestamp)
{
  ALValue commands;
  try
  {
    commands.arraySetSize(3);
    commands[0] = path;
    commands[1] = string("ClearAll");
    commands[2].arraySetSize(1);
    commands[2][0].arraySetSize(2);
    commands[2][0][0] = value;
    commands[2][0][1] = timestamp;
    al_dcmproxy->set(commands);
  }
  catch(ALError e)
  {
    std::cerr << "Failed to set " << path << ": " << e.toString() << endl;
  }
}//end sendToDCM


//////////////////////////// BEGIN CREATE SENSOR PATHS ////////////////////////////

// SensorJoint
void DCMHandler::initSensorJoint()
{
  for(int i=0;i<JointData::numOfJoint;i++)
  {
    DCMPath_SensorJointPosition[(JointData::JointID) i] =
     "Device/SubDeviceList/" + JointData::getJointName((JointData::JointID) i) + "/Position/Sensor/Value"; 

    DCMPath_SensorJointElectricCurrent[(JointData::JointID) i] =
      "Device/SubDeviceList/" + JointData::getJointName((JointData::JointID) i) + "/ElectricCurrent/Sensor/Value";

    DCMPath_SensorJointTemperature[(JointData::JointID) i] =
      "Device/SubDeviceList/" + JointData::getJointName((JointData::JointID) i) + "/Temperature/Sensor/Value";
  }
}//end initSensorJoint


// fsr
void DCMHandler::initFSR()
{
  DCMPath_FSR[FSRData::LFsrFL] = "Device/SubDeviceList/LFoot/FSR/FrontLeft/Sensor/Value";
  DCMPath_FSR[FSRData::LFsrFR] = "Device/SubDeviceList/LFoot/FSR/FrontRight/Sensor/Value";
  DCMPath_FSR[FSRData::LFsrBL] = "Device/SubDeviceList/LFoot/FSR/RearLeft/Sensor/Value";
  DCMPath_FSR[FSRData::LFsrBR] = "Device/SubDeviceList/LFoot/FSR/RearRight/Sensor/Value";
  DCMPath_FSR[FSRData::RFsrFL] = "Device/SubDeviceList/RFoot/FSR/FrontLeft/Sensor/Value";
  DCMPath_FSR[FSRData::RFsrFR] = "Device/SubDeviceList/RFoot/FSR/FrontRight/Sensor/Value";
  DCMPath_FSR[FSRData::RFsrBL] = "Device/SubDeviceList/RFoot/FSR/RearLeft/Sensor/Value";
  DCMPath_FSR[FSRData::RFsrBR] = "Device/SubDeviceList/RFoot/FSR/RearRight/Sensor/Value";
}//end initFSR


void DCMHandler::initAccelerometer()
{
  DCMPath_Accelerometer[0] = "Device/SubDeviceList/InertialSensor/AccX/Sensor/Value";
  DCMPath_Accelerometer[1] = "Device/SubDeviceList/InertialSensor/AccY/Sensor/Value";
  DCMPath_Accelerometer[2] = "Device/SubDeviceList/InertialSensor/AccZ/Sensor/Value";

  DCMPath_Accelerometer[3] = "Device/SubDeviceList/InertialSensor/AccelerometerX/Sensor/Value";
  DCMPath_Accelerometer[4] = "Device/SubDeviceList/InertialSensor/AccelerometerY/Sensor/Value";
  DCMPath_Accelerometer[5] = "Device/SubDeviceList/InertialSensor/AccelerometerZ/Sensor/Value";
}//end initAccelerometer

void DCMHandler::initGyrometer()
{
  DCMPath_Gyrometer[0] = "Device/SubDeviceList/InertialSensor/GyrX/Sensor/Value";
  DCMPath_Gyrometer[1] = "Device/SubDeviceList/InertialSensor/GyrY/Sensor/Value";
  DCMPath_Gyrometer[2] = "Device/SubDeviceList/InertialSensor/GyrZ/Sensor/Value";

  DCMPath_Gyrometer[3] = "Device/SubDeviceList/InertialSensor/GyroscopeX/Sensor/Value";
  DCMPath_Gyrometer[4] = "Device/SubDeviceList/InertialSensor/GyroscopeY/Sensor/Value";
  DCMPath_Gyrometer[5] = "Device/SubDeviceList/InertialSensor/GyroscopeZ/Sensor/Value";

  DCMPath_Gyrometer[6] = "Device/SubDeviceList/InertialSensor/GyrRef/Sensor/Value";
}//end initGyrometer

void DCMHandler::initInertialSensor()
{
  DCMPath_InertialSensor[0] = "Device/SubDeviceList/InertialSensor/AngleX/Sensor/Value";
  DCMPath_InertialSensor[1] = "Device/SubDeviceList/InertialSensor/AngleY/Sensor/Value";
}//end initInertialSensor

void DCMHandler::initIRReceive()
{
  DCMPath_IRReceive[IRReceiveData::LeftBeacon] =  "Device/SubDeviceList/IR/BeaconCode/Left/Sensor/Value";
  DCMPath_IRReceive[IRReceiveData::LeftByte1] = "Device/SubDeviceList/IR/RobotCode/Left/Byte1/Sensor/Value";
  DCMPath_IRReceive[IRReceiveData::LeftByte2] = "Device/SubDeviceList/IR/RobotCode/Left/Byte2/Sensor/Value";
  DCMPath_IRReceive[IRReceiveData::LeftByte3] = "Device/SubDeviceList/IR/RobotCode/Left/Byte3/Sensor/Value";
  DCMPath_IRReceive[IRReceiveData::LeftByte4] = "Device/SubDeviceList/IR/RobotCode/Left/Byte4/Sensor/Value";
  DCMPath_IRReceive[IRReceiveData::LeftRCByte1] = "Device/SubDeviceList/IR/RC5Code/Left/Byte1/Sensor/Value";
  DCMPath_IRReceive[IRReceiveData::LeftRCByte2] = "Device/SubDeviceList/IR/RC5Code/Left/Byte2/Sensor/Value";

  DCMPath_IRReceive[IRReceiveData::RightBeacon] =  "Device/SubDeviceList/IR/BeaconCode/Right/Sensor/Value";
  DCMPath_IRReceive[IRReceiveData::RightByte1] = "Device/SubDeviceList/IR/RobotCode/Right/Byte1/Sensor/Value";
  DCMPath_IRReceive[IRReceiveData::RightByte2] = "Device/SubDeviceList/IR/RobotCode/Right/Byte2/Sensor/Value";
  DCMPath_IRReceive[IRReceiveData::RightByte3] = "Device/SubDeviceList/IR/RobotCode/Right/Byte3/Sensor/Value";
  DCMPath_IRReceive[IRReceiveData::RightByte4] = "Device/SubDeviceList/IR/RobotCode/Right/Byte4/Sensor/Value";
  DCMPath_IRReceive[IRReceiveData::RightRCByte1] = "Device/SubDeviceList/IR/RC5Code/Right/Byte1/Sensor/Value";
  DCMPath_IRReceive[IRReceiveData::RightRCByte2] = "Device/SubDeviceList/IR/RC5Code/Right/Byte2/Sensor/Value";
}//end initIRReceive

void DCMHandler::initButton()
{
  DCMPath_Button[ButtonData::Chest] = "Device/SubDeviceList/ChestBoard/Button/Sensor/Value";
  DCMPath_Button[ButtonData::LeftFootLeft] = "Device/SubDeviceList/LFoot/Bumper/Left/Sensor/Value";
  DCMPath_Button[ButtonData::LeftFootRight] = "Device/SubDeviceList/LFoot/Bumper/Right/Sensor/Value";
  DCMPath_Button[ButtonData::RightFootLeft] = "Device/SubDeviceList/RFoot/Bumper/Left/Sensor/Value";
  DCMPath_Button[ButtonData::RightFootRight] = "Device/SubDeviceList/RFoot/Bumper/Right/Sensor/Value";

  DCMPath_Button[ButtonData::HeadFront]       = "Device/SubDeviceList/Head/Touch/Front/Sensor/Value";
  DCMPath_Button[ButtonData::HeadMiddle]      = "Device/SubDeviceList/Head/Touch/Middle/Sensor/Value";
  DCMPath_Button[ButtonData::HeadRear]        = "Device/SubDeviceList/Head/Touch/Rear/Sensor/Value";
  DCMPath_Button[ButtonData::LeftHandBack]    = "Device/SubDeviceList/LHand/Touch/Back/Sensor/Value";
  DCMPath_Button[ButtonData::LeftHandLeft]    = "Device/SubDeviceList/LHand/Touch/Left/Sensor/Value";
  DCMPath_Button[ButtonData::LeftHandRight]   = "Device/SubDeviceList/LHand/Touch/Right/Sensor/Value";
  DCMPath_Button[ButtonData::RightHandBack]   = "Device/SubDeviceList/RHand/Touch/Back/Sensor/Value";
  DCMPath_Button[ButtonData::RightHandLeft]   = "Device/SubDeviceList/RHand/Touch/Left/Sensor/Value";
  DCMPath_Button[ButtonData::RightHandRight]  = "Device/SubDeviceList/RHand/Touch/Right/Sensor/Value";
}//end initButton


void DCMHandler::initUltraSoundReceive()
{
  DCMPath_UltraSoundReceive = "Device/SubDeviceList/US/Sensor/Value";
  
  DCMPath_UltraSoundReceiveLeft[0] = "Device/SubDeviceList/US/Left/Sensor/Value";
  DCMPath_UltraSoundReceiveLeft[1] = "Device/SubDeviceList/US/Left/Sensor/Value1";
  DCMPath_UltraSoundReceiveLeft[2] = "Device/SubDeviceList/US/Left/Sensor/Value2";
  DCMPath_UltraSoundReceiveLeft[3] = "Device/SubDeviceList/US/Left/Sensor/Value3";
  DCMPath_UltraSoundReceiveLeft[4] = "Device/SubDeviceList/US/Left/Sensor/Value4";
  DCMPath_UltraSoundReceiveLeft[5] = "Device/SubDeviceList/US/Left/Sensor/Value5";
  DCMPath_UltraSoundReceiveLeft[6] = "Device/SubDeviceList/US/Left/Sensor/Value6";
  DCMPath_UltraSoundReceiveLeft[7] = "Device/SubDeviceList/US/Left/Sensor/Value7";
  DCMPath_UltraSoundReceiveLeft[8] = "Device/SubDeviceList/US/Left/Sensor/Value8";
  DCMPath_UltraSoundReceiveLeft[9] = "Device/SubDeviceList/US/Left/Sensor/Value9";

  DCMPath_UltraSoundReceiveRight[0] = "Device/SubDeviceList/US/Right/Sensor/Value";
  DCMPath_UltraSoundReceiveRight[1] = "Device/SubDeviceList/US/Right/Sensor/Value1";
  DCMPath_UltraSoundReceiveRight[2] = "Device/SubDeviceList/US/Right/Sensor/Value2";
  DCMPath_UltraSoundReceiveRight[3] = "Device/SubDeviceList/US/Right/Sensor/Value3";
  DCMPath_UltraSoundReceiveRight[4] = "Device/SubDeviceList/US/Right/Sensor/Value4";
  DCMPath_UltraSoundReceiveRight[5] = "Device/SubDeviceList/US/Right/Sensor/Value5";
  DCMPath_UltraSoundReceiveRight[6] = "Device/SubDeviceList/US/Right/Sensor/Value6";
  DCMPath_UltraSoundReceiveRight[7] = "Device/SubDeviceList/US/Right/Sensor/Value7";
  DCMPath_UltraSoundReceiveRight[8] = "Device/SubDeviceList/US/Right/Sensor/Value8";
  DCMPath_UltraSoundReceiveRight[9] = "Device/SubDeviceList/US/Right/Sensor/Value9";
}//end initUltraSoundReceive


void DCMHandler::initDeviceState()
{
  /*
  "Device/DeviceList/ChestBoard/Ack",
  "Device/DeviceList/ChestBoard/Nack",
  "Device/DeviceList/ChestBoard/Error",
  "Device/DeviceList/Battery/Ack",
  "Device/DeviceList/Battery/Nack",
  "Device/DeviceList/Battery/Error",
  "Device/DeviceList/USBoard/Ack",
  "Device/DeviceList/USBoard/Nack",
  "Device/DeviceList/USBoard/Error",
  "Device/DeviceList/InertialSensor/Ack",
  "Device/DeviceList/InertialSensor/Nack",
  "Device/DeviceList/InertialSensor/Error",
  "Device/DeviceList/HeadBoard/Ack",
  "Device/DeviceList/HeadBoard/Nack",
  "Device/DeviceList/HeadBoard/Error",
  "Device/DeviceList/EarLeds/Ack",
  "Device/DeviceList/EarLeds/Nack",
  "Device/DeviceList/EarLeds/Error",
  "Device/DeviceList/FaceBoard/Ack",
  "Device/DeviceList/FaceBoard/Nack",
  "Device/DeviceList/FaceBoard/Error",
  "Device/DeviceList/LeftShoulderBoard/Ack",
  "Device/DeviceList/LeftShoulderBoard/Nack",
  "Device/DeviceList/LeftShoulderBoard/Error",
  "Device/DeviceList/LeftArmBoard/Ack",
  "Device/DeviceList/LeftArmBoard/Nack",
  "Device/DeviceList/LeftArmBoard/Error",
  "Device/DeviceList/RightShoulderBoard/Ack",
  "Device/DeviceList/RightShoulderBoard/Nack",
  "Device/DeviceList/RightShoulderBoard/Error",
  "Device/DeviceList/RightArmBoard/Ack",
  "Device/DeviceList/RightArmBoard/Nack",
  "Device/DeviceList/RightArmBoard/Error",
  "Device/DeviceList/LeftHipBoard/Ack",
  "Device/DeviceList/LeftHipBoard/Nack",
  "Device/DeviceList/LeftHipBoard/Error",
  "Device/DeviceList/LeftThighBoard/Ack",
  "Device/DeviceList/LeftThighBoard/Nack",
  "Device/DeviceList/LeftThighBoard/Error",
  "Device/DeviceList/LeftShinBoard/Ack",
  "Device/DeviceList/LeftShinBoard/Nack",
  "Device/DeviceList/LeftShinBoard/Error",
  "Device/DeviceList/LeftFootBoard/Ack",
  "Device/DeviceList/LeftFootBoard/Nack",
  "Device/DeviceList/LeftFootBoard/Error",
  "Device/DeviceList/RightHipBoard/Ack",
  "Device/DeviceList/RightHipBoard/Nack",
  "Device/DeviceList/RightHipBoard/Error",
  "Device/DeviceList/RightThighBoard/Ack",
  "Device/DeviceList/RightThighBoard/Nack",
  "Device/DeviceList/RightThighBoard/Error",
  "Device/DeviceList/RightShinBoard/Ack",
  "Device/DeviceList/RightShinBoard/Nack",
  "Device/DeviceList/RightShinBoard/Error",
  "Device/DeviceList/RightFootBoard/Ack",
  "Device/DeviceList/RightFootBoard/Nack",
  "Device/DeviceList/RightFootBoard/Error"
  */
}//end initDeviceState


// create an array containing all sensors
void DCMHandler::initAllSensorData()
{
  unsigned int currentIndex=0;

  //SensorJointData
  ASSERT(theSensorJointDataIndex == currentIndex);
  for (int i = 0; i < JointData::RHipYawPitch; i++)
  {
    allSensorsList[currentIndex++] = DCMPath_SensorJointElectricCurrent[i];
    allSensorsList[currentIndex++] = DCMPath_SensorJointTemperature[i];
    allSensorsList[currentIndex++] = DCMPath_SensorJointPosition[i];
    allSensorsList[currentIndex++] = DCMPath_MotorJointHardness[i];
  }
  // avoid the LHipYawPitch
  for (int i = JointData::RHipYawPitch + 1; i < JointData::numOfJoint; i++)
  {
    allSensorsList[currentIndex++] = DCMPath_SensorJointElectricCurrent[i];
    allSensorsList[currentIndex++] = DCMPath_SensorJointTemperature[i];
    allSensorsList[currentIndex++] = DCMPath_SensorJointPosition[i];
    allSensorsList[currentIndex++] = DCMPath_MotorJointHardness[i];
  }

  //FSRData
  ASSERT(theFSRDataIndex == currentIndex);
  for(int i=0;i<FSRData::numOfFSR;i++)
  {
    allSensorsList[currentIndex++] = DCMPath_FSR[i];
  }

  //AccelerometerData
  ASSERT(theAccelerometerDataIndex == currentIndex);
  for(int i=0;i<3+3;i++)
  {
    allSensorsList[currentIndex++] = DCMPath_Accelerometer[i];
  }

  //GyrometerData
  ASSERT(theGyrometerDataIndex == currentIndex);
  for(int i=0;i<3+3+1;i++)
  {
    allSensorsList[currentIndex++] = DCMPath_Gyrometer[i];
  }

  //InertialSensorsData
  ASSERT(theInertialSensorDataIndex == currentIndex);
  for(int i=0;i<2;i++)
  {
    allSensorsList[currentIndex++] = DCMPath_InertialSensor[i];
  }

  //IRReceiveData
  ASSERT(theIRReceiveDataIndex == currentIndex);
  for(int i=0;i<IRReceiveData::numOfIRReceive;i++)
  {
    allSensorsList[currentIndex++] = DCMPath_IRReceive[i];
  }

  //ButtonData
  ASSERT(theButtonDataIndex == currentIndex);
  for(int i=0;i<ButtonData::numOfButtons;i++)
  {
    allSensorsList[currentIndex++] = DCMPath_Button[i];
  }

  //UltraSoundReceiveData
  ASSERT(theUltraSoundReceiveDataIndex == currentIndex);
  allSensorsList[currentIndex++] = DCMPath_UltraSoundReceive; 
  for(int i = 0; i < UltraSoundData::numOfUSEcho; i++)
  {
    allSensorsList[currentIndex++] = DCMPath_UltraSoundReceiveLeft[i];
    allSensorsList[currentIndex++] = DCMPath_UltraSoundReceiveRight[i];
  }

  ASSERT(thBatteryDataIdex == currentIndex);
  allSensorsList[currentIndex++] = DCMPath_BatteryCharge;

  // little senity check
  assert(currentIndex == numOfSensors);

  //connect variables
  try {
    for(unsigned int i=0; i<numOfSensors; i++)
    {
      sensorPtrs[i] = (float*)al_memory->getDataPtr(allSensorsList[i]);
    }
  } catch(AL::ALError& e)
  {
    std::cerr << "[NaoSMAL] " << e.toString() << std::endl;
  }

}//end initAllSensorData


void DCMHandler::readSensorData(float* dest)
{
  //timeStamp = al_dcmproxy->getTime(time_delay);

  for(unsigned int i=0; i<numOfSensors; i++)
  {
    dest[i] = *(sensorPtrs[i]);
  }
}//end readSensorData

//////////////////////////// END CREATE SENSOR PATHS ////////////////////////////



// MotorJoint
void DCMHandler::initMotorJoint()
{
  for(int i=0;i<JointData::numOfJoint;i++)
  {
    DCMPath_MotorJointPosition[(JointData::JointID) i] =
      "Device/SubDeviceList/" + JointData::getJointName((JointData::JointID) i) + "/Position/Actuator/Value";

    DCMPath_MotorJointHardness[(JointData::JointID) i] =
      "Device/SubDeviceList/" + JointData::getJointName((JointData::JointID) i) + "/Hardness/Actuator/Value";
  }

  try {
    ALValue aliasCommandPosition;
    ALValue aliasCommandHardness;
    aliasCommandPosition.arraySetSize(2);
    aliasCommandHardness.arraySetSize(2);

    //AllMotorJoints
    aliasCommandPosition[0] = string("MPos");
    aliasCommandHardness[0] = string("MHard");
    //RHipYawPitch doesn't exist
    aliasCommandPosition[1].arraySetSize(JointData::numOfJoint - 1);
    aliasCommandHardness[1].arraySetSize(JointData::numOfJoint - 1);
    for(int i=0;i<JointData::RHipYawPitch;i++)
    {
      aliasCommandHardness[1][i] = DCMPath_MotorJointHardness[i];
      aliasCommandPosition[1][i] = DCMPath_MotorJointPosition[i];
    }
    for(int i=JointData::RHipYawPitch+1;i<JointData::numOfJoint;i++)
    {
      aliasCommandHardness[1][i-1] = DCMPath_MotorJointHardness[i];
      aliasCommandPosition[1][i-1] = DCMPath_MotorJointPosition[i];
    }

    al_dcmproxy->createAlias(aliasCommandPosition);
    al_dcmproxy->createAlias(aliasCommandHardness);

    allMotorPositionCommands.arraySetSize(6);
    allMotorHardnessCommands.arraySetSize(6);
    
    allMotorPositionCommands[0] = string("MPos");
    allMotorHardnessCommands[0] = string("MHard");
    allMotorPositionCommands[1] = string("ClearAll");
    allMotorHardnessCommands[1] = string("ClearAll");
    allMotorPositionCommands[2] = string("time-separate");
    allMotorHardnessCommands[2] = string("time-separate");
    allMotorPositionCommands[3] = 0;
    allMotorHardnessCommands[3] = 0;
    allMotorPositionCommands[4].arraySetSize(1);
    allMotorHardnessCommands[4].arraySetSize(1);
    allMotorPositionCommands[4][0] = 0;
    allMotorHardnessCommands[4][0] = 0;
    allMotorPositionCommands[5].arraySetSize(JointData::numOfJoint-1);
    allMotorHardnessCommands[5].arraySetSize(JointData::numOfJoint-1);

    for(int i=0;i<JointData::RHipYawPitch;i++)
    {
      allMotorPositionCommands[5][i].arraySetSize(1);
      allMotorPositionCommands[5][i][0] = 0.0;
      allMotorHardnessCommands[5][i].arraySetSize(1);
      allMotorHardnessCommands[5][i][0] = 0.0;
    }
    for(int i=JointData::RHipYawPitch+1;i<JointData::numOfJoint;i++)
    {
      allMotorPositionCommands[5][i-1].arraySetSize(1);
      allMotorPositionCommands[5][i-1][0] = 0.0;
      allMotorHardnessCommands[5][i-1].arraySetSize(1);
      allMotorHardnessCommands[5][i-1][0] = 0.0;
    }

  }
  catch(ALError e) {
    std::cerr << "[NaoSMAL] Failed to init MotorJointAlias: " << e.toString() << endl;
  }
}//end initMotorJoint


void DCMHandler::setSingleMotorData(const JointData::JointID jointID, const MotorJointData *theMotorJointData, int dcmTime)
{
  sendToDCM(DCMPath_MotorJointHardness[jointID],theMotorJointData->stiffness[jointID],dcmTime);
  sendToDCM(DCMPath_MotorJointPosition[jointID],theMotorJointData->position[jointID],dcmTime);
}//end setSingleMotorData


void DCMHandler::setAllPositionData(const MotorJointData& mjd, int dcmTime)
{
  allMotorPositionCommands[4][0] = dcmTime;

  //MotorJoints
  for(int i=0;i<JointData::RHipYawPitch;i++)
  {
    allMotorPositionCommands[5][i][0] = mjd.position[i];
  }
  for(int i=JointData::RHipYawPitch+1;i<JointData::numOfJoint;i++)
  {
    allMotorPositionCommands[5][i-1][0] = mjd.position[i];
  }

  try
  {
    al_dcmproxy->setAlias(allMotorPositionCommands);
  }
  catch(ALError e) {
    std::cerr << "[NaoSMAL] Failed to set AllPositionData: " << e.toString() << endl;
  }
}//end setAllPositionData


bool DCMHandler::setAllHardnessDataSmart(const MotorJointData& mjd, int dcmTime)
{
  // check if there are any changes
  for(int i=0;i<JointData::numOfJoint;i++)
  {
    if(mjd.stiffness[i] != lastMotorJointData.stiffness[i])
    {
      // copy all
      lastMotorJointData = mjd;
      setAllHardnessData(mjd, dcmTime);
      return true;
    }//end if
  }//end for

  return false;
}//end setAllHardnessDataSmart



void DCMHandler::setAllHardnessData(const MotorJointData& mjd, int dcmTime)
{
  allMotorHardnessCommands[4][0] = dcmTime;

  //MotorJoints
  for(int i=0;i<JointData::RHipYawPitch;i++)
  {
    allMotorHardnessCommands[5][i][0] = mjd.stiffness[i];
  }
  for(int i=JointData::RHipYawPitch+1;i<JointData::numOfJoint;i++)
  {
    allMotorHardnessCommands[5][i-1][0] = mjd.stiffness[i];
  }

  try
  {
    al_dcmproxy->setAlias(allMotorHardnessCommands);
  }
  catch(ALError e) {
    std::cerr << "[NaoSMAL] Failed to set AllHardnessData: " << e.toString() << endl;
  }
}//end setAllHardnessData


void DCMHandler::setAllHardnessData(double value, int dcmTime)
{
  allMotorHardnessCommands[4][0] = dcmTime;

  //MotorJoints
  for(int i=0;i<JointData::RHipYawPitch;i++)
  {
    allMotorHardnessCommands[5][i][0] = value;
  }
  for(int i=JointData::RHipYawPitch+1;i<JointData::numOfJoint;i++)
  {
    allMotorHardnessCommands[5][i-1][0] = value;
  }

  try
  {
    al_dcmproxy->setAlias(allMotorHardnessCommands);
  }
  catch(ALError e) {
    std::cerr << "[NaoSMAL] Failed to set AllHardnessData: " << e.toString() << endl;
  }
}//end setAllHardnessData



//LED
void DCMHandler::initLED()
{
  DCMPath_MonoLED[LEDData::EarLeft0]    = "Ears/Led/Left/0Deg/Actuator/Value";
  DCMPath_MonoLED[LEDData::EarLeft36]   = "Ears/Led/Left/36Deg/Actuator/Value";
  DCMPath_MonoLED[LEDData::EarLeft72]   = "Ears/Led/Left/72Deg/Actuator/Value";
  DCMPath_MonoLED[LEDData::EarLeft108]  = "Ears/Led/Left/108Deg/Actuator/Value";
  DCMPath_MonoLED[LEDData::EarLeft144]  = "Ears/Led/Left/144Deg/Actuator/Value";
  DCMPath_MonoLED[LEDData::EarLeft180]  = "Ears/Led/Left/180Deg/Actuator/Value";
  DCMPath_MonoLED[LEDData::EarLeft216]  = "Ears/Led/Left/216Deg/Actuator/Value";
  DCMPath_MonoLED[LEDData::EarLeft252]  = "Ears/Led/Left/252Deg/Actuator/Value";
  DCMPath_MonoLED[LEDData::EarLeft288]  = "Ears/Led/Left/288Deg/Actuator/Value";
  DCMPath_MonoLED[LEDData::EarLeft324]  = "Ears/Led/Left/324Deg/Actuator/Value";

  DCMPath_MonoLED[LEDData::EarRight0]   = "Ears/Led/Right/0Deg/Actuator/Value";
  DCMPath_MonoLED[LEDData::EarRight36]  = "Ears/Led/Right/36Deg/Actuator/Value";
  DCMPath_MonoLED[LEDData::EarRight72]  = "Ears/Led/Right/72Deg/Actuator/Value";
  DCMPath_MonoLED[LEDData::EarRight108] = "Ears/Led/Right/108Deg/Actuator/Value";
  DCMPath_MonoLED[LEDData::EarRight144] = "Ears/Led/Right/144Deg/Actuator/Value";
  DCMPath_MonoLED[LEDData::EarRight180] = "Ears/Led/Right/180Deg/Actuator/Value";
  DCMPath_MonoLED[LEDData::EarRight216] = "Ears/Led/Right/216Deg/Actuator/Value";
  DCMPath_MonoLED[LEDData::EarRight252] = "Ears/Led/Right/252Deg/Actuator/Value";
  DCMPath_MonoLED[LEDData::EarRight288] = "Ears/Led/Right/288Deg/Actuator/Value";
  DCMPath_MonoLED[LEDData::EarRight324] = "Ears/Led/Right/324Deg/Actuator/Value";

  DCMPath_MonoLED[LEDData::HeadFrontLeft0]    = "Head/Led/Front/Left/0/Actuator/Value";
  DCMPath_MonoLED[LEDData::HeadFrontLeft1]    = "Head/Led/Front/Left/1/Actuator/Value";
  DCMPath_MonoLED[LEDData::HeadFrontRight0]   = "Head/Led/Front/Right/0/Actuator/Value";
  DCMPath_MonoLED[LEDData::HeadFrontRight1]   = "Head/Led/Front/Right/1/Actuator/Value";
  DCMPath_MonoLED[LEDData::HeadMiddleLeft0]   = "Head/Led/Middle/Left/0/Actuator/Value";
  DCMPath_MonoLED[LEDData::HeadMiddleRight0]  = "Head/Led/Middle/Right/0/Actuator/Value";
  DCMPath_MonoLED[LEDData::HeadRearLeft0]     = "Head/Led/Rear/Left/0/Actuator/Value";
  DCMPath_MonoLED[LEDData::HeadRearLeft1]     = "Head/Led/Rear/Left/1/Actuator/Value";
  DCMPath_MonoLED[LEDData::HeadRearLeft2]     = "Head/Led/Rear/Left/2/Actuator/Value";
  DCMPath_MonoLED[LEDData::HeadRearRight0]    = "Head/Led/Rear/Right/0/Actuator/Value";
  DCMPath_MonoLED[LEDData::HeadRearRight1]    = "Head/Led/Rear/Right/1/Actuator/Value";
  DCMPath_MonoLED[LEDData::HeadRearRight2]    = "Head/Led/Rear/Right/2/Actuator/Value";

  DCMPath_MultiLED[LEDData::FaceLeft0][LEDData::RED]      = "Face/Led/Red/Left/0Deg/Actuator/Value";
  DCMPath_MultiLED[LEDData::FaceLeft0][LEDData::GREEN]    = "Face/Led/Green/Left/0Deg/Actuator/Value";
  DCMPath_MultiLED[LEDData::FaceLeft0][LEDData::BLUE]     = "Face/Led/Blue/Left/0Deg/Actuator/Value";
  DCMPath_MultiLED[LEDData::FaceLeft45][LEDData::RED]     = "Face/Led/Red/Left/45Deg/Actuator/Value";
  DCMPath_MultiLED[LEDData::FaceLeft45][LEDData::GREEN]   = "Face/Led/Green/Left/45Deg/Actuator/Value";
  DCMPath_MultiLED[LEDData::FaceLeft45][LEDData::BLUE]    = "Face/Led/Blue/Left/45Deg/Actuator/Value";
  DCMPath_MultiLED[LEDData::FaceLeft90][LEDData::RED]     = "Face/Led/Red/Left/90Deg/Actuator/Value";
  DCMPath_MultiLED[LEDData::FaceLeft90][LEDData::GREEN]   = "Face/Led/Green/Left/90Deg/Actuator/Value";
  DCMPath_MultiLED[LEDData::FaceLeft90][LEDData::BLUE]    = "Face/Led/Blue/Left/90Deg/Actuator/Value";
  DCMPath_MultiLED[LEDData::FaceLeft135][LEDData::RED]    = "Face/Led/Red/Left/135Deg/Actuator/Value";
  DCMPath_MultiLED[LEDData::FaceLeft135][LEDData::GREEN]  = "Face/Led/Green/Left/135Deg/Actuator/Value";
  DCMPath_MultiLED[LEDData::FaceLeft135][LEDData::BLUE]   = "Face/Led/Blue/Left/135Deg/Actuator/Value";
  DCMPath_MultiLED[LEDData::FaceLeft180][LEDData::RED]    = "Face/Led/Red/Left/180Deg/Actuator/Value";
  DCMPath_MultiLED[LEDData::FaceLeft180][LEDData::GREEN]  = "Face/Led/Green/Left/180Deg/Actuator/Value";
  DCMPath_MultiLED[LEDData::FaceLeft180][LEDData::BLUE]   = "Face/Led/Blue/Left/180Deg/Actuator/Value";
  DCMPath_MultiLED[LEDData::FaceLeft225][LEDData::RED]    = "Face/Led/Red/Left/225Deg/Actuator/Value";
  DCMPath_MultiLED[LEDData::FaceLeft225][LEDData::GREEN]  = "Face/Led/Green/Left/225Deg/Actuator/Value";
  DCMPath_MultiLED[LEDData::FaceLeft225][LEDData::BLUE]   = "Face/Led/Blue/Left/225Deg/Actuator/Value";
  DCMPath_MultiLED[LEDData::FaceLeft270][LEDData::RED]    = "Face/Led/Red/Left/270Deg/Actuator/Value";
  DCMPath_MultiLED[LEDData::FaceLeft270][LEDData::GREEN]  = "Face/Led/Green/Left/270Deg/Actuator/Value";
  DCMPath_MultiLED[LEDData::FaceLeft270][LEDData::BLUE]   = "Face/Led/Blue/Left/270Deg/Actuator/Value";
  DCMPath_MultiLED[LEDData::FaceLeft315][LEDData::RED]    = "Face/Led/Red/Left/315Deg/Actuator/Value";
  DCMPath_MultiLED[LEDData::FaceLeft315][LEDData::GREEN]  = "Face/Led/Green/Left/315Deg/Actuator/Value";
  DCMPath_MultiLED[LEDData::FaceLeft315][LEDData::BLUE]   = "Face/Led/Blue/Left/315Deg/Actuator/Value";

  DCMPath_MultiLED[LEDData::FaceRight0][LEDData::RED]     = "Face/Led/Red/Right/0Deg/Actuator/Value";
  DCMPath_MultiLED[LEDData::FaceRight0][LEDData::GREEN]   = "Face/Led/Green/Right/0Deg/Actuator/Value";
  DCMPath_MultiLED[LEDData::FaceRight0][LEDData::BLUE]    = "Face/Led/Blue/Right/0Deg/Actuator/Value";
  DCMPath_MultiLED[LEDData::FaceRight45][LEDData::RED]    = "Face/Led/Red/Right/45Deg/Actuator/Value";
  DCMPath_MultiLED[LEDData::FaceRight45][LEDData::GREEN]  = "Face/Led/Green/Right/45Deg/Actuator/Value";
  DCMPath_MultiLED[LEDData::FaceRight45][LEDData::BLUE]   = "Face/Led/Blue/Right/45Deg/Actuator/Value";
  DCMPath_MultiLED[LEDData::FaceRight90][LEDData::RED]    = "Face/Led/Red/Right/90Deg/Actuator/Value";
  DCMPath_MultiLED[LEDData::FaceRight90][LEDData::GREEN]  = "Face/Led/Green/Right/90Deg/Actuator/Value";
  DCMPath_MultiLED[LEDData::FaceRight90][LEDData::BLUE]   = "Face/Led/Blue/Right/90Deg/Actuator/Value";
  DCMPath_MultiLED[LEDData::FaceRight135][LEDData::RED]   = "Face/Led/Red/Right/135Deg/Actuator/Value";
  DCMPath_MultiLED[LEDData::FaceRight135][LEDData::GREEN] = "Face/Led/Green/Right/135Deg/Actuator/Value";
  DCMPath_MultiLED[LEDData::FaceRight135][LEDData::BLUE]  = "Face/Led/Blue/Right/135Deg/Actuator/Value";
  DCMPath_MultiLED[LEDData::FaceRight180][LEDData::RED]   = "Face/Led/Red/Right/180Deg/Actuator/Value";
  DCMPath_MultiLED[LEDData::FaceRight180][LEDData::GREEN] = "Face/Led/Green/Right/180Deg/Actuator/Value";
  DCMPath_MultiLED[LEDData::FaceRight180][LEDData::BLUE]  = "Face/Led/Blue/Right/180Deg/Actuator/Value";
  DCMPath_MultiLED[LEDData::FaceRight225][LEDData::RED]   = "Face/Led/Red/Right/225Deg/Actuator/Value";
  DCMPath_MultiLED[LEDData::FaceRight225][LEDData::GREEN] = "Face/Led/Green/Right/225Deg/Actuator/Value";
  DCMPath_MultiLED[LEDData::FaceRight225][LEDData::BLUE]  = "Face/Led/Blue/Right/225Deg/Actuator/Value";
  DCMPath_MultiLED[LEDData::FaceRight270][LEDData::RED]   = "Face/Led/Red/Right/270Deg/Actuator/Value";
  DCMPath_MultiLED[LEDData::FaceRight270][LEDData::GREEN] = "Face/Led/Green/Right/270Deg/Actuator/Value";
  DCMPath_MultiLED[LEDData::FaceRight270][LEDData::BLUE]  = "Face/Led/Blue/Right/270Deg/Actuator/Value";
  DCMPath_MultiLED[LEDData::FaceRight315][LEDData::RED]   = "Face/Led/Red/Right/315Deg/Actuator/Value";
  DCMPath_MultiLED[LEDData::FaceRight315][LEDData::GREEN] = "Face/Led/Green/Right/315Deg/Actuator/Value";
  DCMPath_MultiLED[LEDData::FaceRight315][LEDData::BLUE]  = "Face/Led/Blue/Right/315Deg/Actuator/Value";

  DCMPath_MultiLED[LEDData::FootLeft][LEDData::RED]       = "LFoot/Led/Red/Actuator/Value";
  DCMPath_MultiLED[LEDData::FootLeft][LEDData::GREEN]     = "LFoot/Led/Green/Actuator/Value";
  DCMPath_MultiLED[LEDData::FootLeft][LEDData::BLUE]      = "LFoot/Led/Blue/Actuator/Value";
  DCMPath_MultiLED[LEDData::FootRight][LEDData::RED]      = "RFoot/Led/Red/Actuator/Value";
  DCMPath_MultiLED[LEDData::FootRight][LEDData::GREEN]    = "RFoot/Led/Green/Actuator/Value";
  DCMPath_MultiLED[LEDData::FootRight][LEDData::BLUE]     = "RFoot/Led/Blue/Actuator/Value";

  DCMPath_MultiLED[LEDData::ChestButton][LEDData::RED]    = "ChestBoard/Led/Red/Actuator/Value";
  DCMPath_MultiLED[LEDData::ChestButton][LEDData::GREEN]  = "ChestBoard/Led/Green/Actuator/Value";
  DCMPath_MultiLED[LEDData::ChestButton][LEDData::BLUE]   = "ChestBoard/Led/Blue/Actuator/Value";

  try
  {
    ALValue aliasCommand;
    aliasCommand.arraySetSize(2);

    //AllMotorJoints
    aliasCommand[0] = string("AllLED");

    aliasCommand[1].arraySetSize(LEDData::numOfMonoLED + 3*LEDData::numOfMultiLED);
    for(int i=0;i<LEDData::numOfMonoLED;i++)
    {
      aliasCommand[1][i] = DCMPath_MonoLED[i];
    }
    for(int i=0;i<LEDData::numOfMultiLED;i++)
    {
      aliasCommand[1][LEDData::numOfMonoLED + 3*i] = DCMPath_MultiLED[i][LEDData::RED];
      aliasCommand[1][LEDData::numOfMonoLED + 3*i+1] = DCMPath_MultiLED[i][LEDData::GREEN];
      aliasCommand[1][LEDData::numOfMonoLED + 3*i+2] = DCMPath_MultiLED[i][LEDData::BLUE];
    }

    al_dcmproxy->createAlias(aliasCommand);
  }
  catch(ALError e)
  {
    std::cerr << "[NaoSMAL] Failed to create LED-Alias: " << e.toString() << endl;
  }

  ledCommands.arraySetSize(6);
  ledCommands[0] = string("AllLED");
  ledCommands[1] = string("ClearAll");
  ledCommands[2] = string("time-separate");
  ledCommands[3] = 0;
  ledCommands[4].arraySetSize(1);
  ledCommands[4][0] = 0;
  ledCommands[5].arraySetSize(LEDData::numOfMonoLED + 3*LEDData::numOfMultiLED);
  for(int i=0;i<LEDData::numOfMonoLED;i++)
  {
    ledCommands[5][i].arraySetSize(1);
    ledCommands[5][i][0] = 0.0;
  }
  for(int i=LEDData::numOfMonoLED;i<LEDData::numOfMonoLED + 3*LEDData::numOfMultiLED;i+=3)
  {
    ledCommands[5][i].arraySetSize(1);
    ledCommands[5][i][0] = 0.0;
    ledCommands[5][i+1].arraySetSize(1);
    ledCommands[5][i+1][0] = 0.0;
    ledCommands[5][i+2].arraySetSize(1);
    ledCommands[5][i+2][0] = 0.0;
  }
}//end initLED


void DCMHandler::initSingleLED()
{
  singleLedCommand.arraySetSize(3);
  singleLedCommand[1] = string("ClearAll");
  singleLedCommand[2].arraySetSize(1);
  singleLedCommand[2][0].arraySetSize(2);
  singleLedCommand[2][0][1] = 0;


  // set all requests to invalide values
  // this will force the function setSingleLED to set all 
  // values at the first call
  for(int i=0;i<LEDData::numOfMonoLED;i++)
  {
    lastLEDData.theMonoLED[i] = -1.0;
  }//end for

  for(int i=0;i<LEDData::numOfMultiLED;i++)
  {
    for(int k=0;k<LEDData::numOfLEDColor;k++)
    {
      lastLEDData.theMultiLED[i][k] = -1.0;
    }//end for
  }//end for
}//end initSingleLED


bool DCMHandler::setLEDSmart(const LEDData& data, int dcmTime)
{
  for(int i=0;i<LEDData::numOfMonoLED;i++)
  {
    if(lastLEDData.theMonoLED[i] != data.theMonoLED[i])
    {
      // copy all
      lastLEDData = data;
      setLED(data, dcmTime);
      return true;
    }
  }//end for

  for(int i=0;i<LEDData::numOfMultiLED;i++)
  {
    if( lastLEDData.theMultiLED[i][LEDData::RED]   != data.theMultiLED[i][LEDData::RED] ||
        lastLEDData.theMultiLED[i][LEDData::GREEN] != data.theMultiLED[i][LEDData::GREEN] ||
        lastLEDData.theMultiLED[i][LEDData::BLUE]  != data.theMultiLED[i][LEDData::BLUE])
    {
      // copy all
      lastLEDData = data;
      setLED(data, dcmTime);
      return true;
    }
  }//end for

  return false;
}//end setLEDSmart

bool DCMHandler::setSingleLED(const LEDData& data, int dcmTime)
{
  bool result = false;
  try
  {
    singleLedCommand[2][0][1] = dcmTime;

    for(int i=0;i<LEDData::numOfMonoLED;i++)
    {
      if(lastLEDData.theMonoLED[i] != data.theMonoLED[i])
      {
        result = true;
        singleLedCommand[0] = DCMPath_MonoLED[i];
        singleLedCommand[2][0][0] = lastLEDData.theMonoLED[i] = data.theMonoLED[i];
        al_dcmproxy->set(singleLedCommand);
      }//end if
    }//end for


    for(int i=0;i<LEDData::numOfMultiLED;i++)
    {
      for(int k=0;k<LEDData::numOfLEDColor;k++)
      {
        if(lastLEDData.theMultiLED[i][k] != data.theMultiLED[i][k])
        {
          result = true;
          singleLedCommand[0] = DCMPath_MultiLED[i][k];
          singleLedCommand[2][0][0] = lastLEDData.theMultiLED[i][k] = data.theMultiLED[i][k];
          al_dcmproxy->set(singleLedCommand);
        }//end if
      }//end for
    }//end for

  }
  catch(ALError e) {
    std::cerr << "[NaoSMAL] Failed to set LEDs: " << e.toString() << endl;
  }
  return result;
}//end setSingleLED


void DCMHandler::setLED(const LEDData& data, int dcmTime)
{
  ledCommands[4][0] = dcmTime;

  for(int i=0;i<LEDData::numOfMonoLED;i++)
  {
    ledCommands[5][i][0] = data.theMonoLED[i];
  }
  for(int i=0;i<LEDData::numOfMultiLED;i++)
  {
    ledCommands[5][LEDData::numOfMonoLED + 3*i][0] = data.theMultiLED[i][LEDData::RED];
    ledCommands[5][LEDData::numOfMonoLED + 3*i+1][0] = data.theMultiLED[i][LEDData::GREEN];
    ledCommands[5][LEDData::numOfMonoLED + 3*i+2][0] = data.theMultiLED[i][LEDData::BLUE];
  }
  try
  {
    al_dcmproxy->setAlias(ledCommands);
  }
  catch(ALError e) {
    std::cerr << "[NaoSMAL] Failed to set LEDs: " << e.toString() << endl;
  }
}//end setLED



void DCMHandler::initIRSend()
{
  DCMPath_IRSend[IRSendData::Beacon] =  "Device/SubDeviceList/IR/BeaconCode/Actuator/Value";
  DCMPath_IRSend[IRSendData::Byte1] = "Device/SubDeviceList/IR/RobotCode/Byte1/Actuator/Value";
  DCMPath_IRSend[IRSendData::Byte2] = "Device/SubDeviceList/IR/RobotCode/Byte2/Actuator/Value";
  DCMPath_IRSend[IRSendData::Byte3] = "Device/SubDeviceList/IR/RobotCode/Byte3/Actuator/Value";
  DCMPath_IRSend[IRSendData::Byte4] = "Device/SubDeviceList/IR/RobotCode/Byte4/Actuator/Value";
  DCMPath_IRSend[IRSendData::RCByte1] = "Device/SudDeviceList/IR/RC5Code/Byte1/Actuator/Value";
  DCMPath_IRSend[IRSendData::RCByte2] = "Device/SubDeviceList/IR/RC5Code/Byte2/Actuator/Value";

  try
  {
    ALValue aliasCommand;
    aliasCommand.arraySetSize(2);

    //AllMotorJoints
    aliasCommand[0] = string("AllIR");
    aliasCommand[1].arraySetSize(IRSendData::numOfIRSend);
    for(int i=0;i<IRSendData::numOfIRSend;i++)
    {
      aliasCommand[1][i] = DCMPath_IRSend[i];
    }
    al_dcmproxy->createAlias(aliasCommand);
  }
  catch(ALError e)
  {
    std::cerr << "[NaoSMAL] Failed to create IR-Alias: " << e.toString() << endl;
  }

  irCommands.arraySetSize(6);
  irCommands[0] = string("AllIR");
  irCommands[1] = string("ClearAll");
  irCommands[2] = string("time-separate");
  irCommands[3] = 0;
  irCommands[4].arraySetSize(1);
  irCommands[4][0] = 0;
  irCommands[5].arraySetSize(IRSendData::numOfIRSend);
  for(int i=0;i<IRSendData::numOfIRSend;i++)
  {
    irCommands[5][i].arraySetSize(1);
    irCommands[5][i][0] = 0.0;
  }
}//end initIRSend

void DCMHandler::setIRSend(const IRSendData& data, int dcmTime)
{
  if ( !data.changed ) return;
  
  irCommands[4][0] = dcmTime;

  for(int i=0;i<IRSendData::numOfIRSend;i++)
  {
    irCommands[5][i][0] = data.data[i];
  }
  try
  {
    al_dcmproxy->setAlias(irCommands);
  }
  catch(ALError e) {
    std::cerr << "[NaoSMAL] Failed to set IR: " << e.toString() << endl;
  }
}//end setIRSend


void DCMHandler::initUltraSoundSend()
{
  DCMPath_UltraSoundSend = "Device/SubDeviceList/US/Actuator/Value";

  try
  {
    ALValue aliasCommand;
    aliasCommand.arraySetSize(2);

    // create alias
    aliasCommand[0] = string("USSend");
    aliasCommand[1].arraySetSize(1);
    aliasCommand[1][0] = DCMPath_UltraSoundSend;

    al_dcmproxy->createAlias(aliasCommand);

    // prepare the request
    usSendCommands.arraySetSize(6);
    usSendCommands[0] = string("USSend");
    usSendCommands[1] = string("Merge");
    usSendCommands[2] = string("time-separate");
    usSendCommands[3] = 0;
    usSendCommands[4].arraySetSize(1);
    usSendCommands[4][0] = 0;
    usSendCommands[5].arraySetSize(1);
  
    usSendCommands[5][0].arraySetSize(1);
    usSendCommands[5][0][0] = 0.0;
  }
  catch(ALError e)
  {
    std::cerr << "[NaoSMAL] Failed to create UltraSound-Alias: " << e.toString() << endl;
  }
}//end initUltraSoundSend

void DCMHandler::setUltraSoundSend(const UltraSoundSendData& data, int dcmTime)
{
  usSendCommands[4][0] = dcmTime;
  usSendCommands[5][0][0] = static_cast<double>(data.mode);

  try
  {
    al_dcmproxy->setAlias(usSendCommands);
  }
  catch(ALError e) {
    std::cerr << "[NaoSMAL] Failed to set UltraSound: " << e.toString() << endl;
  }
}//end setUltraSoundSend

