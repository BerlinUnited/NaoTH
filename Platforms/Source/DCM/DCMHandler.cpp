#include "DCMHandler.h"
#include <Tools/NaoInfo.h>

using namespace naoth;

DCMHandler::DCMHandler()
{}

DCMHandler::~DCMHandler()
{}

void DCMHandler::init(ALPtr<ALBroker> pB)
{
  pBroker = pB;

  time_delay = 0;

  //connect to DCM
  try
  {
    al_dcmproxy = new DCMProxy(pB);
    al_memory = pB->getMemoryProxy();
  }
  catch(ALError e) {
    std::cerr << "Failed to init DCMHandler: " << e.toString() << endl;
  }

  //Generate all strings for interaction
  initFSR();
  initSensorJoint();
  initMotorJoint();
  initLED();
  initAccelerometer();
  initGyrometer();
  initInertialSensor();
  initButton();
  initIRReceive();
  initIRSend();
  initUltraSoundReceive();
  initUltraSoundSend();

  DCMPath_BodyId = "Device/DeviceList/ChestBoard/BodyId";
  DCMPath_BodyNickName = "Device/DeviceList/ChestBoard/BodyNickName";
  DCMPath_BatteryCharge = "Device/SubDeviceList/Battery/Charge/Sensor/Value";

  initAllSensorData();

}

string DCMHandler::getBodyID()
{
  return getFromALMemory(DCMPath_BodyId);
}

string DCMHandler::getBodyNickName()
{
  return getFromALMemory(DCMPath_BodyNickName);
}

/*
 * function for retrieving a single value from ALMemory
 * For multiple data use al_memory->getListData() directly
 */
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
}

/*
 * function for sending a single value to DCM
 * For multiple data use Alias together with DCM directly
 */
inline void DCMHandler::sendToDCM(const string path,const double value,const int timestamp)
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
}

//fsr
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
}

//SensorJoint
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
}

//MotorJoint
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
    std::cerr << "Failed to init MotorJointAlias: " << e.toString() << endl;
  }
}

void DCMHandler::setSingleMotorData(const JointData::JointID jointID,const MotorJointData *theMotorJointData)
{
  unsigned int timestamp = al_dcmproxy->getTime(time_delay);
  sendToDCM(DCMPath_MotorJointHardness[jointID],theMotorJointData->hardness[jointID],timestamp);
  sendToDCM(DCMPath_MotorJointPosition[jointID],theMotorJointData->position[jointID],timestamp);
}

void DCMHandler::setAllMotorData(const MotorJointData& theMotorJointData)
{
  int currentAbsDelay = al_dcmproxy->getTime(time_delay);
  allMotorPositionCommands[4][0] = currentAbsDelay;
  allMotorHardnessCommands[4][0] = currentAbsDelay;

  //MotorJoints
  for(int i=0;i<JointData::RHipYawPitch;i++)
  {
    allMotorHardnessCommands[5][i][0] = theMotorJointData.hardness[i];
    allMotorPositionCommands[5][i][0] = theMotorJointData.position[i];
  }
  for(int i=JointData::RHipYawPitch+1;i<JointData::numOfJoint;i++)
  {
    allMotorHardnessCommands[5][i-1][0] = theMotorJointData.hardness[i];
    allMotorPositionCommands[5][i-1][0] = theMotorJointData.position[i];
  }

  try
  {
    al_dcmproxy->setAlias(allMotorPositionCommands);
    al_dcmproxy->setAlias(allMotorHardnessCommands);
  }
  catch(ALError e) {
    std::cerr << "Failed to set AllMotorJoints: " << e.toString() << endl;
  }
}

