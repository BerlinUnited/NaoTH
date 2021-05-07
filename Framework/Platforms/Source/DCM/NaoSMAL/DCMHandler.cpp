

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
  catch(ALError& e) {
    std::cerr << "Failed to init DCMHandler: " << e.what() << endl;
  }

  // init device handlers
  ledHandler.init(pB);

  //Generate all strings for interaction

   // init sensors
  initSensorJoint();
  initFSR();
  initAccelerometer();
  initGyrometer();
  initInertialSensor();
  initButton();
  initUltraSoundReceive();

  // init actuators
  initMotorJoint();
  initUltraSoundSend();


  DCMPath_BodyId = "Device/DeviceList/ChestBoard/BodyId";
  DCMPath_BodyNickName = "Device/DeviceList/ChestBoard/BodyNickName";

  DCMPath_BatteryCharge = "Device/SubDeviceList/Battery/Charge/Sensor/Value";
  DCMPath_BatteryCurrent = "Device/SubDeviceList/Battery/Current/Sensor/Value";
  DCMPath_BatteryTemperature = "Device/SubDeviceList/Battery/Temperature/Sensor/Value";
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
  } catch (ALError& e)
  {
    std::cerr << "Failed to get Information from: " << e.what() << endl;
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
  catch(ALError& e)
  {
    std::cerr << "Failed to set " << path << ": " << e.what() << endl;
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
  DCMPath_FSR_Left[FSRData::FrontLeft]   = "Device/SubDeviceList/LFoot/FSR/FrontLeft/Sensor/Value";
  DCMPath_FSR_Left[FSRData::FrontRight]  = "Device/SubDeviceList/LFoot/FSR/FrontRight/Sensor/Value";
  DCMPath_FSR_Left[FSRData::RearLeft]    = "Device/SubDeviceList/LFoot/FSR/RearLeft/Sensor/Value";
  DCMPath_FSR_Left[FSRData::RearRight]   = "Device/SubDeviceList/LFoot/FSR/RearRight/Sensor/Value";

  DCMPath_FSR_Right[FSRData::FrontLeft]  = "Device/SubDeviceList/RFoot/FSR/FrontLeft/Sensor/Value";
  DCMPath_FSR_Right[FSRData::FrontRight] = "Device/SubDeviceList/RFoot/FSR/FrontRight/Sensor/Value";
  DCMPath_FSR_Right[FSRData::RearLeft]   = "Device/SubDeviceList/RFoot/FSR/RearLeft/Sensor/Value";
  DCMPath_FSR_Right[FSRData::RearRight]  = "Device/SubDeviceList/RFoot/FSR/RearRight/Sensor/Value";
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
  for(int i=0;i<FSRData::numOfFSR;i++) {
    allSensorsList[currentIndex++] = DCMPath_FSR_Left[i];
  }
  for(int i=0;i<FSRData::numOfFSR;i++) {
    allSensorsList[currentIndex++] = DCMPath_FSR_Right[i];
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

  //ButtonData
  ASSERT(theButtonDataIndex == currentIndex);
  for(int i=0;i<ButtonData::numOfButtons;i++)
  {
    allSensorsList[currentIndex++] = DCMPath_Button[i];
  }

  //UltraSoundReceiveData
  ASSERT(theUltraSoundReceiveDataIndex == currentIndex);
  allSensorsList[currentIndex++] = DCMPath_UltraSoundReceive;
  for(int i = 0; i < UltraSoundReceiveData::numOfUSEcho; i++)
  {
    allSensorsList[currentIndex++] = DCMPath_UltraSoundReceiveLeft[i];
    allSensorsList[currentIndex++] = DCMPath_UltraSoundReceiveRight[i];
  }

  ASSERT(theBatteryDataIndex == currentIndex);
  allSensorsList[currentIndex++] = DCMPath_BatteryCharge;
  allSensorsList[currentIndex++] = DCMPath_BatteryCurrent;
  allSensorsList[currentIndex++] = DCMPath_BatteryTemperature;

  // little sanity check
  assert(currentIndex == numOfSensors);

  //connect variables
  try {
    for(unsigned int i=0; i<numOfSensors; i++) {
      sensorPtrs[i] = (float*)al_memory->getDataPtr(allSensorsList[i]);
    }
  } catch(AL::ALError& e) {
    std::cerr << "[NaoSMAL] " << e.what() << std::endl;
  }

}//end initAllSensorData


void DCMHandler::readSensorData(float* dest)
{
  //timeStamp = al_dcmproxy->getTime(time_delay);

  for(unsigned int i=0; i<numOfSensors; i++) {
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
  catch(ALError& e) {
    std::cerr << "[NaoSMAL] Failed to init MotorJointAlias: " << e.what() << endl;
  }
}//end initMotorJoint


void DCMHandler::setSingleMotorData(const JointData::JointID jointID, const MotorJointData *theMotorJointData, int dcmTime)
{
  sendToDCM(DCMPath_MotorJointHardness[jointID],theMotorJointData->stiffness[jointID],dcmTime);
  sendToDCM(DCMPath_MotorJointPosition[jointID],theMotorJointData->position[jointID],dcmTime);
}


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
  catch(ALError& e) {
    std::cerr << "[NaoSMAL] Failed to set AllPositionData: " << e.what() << endl;
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
  catch(ALError& e) {
    std::cerr << "[NaoSMAL] Failed to set AllHardnessData: " << e.what() << endl;
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
  catch(ALError& e) {
    std::cerr << "[NaoSMAL] Failed to set AllHardnessData: " << e.what() << endl;
  }
}//end setAllHardnessData

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
  catch(ALError& e)
  {
    std::cerr << "[NaoSMAL] Failed to create UltraSound-Alias: " << e.what() << endl;
  }
}//end initUltraSoundSend

void DCMHandler::setUltraSoundSend(const UltraSoundSendData& data, int dcmTime)
{
  if(data.mode > 0) // don't send negative values
  {
    usSendCommands[4][0] = dcmTime;
    usSendCommands[5][0][0] = static_cast<float>(data.mode);

    try
    {
      al_dcmproxy->setAlias(usSendCommands);
    }
    catch(ALError& e) {
      std::cerr << "[NaoSMAL] Failed to set UltraSound: " << e.what() << endl;
    }
  }
}//end setUltraSoundSend
