/**
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 */

#include "Sensor.h"
#include <PlatformInterface/Platform.h>

Sensor::Sensor()
{
}

Sensor::~Sensor()
{

}


#define REG_INPUT(R) \
  platformInterface.registerCognitionInput(get##R())

void Sensor::init(naoth::PlatformInterfaceBase& platformInterface)
{
  // read RobotInfo
  RobotInfo& robot = getRobotInfo();
  robot.platform = platformInterface.getName();
  robot.headNickName = platformInterface.getHeadNickName();
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
  REG_INPUT(CameraMatrix);
  
  platformInterface.registerCognitionInputChanel<MotionStatus, Serializer<MotionStatus> >(getMotionStatus());
  platformInterface.registerCognitionInputChanel<OdometryData, Serializer<OdometryData> >(getOdometryData());
  //platformInterface.registerCognitionInputChanel<CalibrationData, Serializer<CalibrationData> >(getCalibrationData());
  platformInterface.registerCognitionInputChanel<InertialModel, Serializer<InertialModel> >(getInertialModel());
}//end init


void Sensor::execute()
{
  // add calibration to inertial sensor
  //getInertialPercept().data = getInertialSensorData().data + getCalibrationData().inertialSensorOffset;

  GT_TRACE("Sensor:execute() end");

}//end execute