//LED
void DCMHandler::initLED()
{
  DCMPath_MonoLED[LEDData::EarLeft0] = "Device/SubDeviceList/Ears/Led/Left/0Deg/Actuator/Value";
  DCMPath_MonoLED[LEDData::EarLeft36] = "Device/SubDeviceList/Ears/Led/Left/36Deg/Actuator/Value";
  DCMPath_MonoLED[LEDData::EarLeft72] = "Device/SubDeviceList/Ears/Led/Left/72Deg/Actuator/Value";
  DCMPath_MonoLED[LEDData::EarLeft108] = "Device/SubDeviceList/Ears/Led/Left/108Deg/Actuator/Value";
  DCMPath_MonoLED[LEDData::EarLeft144] = "Device/SubDeviceList/Ears/Led/Left/144Deg/Actuator/Value";
  DCMPath_MonoLED[LEDData::EarLeft180] = "Device/SubDeviceList/Ears/Led/Left/180Deg/Actuator/Value";
  DCMPath_MonoLED[LEDData::EarLeft216] = "Device/SubDeviceList/Ears/Led/Left/216Deg/Actuator/Value";
  DCMPath_MonoLED[LEDData::EarLeft252] = "Device/SubDeviceList/Ears/Led/Left/252Deg/Actuator/Value";
  DCMPath_MonoLED[LEDData::EarLeft288] = "Device/SubDeviceList/Ears/Led/Left/288Deg/Actuator/Value";
  DCMPath_MonoLED[LEDData::EarLeft324] = "Device/SubDeviceList/Ears/Led/Left/324Deg/Actuator/Value";
  DCMPath_MonoLED[LEDData::EarRight0] = "Device/SubDeviceList/Ears/Led/Right/0Deg/Actuator/Value";
  DCMPath_MonoLED[LEDData::EarRight36] = "Device/SubDeviceList/Ears/Led/Right/36Deg/Actuator/Value";
  DCMPath_MonoLED[LEDData::EarRight72] = "Device/SubDeviceList/Ears/Led/Right/72Deg/Actuator/Value";
  DCMPath_MonoLED[LEDData::EarRight108] = "Device/SubDeviceList/Ears/Led/Right/108Deg/Actuator/Value";
  DCMPath_MonoLED[LEDData::EarRight144] = "Device/SubDeviceList/Ears/Led/Right/144Deg/Actuator/Value";
  DCMPath_MonoLED[LEDData::EarRight180] = "Device/SubDeviceList/Ears/Led/Right/180Deg/Actuator/Value";
  DCMPath_MonoLED[LEDData::EarRight216] = "Device/SubDeviceList/Ears/Led/Right/216Deg/Actuator/Value";
  DCMPath_MonoLED[LEDData::EarRight252] = "Device/SubDeviceList/Ears/Led/Right/252Deg/Actuator/Value";
  DCMPath_MonoLED[LEDData::EarRight288] = "Device/SubDeviceList/Ears/Led/Right/288Deg/Actuator/Value";
  DCMPath_MonoLED[LEDData::EarRight324] = "Device/SubDeviceList/Ears/Led/Right/324Deg/Actuator/Value";

  DCMPath_MultiLED[LEDData::FaceLeft0][LEDData::RED] = "Device/SubDeviceList/Face/Led/Red/Left/0Deg/Actuator/Value";
  DCMPath_MultiLED[LEDData::FaceLeft0][LEDData::GREEN] = "Device/SubDeviceList/Face/Led/Green/Left/0Deg/Actuator/Value";
  DCMPath_MultiLED[LEDData::FaceLeft0][LEDData::BLUE] = "Device/SubDeviceList/Face/Led/Blue/Left/0Deg/Actuator/Value";
  DCMPath_MultiLED[LEDData::FaceLeft45][LEDData::RED] = "Device/SubDeviceList/Face/Led/Red/Left/45Deg/Actuator/Value";
  DCMPath_MultiLED[LEDData::FaceLeft45][LEDData::GREEN] = "Device/SubDeviceList/Face/Led/Green/Left/45Deg/Actuator/Value";
  DCMPath_MultiLED[LEDData::FaceLeft45][LEDData::BLUE] = "Device/SubDeviceList/Face/Led/Blue/Left/45Deg/Actuator/Value";
  DCMPath_MultiLED[LEDData::FaceLeft90][LEDData::RED] = "Device/SubDeviceList/Face/Led/Red/Left/90Deg/Actuator/Value";
  DCMPath_MultiLED[LEDData::FaceLeft90][LEDData::GREEN] = "Device/SubDeviceList/Face/Led/Green/Left/90Deg/Actuator/Value";
  DCMPath_MultiLED[LEDData::FaceLeft90][LEDData::BLUE] = "Device/SubDeviceList/Face/Led/Blue/Left/90Deg/Actuator/Value";
  DCMPath_MultiLED[LEDData::FaceLeft135][LEDData::RED] = "Device/SubDeviceList/Face/Led/Red/Left/135Deg/Actuator/Value";
  DCMPath_MultiLED[LEDData::FaceLeft135][LEDData::GREEN] = "Device/SubDeviceList/Face/Led/Green/Left/135Deg/Actuator/Value";
  DCMPath_MultiLED[LEDData::FaceLeft135][LEDData::BLUE] = "Device/SubDeviceList/Face/Led/Blue/Left/135Deg/Actuator/Value";
  DCMPath_MultiLED[LEDData::FaceLeft180][LEDData::RED] = "Device/SubDeviceList/Face/Led/Red/Left/180Deg/Actuator/Value";
  DCMPath_MultiLED[LEDData::FaceLeft180][LEDData::GREEN] = "Device/SubDeviceList/Face/Led/Green/Left/180Deg/Actuator/Value";
  DCMPath_MultiLED[LEDData::FaceLeft180][LEDData::BLUE] = "Device/SubDeviceList/Face/Led/Blue/Left/180Deg/Actuator/Value";
  DCMPath_MultiLED[LEDData::FaceLeft225][LEDData::RED] = "Device/SubDeviceList/Face/Led/Red/Left/225Deg/Actuator/Value";
  DCMPath_MultiLED[LEDData::FaceLeft225][LEDData::GREEN] = "Device/SubDeviceList/Face/Led/Green/Left/225Deg/Actuator/Value";
  DCMPath_MultiLED[LEDData::FaceLeft225][LEDData::BLUE] = "Device/SubDeviceList/Face/Led/Blue/Left/225Deg/Actuator/Value";
  DCMPath_MultiLED[LEDData::FaceLeft270][LEDData::RED] = "Device/SubDeviceList/Face/Led/Red/Left/270Deg/Actuator/Value";
  DCMPath_MultiLED[LEDData::FaceLeft270][LEDData::GREEN] = "Device/SubDeviceList/Face/Led/Green/Left/270Deg/Actuator/Value";
  DCMPath_MultiLED[LEDData::FaceLeft270][LEDData::BLUE] = "Device/SubDeviceList/Face/Led/Blue/Left/270Deg/Actuator/Value";
  DCMPath_MultiLED[LEDData::FaceLeft315][LEDData::RED] = "Device/SubDeviceList/Face/Led/Red/Left/315Deg/Actuator/Value";
  DCMPath_MultiLED[LEDData::FaceLeft315][LEDData::GREEN] = "Device/SubDeviceList/Face/Led/Green/Left/315Deg/Actuator/Value";
  DCMPath_MultiLED[LEDData::FaceLeft315][LEDData::BLUE] = "Device/SubDeviceList/Face/Led/Blue/Left/315Deg/Actuator/Value";

  DCMPath_MultiLED[LEDData::FaceRight0][LEDData::RED] = "Device/SubDeviceList/Face/Led/Red/Right/0Deg/Actuator/Value";
  DCMPath_MultiLED[LEDData::FaceRight0][LEDData::GREEN] = "Device/SubDeviceList/Face/Led/Green/Right/0Deg/Actuator/Value";
  DCMPath_MultiLED[LEDData::FaceRight0][LEDData::BLUE] = "Device/SubDeviceList/Face/Led/Blue/Right/0Deg/Actuator/Value";
  DCMPath_MultiLED[LEDData::FaceRight45][LEDData::RED] = "Device/SubDeviceList/Face/Led/Red/Right/45Deg/Actuator/Value";
  DCMPath_MultiLED[LEDData::FaceRight45][LEDData::GREEN] = "Device/SubDeviceList/Face/Led/Green/Right/45Deg/Actuator/Value";
  DCMPath_MultiLED[LEDData::FaceRight45][LEDData::BLUE] = "Device/SubDeviceList/Face/Led/Blue/Right/45Deg/Actuator/Value";
  DCMPath_MultiLED[LEDData::FaceRight90][LEDData::RED] = "Device/SubDeviceList/Face/Led/Red/Right/90Deg/Actuator/Value";
  DCMPath_MultiLED[LEDData::FaceRight90][LEDData::GREEN] = "Device/SubDeviceList/Face/Led/Green/Right/90Deg/Actuator/Value";
  DCMPath_MultiLED[LEDData::FaceRight90][LEDData::BLUE] = "Device/SubDeviceList/Face/Led/Blue/Right/90Deg/Actuator/Value";
  DCMPath_MultiLED[LEDData::FaceRight135][LEDData::RED] = "Device/SubDeviceList/Face/Led/Red/Right/135Deg/Actuator/Value";
  DCMPath_MultiLED[LEDData::FaceRight135][LEDData::GREEN] = "Device/SubDeviceList/Face/Led/Green/Right/135Deg/Actuator/Value";
  DCMPath_MultiLED[LEDData::FaceRight135][LEDData::BLUE] = "Device/SubDeviceList/Face/Led/Blue/Right/135Deg/Actuator/Value";
  DCMPath_MultiLED[LEDData::FaceRight180][LEDData::RED] = "Device/SubDeviceList/Face/Led/Red/Right/180Deg/Actuator/Value";
  DCMPath_MultiLED[LEDData::FaceRight180][LEDData::GREEN] = "Device/SubDeviceList/Face/Led/Green/Right/180Deg/Actuator/Value";
  DCMPath_MultiLED[LEDData::FaceRight180][LEDData::BLUE] = "Device/SubDeviceList/Face/Led/Blue/Right/180Deg/Actuator/Value";
  DCMPath_MultiLED[LEDData::FaceRight225][LEDData::RED] = "Device/SubDeviceList/Face/Led/Red/Right/225Deg/Actuator/Value";
  DCMPath_MultiLED[LEDData::FaceRight225][LEDData::GREEN] = "Device/SubDeviceList/Face/Led/Green/Right/225Deg/Actuator/Value";
  DCMPath_MultiLED[LEDData::FaceRight225][LEDData::BLUE] = "Device/SubDeviceList/Face/Led/Blue/Right/225Deg/Actuator/Value";
  DCMPath_MultiLED[LEDData::FaceRight270][LEDData::RED] = "Device/SubDeviceList/Face/Led/Red/Right/270Deg/Actuator/Value";
  DCMPath_MultiLED[LEDData::FaceRight270][LEDData::GREEN] = "Device/SubDeviceList/Face/Led/Green/Right/270Deg/Actuator/Value";
  DCMPath_MultiLED[LEDData::FaceRight270][LEDData::BLUE] = "Device/SubDeviceList/Face/Led/Blue/Right/270Deg/Actuator/Value";
  DCMPath_MultiLED[LEDData::FaceRight315][LEDData::RED] = "Device/SubDeviceList/Face/Led/Red/Right/315Deg/Actuator/Value";
  DCMPath_MultiLED[LEDData::FaceRight315][LEDData::GREEN] = "Device/SubDeviceList/Face/Led/Green/Right/315Deg/Actuator/Value";
  DCMPath_MultiLED[LEDData::FaceRight315][LEDData::BLUE] = "Device/SubDeviceList/Face/Led/Blue/Right/315Deg/Actuator/Value";

  DCMPath_MultiLED[LEDData::FootLeft][LEDData::RED] = "Device/SubDeviceList/LFoot/Led/Red/Actuator/Value";
  DCMPath_MultiLED[LEDData::FootLeft][LEDData::GREEN] = "Device/SubDeviceList/LFoot/Led/Green/Actuator/Value";
  DCMPath_MultiLED[LEDData::FootLeft][LEDData::BLUE] = "Device/SubDeviceList/LFoot/Led/Blue/Actuator/Value";
  DCMPath_MultiLED[LEDData::FootRight][LEDData::RED] = "Device/SubDeviceList/RFoot/Led/Red/Actuator/Value";
  DCMPath_MultiLED[LEDData::FootRight][LEDData::GREEN] = "Device/SubDeviceList/RFoot/Led/Green/Actuator/Value";
  DCMPath_MultiLED[LEDData::FootRight][LEDData::BLUE] = "Device/SubDeviceList/RFoot/Led/Blue/Actuator/Value";
  DCMPath_MultiLED[LEDData::ChestButton][LEDData::RED] = "Device/SubDeviceList/ChestBoard/Led/Red/Actuator/Value";
  DCMPath_MultiLED[LEDData::ChestButton][LEDData::GREEN] = "Device/SubDeviceList/ChestBoard/Led/Green/Actuator/Value";
  DCMPath_MultiLED[LEDData::ChestButton][LEDData::BLUE] = "Device/SubDeviceList/ChestBoard/Led/Blue/Actuator/Value";

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
    std::cerr << "Failed to create LED-Alias: " << e.toString() << endl;
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
}

