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
  platformInterface.registerInput(get##R())

void Sensor::init(naoth::ProcessInterface& platformInterface, const naoth::PlatformBase& platform)
{
  // read RobotInfo
  RobotInfo& robot = getRobotInfo();
  robot.platform = platform.getName();
  robot.headNickName = platform.getHeadNickName();
  robot.bodyNickName = platform.getBodyNickName();
  robot.bodyID = platform.getBodyID();
  robot.basicTimeStep = platform.getBasicTimeStep();
  
  // init player number, team number and etc.
  getGameData().loadFromCfg( naoth::Platform::getInstance().theConfiguration );

  REG_INPUT(Image);
  REG_INPUT(ImageTop);
  REG_INPUT(CurrentCameraSettings);
  REG_INPUT(CurrentCameraSettingsTop);
  REG_INPUT(VirtualVision);
  REG_INPUT(VirtualVisionTop);
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
  
  REG_INPUT(GPSData);
  REG_INPUT(TeamMessageDataIn);
  REG_INPUT(GameData);
  REG_INPUT(DebugMessageIn);

  platformInterface.registerBufferedInputChanel(getCameraMatrixBuffer());
  platformInterface.registerBufferedInputChanel(getCameraMatrixBuffer2());
  platformInterface.registerInputChanel(getMotionStatus());
  platformInterface.registerInputChanel(getOdometryData());
  platformInterface.registerInputChanel(getCalibrationData());
  platformInterface.registerInputChanel(getInertialModel());
}//end init


void Sensor::execute()
{
  // add calibration to inertial sensor
  //getInertialPercept().data = getInertialSensorData().data + getCalibrationData().inertialSensorOffset;

  GT_TRACE("Sensor:execute() end");

}//end execute

