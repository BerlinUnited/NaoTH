/**
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 */

#include "Sensor.h"
#include <PlatformInterface/Platform.h>

Sensor::Sensor():
theMotionStatusReader(NULL),
theOdometryDataReader(NULL)
{
}

Sensor::~Sensor()
{
  if (theMotionStatusReader != NULL)
    delete theMotionStatusReader;
  if (theOdometryDataReader != NULL)
    delete theOdometryDataReader;
  if (theCalibrationDataReader != NULL)
    delete theCalibrationDataReader;
}


#define REG_INPUT(R) \
  platformInterface.registerCognitionInput(get##R())

void Sensor::init(naoth::PlatformInterfaceBase& platformInterface)
{
  // read RobotInfo
  RobotInfo& robot = getRobotInfo();
  robot.platform = platformInterface.getName();
  robot.bodyNickName = platformInterface.getBodyNickName();
  robot.bodyID = platformInterface.getBodyID();
  robot.basicTimeStep = platformInterface.getBasicTimeStep();

  // init player number, team number and etc.
  getGameData().loadFromCfg( naoth::Platform::getInstance().theConfiguration );

  REG_INPUT(Image);
  REG_INPUT(CurrentCameraSettings);
  REG_INPUT(VirtualVision);
  REG_INPUT(FrameInfo);

  REG_INPUT(AccelerometerData);
  REG_INPUT(GyrometerData);
  REG_INPUT(InertialSensorData);
  REG_INPUT(FSRData);
  REG_INPUT(SensorJointData);
  REG_INPUT(UltraSoundReceiveData);
  REG_INPUT(BatteryData);
  REG_INPUT(ButtonData);
  REG_INPUT(IRReceiveData);
  
  REG_INPUT(TeamMessageDataIn);
  REG_INPUT(GameData);
  REG_INPUT(DebugMessageIn);
  
  platformInterface.registerCognitionInputChanel<MotionStatus, Serializer<MotionStatus> >(getMotionStatus());
  platformInterface.registerCognitionInputChanel<OdometryData, Serializer<OdometryData> >(getOdometryData());
  platformInterface.registerCognitionInputChanel<CalibrationData, Serializer<CalibrationData> >(getCalibrationData());

  //theMotionStatusReader = new MessageReader(platformInterface.getMessageQueue("MotionStatus"));
  //theOdometryDataReader = new MessageReader(platformInterface.getMessageQueue("OdometryData"));
  //theCalibrationDataReader = new MessageReader(platformInterface.getMessageQueue("CalibrationData"));
}//end init


void Sensor::execute()
{
  /*
  // data from motion
  GT_TRACE("!theMotionStatusReader->empty()");
  if ( !theMotionStatusReader->empty() )
  {
    string msg = theMotionStatusReader->read();
    // drop old message, TODO: use them!
    while ( !theMotionStatusReader->empty() )
    {
      msg = theMotionStatusReader->read();
    }
    stringstream ss(msg);
    Serializer<MotionStatus>::deserialize(ss, getMotionStatus());
  }
  
  GT_TRACE(" !theOdometryDataReader->empty()");
  if ( !theOdometryDataReader->empty() )
  {
    string msg = theOdometryDataReader->read();
    // drop old message, TODO: use them!
    while ( !theOdometryDataReader->empty() )
    {
      msg = theOdometryDataReader->read();
    }
    stringstream ss(msg);
    Serializer<OdometryData>::deserialize(ss, getOdometryData());
  }

  GT_TRACE("!theCalibrationDataReader->empty()");
  if ( !theCalibrationDataReader->empty() )
  {
    string msg = theCalibrationDataReader->read();
    // drop old message
    while ( !theCalibrationDataReader->empty() )
    {
      msg = theCalibrationDataReader->read();
    }
    stringstream ss(msg);
    Serializer<CalibrationData>::deserialize(ss, getCalibrationData());
  }
  */

  // add calibration to inertial sensor
  getInertialPercept().data = getInertialSensorData().data + getCalibrationData().inertialSensorOffset;

  GT_TRACE("Sensor:execute() end");

}//end execute