void DCMHandler::setLED(const LEDData& theLEDData)
{  

  
  ledCommands[4][0] = al_dcmproxy->getTime(time_delay);

  for(int i=0;i<LEDData::numOfMonoLED;i++)
  {
    ledCommands[5][i][0] = theLEDData.theMonoLED[i];
  }
  for(int i=0;i<LEDData::numOfMultiLED;i++)
  {
    ledCommands[5][LEDData::numOfMonoLED + 3*i][0] = theLEDData.theMultiLED[i][LEDData::RED];
    ledCommands[5][LEDData::numOfMonoLED + 3*i+1][0] = theLEDData.theMultiLED[i][LEDData::GREEN];
    ledCommands[5][LEDData::numOfMonoLED + 3*i+2][0] = theLEDData.theMultiLED[i][LEDData::BLUE];
  }
  try
  {
    al_dcmproxy->setAlias(ledCommands);
  }
  catch(ALError e) {
    std::cerr << "Failed to set LEDs: " << e.toString() << endl;
  }
}

void DCMHandler::initAccelerometer()
{
  for(int i=0;i<AccelerometerData::numOfAccelerometer;i++)
  {
    DCMPath_Accelerometer[i] = "Device/SubDeviceList/InertialSensor/Acc"
                                + AccelerometerData::getAccelerometerName((AccelerometerData::AccelerometerID) i)
                                + "/Sensor/Value";
  }
}

void DCMHandler::initGyrometer()
{
  for(int i=0;i<GyrometerData::numOfGyrometer;i++)
  {
    DCMPath_Gyrometer[i] = "Device/SubDeviceList/InertialSensor/Gyr"
                           + GyrometerData::getGyrometerName((GyrometerData::GyrometerID) i)
                           + "/Sensor/Value";
  }
  DCMPath_Gyrometer[GyrometerData::numOfGyrometer] = "Device/SubDeviceList/InertialSensor/GyrRef/Sensor/Value";
}

void DCMHandler::initInertialSensor()
{
  for(int i=0;i<InertialSensorData::numOfInertialSensor;i++)
  {
    DCMPath_InertialSensor[i] = "Device/SubDeviceList/InertialSensor/Angle"
                                + InertialSensorData::getInertialSensorName((InertialSensorData::InertialSensorID) i)
                                + "/Sensor/Value";
  }
}

void DCMHandler::initButton()
{
  DCMPath_Button[ButtonData::Chest] = "Device/SubDeviceList/ChestBoard/Button/Sensor/Value";
  DCMPath_Button[ButtonData::LeftFootLeft] = "Device/SubDeviceList/LFoot/Bumper/Left/Sensor/Value";
  DCMPath_Button[ButtonData::LeftFootRight] = "Device/SubDeviceList/LFoot/Bumper/Right/Sensor/Value";
  DCMPath_Button[ButtonData::RightFootLeft] = "Device/SubDeviceList/RFoot/Bumper/Left/Sensor/Value";
  DCMPath_Button[ButtonData::RightFootRight] = "Device/SubDeviceList/RFoot/Bumper/Right/Sensor/Value";
}

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
}

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
    std::cerr << "Failed to create IR-Alias: " << e.toString() << endl;
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
}

void DCMHandler::setIRSend(const IRSendData& theIRSendData)
{
  irCommands[4][0] = al_dcmproxy->getTime(time_delay);

  for(int i=0;i<IRSendData::numOfIRSend;i++)
  {
    irCommands[5][i][0] = theIRSendData.data[i];
  }
  try
  {
    al_dcmproxy->setAlias(irCommands);
  }
  catch(ALError e) {
    std::cerr << "Failed to set IR: " << e.toString() << endl;
  }
}

void DCMHandler::initUltraSoundReceive()
{
  DCMPath_UltraSoundReceive = "Device/SubDeviceList/US/Sensor/Value";
  DCMPath_UltraSoundReceiveLeft[UltraSoundData::distance_1] = "Device/SubDeviceList/US/Left/Sensor/Value1";
  DCMPath_UltraSoundReceiveLeft[UltraSoundData::distance_2] = "Device/SubDeviceList/US/Left/Sensor/Value2";
  DCMPath_UltraSoundReceiveLeft[UltraSoundData::distance_3] = "Device/SubDeviceList/US/Left/Sensor/Value3";
  DCMPath_UltraSoundReceiveLeft[UltraSoundData::distance_4] = "Device/SubDeviceList/US/Left/Sensor/Value4";
  DCMPath_UltraSoundReceiveLeft[UltraSoundData::distance_5] = "Device/SubDeviceList/US/Left/Sensor/Value5";
  DCMPath_UltraSoundReceiveLeft[UltraSoundData::distance_6] = "Device/SubDeviceList/US/Left/Sensor/Value6";
  DCMPath_UltraSoundReceiveLeft[UltraSoundData::distance_7] = "Device/SubDeviceList/US/Left/Sensor/Value7";
  DCMPath_UltraSoundReceiveLeft[UltraSoundData::distance_8] = "Device/SubDeviceList/US/Left/Sensor/Value8";
  DCMPath_UltraSoundReceiveLeft[UltraSoundData::distance_9] = "Device/SubDeviceList/US/Left/Sensor/Value9";
  DCMPath_UltraSoundReceiveRight[UltraSoundData::distance_1] = "Device/SubDeviceList/US/Right/Sensor/Value1";
  DCMPath_UltraSoundReceiveRight[UltraSoundData::distance_2] = "Device/SubDeviceList/US/Right/Sensor/Value2";
  DCMPath_UltraSoundReceiveRight[UltraSoundData::distance_3] = "Device/SubDeviceList/US/Right/Sensor/Value3";
  DCMPath_UltraSoundReceiveRight[UltraSoundData::distance_4] = "Device/SubDeviceList/US/Right/Sensor/Value4";
  DCMPath_UltraSoundReceiveRight[UltraSoundData::distance_5] = "Device/SubDeviceList/US/Right/Sensor/Value5";
  DCMPath_UltraSoundReceiveRight[UltraSoundData::distance_6] = "Device/SubDeviceList/US/Right/Sensor/Value6";
  DCMPath_UltraSoundReceiveRight[UltraSoundData::distance_7] = "Device/SubDeviceList/US/Right/Sensor/Value7";
  DCMPath_UltraSoundReceiveRight[UltraSoundData::distance_8] = "Device/SubDeviceList/US/Right/Sensor/Value8";
  DCMPath_UltraSoundReceiveRight[UltraSoundData::distance_9] = "Device/SubDeviceList/US/Right/Sensor/Value9";
}

void DCMHandler::initUltraSoundSend()
{
   DCMPath_UltraSoundSend = "Device/SubDeviceList/US/Actuator/Value";

   try
  {
    ALValue aliasCommand;
    aliasCommand.arraySetSize(2);

    //AllMotorJoints
    aliasCommand[0] = string("USSend");
    aliasCommand[1].arraySetSize(1);
    aliasCommand[1][0] = DCMPath_UltraSoundSend;

    al_dcmproxy->createAlias(aliasCommand);
  }
  catch(ALError e)
  {
    std::cerr << "Failed to create UltraSound-Alias: " << e.toString() << endl;
  }

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

void DCMHandler::setUltraSoundSend(const UltraSoundSendData& data)
{
  usSendCommands[4][0] = al_dcmproxy->getTime(time_delay);
  usSendCommands[5][0][0] = data.data;

  try
  {
    al_dcmproxy->setAlias(usSendCommands);
  }
  catch(ALError e) {
    std::cerr << "Failed to set UltraSound: " << e.toString() << endl;
  }
}

void DCMHandler::initAllSensorData()
{
  int numElements = 4 * (JointData::numOfJoint - 1)
                  + FSRData::numOfFSR
                  + AccelerometerData::numOfAccelerometer
                  + GyrometerData::numOfGyrometer + 1
                  + InertialSensorData::numOfInertialSensor
                  + IRReceiveData::numOfIRReceive
                  + ButtonData::numOfButtons
                  + 1 + 2 * UltraSoundData::numOfIRSend//3 // ultrasound
                  + 1; // ???

  allSensorsList.resize(numElements);
  currentAllSensorsValue.resize(numElements);
  
  int currentIndex=0;

  //SensorJointData
  theSensorJointDataIndex = currentIndex;
  for(int i=0;i<JointData::RHipYawPitch;i++)
  {
    allSensorsList[currentIndex++] = DCMPath_SensorJointElectricCurrent[i];
    allSensorsList[currentIndex++] = DCMPath_SensorJointTemperature[i];
    allSensorsList[currentIndex++] = DCMPath_SensorJointPosition[i];
    allSensorsList[currentIndex++] = DCMPath_MotorJointHardness[i];
  }

  for(int i=JointData::RHipYawPitch+1;i<JointData::numOfJoint;i++)
  {
    allSensorsList[currentIndex++] = DCMPath_SensorJointElectricCurrent[i];
    allSensorsList[currentIndex++] = DCMPath_SensorJointTemperature[i];
    allSensorsList[currentIndex++] = DCMPath_SensorJointPosition[i];
    allSensorsList[currentIndex++] = DCMPath_MotorJointHardness[i];
  }

  //FSRData
  theFSRDataIndex = currentIndex;
  for(int i=0;i<FSRData::numOfFSR;i++)
  {
    allSensorsList[currentIndex++] = DCMPath_FSR[i];
  }

  //AccelerometerData
  theAccelerometerDataIndex = currentIndex;
  for(int i=0;i<AccelerometerData::numOfAccelerometer;i++)
  {
    allSensorsList[currentIndex++] = DCMPath_Accelerometer[i];
  }

  //GyrometerData
  theGyrometerDataIndex = currentIndex;
  for(int i=0;i<GyrometerData::numOfGyrometer+1;i++)
  {
    allSensorsList[currentIndex++] = DCMPath_Gyrometer[i];
  }

  //InertialSensorsData
  theInertialSensorDataIndex = currentIndex;
  for(int i=0;i<InertialSensorData::numOfInertialSensor;i++)
  {
    allSensorsList[currentIndex++] = DCMPath_InertialSensor[i];
  }

  //IRReceiveData
  theIRReceiveDataIndex = currentIndex;
  for(int i=0;i<IRReceiveData::numOfIRReceive;i++)
  {
    allSensorsList[currentIndex++] = DCMPath_IRReceive[i];
  }

  //ButtonData
  theButtonDataIndex = currentIndex;
  for(int i=0;i<ButtonData::numOfButtons;i++)
  {
    allSensorsList[currentIndex++] = DCMPath_Button[i];
  }

  //UltraSoundReceiveData
  theUltraSoundReceiveDataIndex = currentIndex;
  allSensorsList[currentIndex++] = DCMPath_UltraSoundReceive; 
  for(int i = 0; i < UltraSoundData::numOfIRSend; i++)
  {
    allSensorsList[currentIndex++] = DCMPath_UltraSoundReceiveLeft[i];
    allSensorsList[currentIndex++] = DCMPath_UltraSoundReceiveRight[i];
  }

  thBatteryDataIdex = currentIndex;
  allSensorsList[currentIndex++] = DCMPath_BatteryCharge;

  //connect variables
  al_memoryfast.ConnectToVariables(pBroker,allSensorsList,false);
}

void DCMHandler::getData()
{
  currentTimestamp = al_dcmproxy->getTime(time_delay);
  al_memoryfast.GetValues(currentAllSensorsValue);
}

void DCMHandler::get(SensorJointData& data) const
{
  try {
    unsigned int currentIndex = theSensorJointDataIndex;
    for (int i = 0; i < JointData::RHipYawPitch; i++) {
      data.electricCurrent[i] = currentAllSensorsValue[currentIndex++];
      data.temperature[i] = currentAllSensorsValue[currentIndex++];
      data.position[i] = currentAllSensorsValue[currentIndex++];
      data.hardness[i] = currentAllSensorsValue[currentIndex++];
    }
    for (int i = JointData::RHipYawPitch + 1; i < JointData::numOfJoint; i++) {
      data.electricCurrent[i] = currentAllSensorsValue[currentIndex++];
      data.temperature[i] = currentAllSensorsValue[currentIndex++];
      data.position[i] = currentAllSensorsValue[currentIndex++];
      data.hardness[i] = currentAllSensorsValue[currentIndex++];
    }
    data.electricCurrent[JointData::RHipYawPitch] = data.electricCurrent[JointData::LHipYawPitch];
    data.temperature[JointData::RHipYawPitch] = data.temperature[JointData::LHipYawPitch];
    data.position[JointData::RHipYawPitch] = data.position[JointData::LHipYawPitch];
    data.hardness[JointData::RHipYawPitch] = data.hardness[JointData::LHipYawPitch];
  }  catch (ALError e) {
    std::cerr << "Failed to get Information from: " << e.toString() << endl;
  }
}

void DCMHandler::get(FSRData& data) const
{
  try {
    unsigned int currentIndex = theFSRDataIndex;
    for (int i = 0; i < FSRData::numOfFSR; i++) {
      data.data[i] = currentAllSensorsValue[currentIndex++];
      data.force[i] = data.data[i] * 9.81; // The value returned for each FSR is similar to Kg in NaoQi 1.3.17
      data.valid[i] = ( data.force[i] >= NaoInfo::FSRMinForce && data.force[i] <= NaoInfo::FSRMaxForce );
    }
  } catch (ALError e) {
    std::cerr << "Failed to get Information from: " << e.toString() << endl;
  }
}

void DCMHandler::get(AccelerometerData& data) const
{
  try{
    //forum of Aldebaran scale = g/56.0
    //experiments Nao36 scale = g/60.0
    //wrong sign in Aldebaran
    static float scale_acc = -9.81/60.0;

    unsigned int currentIndex = theAccelerometerDataIndex;
    for (int i = 0; i < AccelerometerData::numOfAccelerometer; i++) {
      data.rawData[i] = currentAllSensorsValue[currentIndex++];
      //0.1532289 = 9.80665/64
      data.data[i] = data.rawData[i] * scale_acc;//* 0.1532289;
    }
  } catch (ALError e) {
    std::cerr << "Failed to get Information from: " << e.toString() << endl;
  }
}

void DCMHandler::get(GyrometerData& data) const
{
  try {
    //data = (raw-zero) * 2.7 * PI/180 [rad/s]
    static float scale_gyro = 2.7 * M_PI/180.0;

    unsigned int currentIndex = theGyrometerDataIndex;
    for (int i = 0; i < GyrometerData::numOfGyrometer + 1; i++) {
      data.rawData[i] = currentAllSensorsValue[currentIndex++];
    }
    for (int i = 0; i < GyrometerData::numOfGyrometer; i++) {
      data.data[i] = (data.rawData[i] - data.rawData[2]) * scale_gyro;
    }
  } catch (ALError e) {
    std::cerr << "Failed to get Information from: " << e.toString() << endl;
  }
}

void DCMHandler::get(InertialSensorData& data) const
{
  try {
    unsigned int currentIndex = theInertialSensorDataIndex;
    for (int i = 0; i < InertialSensorData::numOfInertialSensor; i++)
    {
      data.data[i] = currentAllSensorsValue[currentIndex++] + data.offset[i];
    }
  } catch (ALError e) {
    std::cerr << "Failed to get Information from: " << e.toString() << endl;
  }
}

void DCMHandler::get(IRReceiveData& data) const
{
  try {
    unsigned int currentIndex = theIRReceiveDataIndex;
    for (int i = 0; i < IRReceiveData::numOfIRReceive; i++) {
      data.data[i] = currentAllSensorsValue[currentIndex++];
    }
  } catch (ALError e) {
    std::cerr << "Failed to get Information from: " << e.toString() << endl;
  }
}

void DCMHandler::get(ButtonData& data) const
{
  try{
    unsigned int currentIndex = theButtonDataIndex;
    for (int i = 0; i < ButtonData::numOfButtons; i++) {
      float temp = currentAllSensorsValue[currentIndex++];
      bool wasAlreadPressed = data.isPressed[i];
      if (temp == 1.0f) {
        data.isPressed[i] = true;
        data.numOfFramesPressed[i]++;
        if (!wasAlreadPressed) {
          data.eventCounter[i]++;
        }
      } else {
        data.isPressed[i] = false;
        data.numOfFramesPressed[i] = 0;
      }
    }
  } catch (ALError e) {
    std::cerr << "Failed to get Information from: " << e.toString() << endl;
  }
}

void DCMHandler::get(UltraSoundReceiveData& data) const
{
  try
  {
    unsigned int currentIndex = theUltraSoundReceiveDataIndex;
    if(data.ultraSoundTimeStep != 100) //Hack:is only 100 if mode 4 or 12 etc were the third bit is set
    {
      data.data = currentAllSensorsValue[currentIndex++];
      currentIndex += UltraSoundData::numOfIRSend * 2;
    }
    else
    {
      currentIndex++;
      for(int i = 0; i < UltraSoundData::numOfIRSend;i++)
      {
        data.dataLeft[i] = currentAllSensorsValue[currentIndex++];
        data.dataRight[i] = currentAllSensorsValue[currentIndex++];
      }
    }
  } 
  catch (ALError e)
  {
    std::cerr << "Failed to get Information from: " << e.toString() << endl;
  }
}

void DCMHandler::get(BatteryData& data) const
{
  try{
    data.charge = currentAllSensorsValue[thBatteryDataIdex];
  } catch (ALError e) {
    std::cerr << "Failed to get Information from: " << e.toString() << endl;
  }
}

