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
  getRobotInfo().platform = platform.getPlatformName();
  getRobotInfo().headNickName = platform.getHeadNickName();
  getRobotInfo().bodyNickName = platform.getBodyNickName();
  getRobotInfo().bodyID = platform.getBodyID();
  getRobotInfo().basicTimeStep = platform.getBasicTimeStep();
  
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
  REG_INPUT(CpuData);
  REG_INPUT(AudioData);
  
  REG_INPUT(GPSData);
  REG_INPUT(TeamMessageDataIn);
  REG_INPUT(RemoteMessageDataIn);
  REG_INPUT(GameData);
  REG_INPUT(DebugMessageInCognition);

  platformInterface.registerBufferedInputChanel(getCameraMatrixBuffer());
  platformInterface.registerBufferedInputChanel(getCameraMatrixBufferTop());
  platformInterface.registerInputChanel(getMotionStatus());
  platformInterface.registerInputChanel(getOdometryData());
  platformInterface.registerInputChanel(getCalibrationData());
  platformInterface.registerInputChanel(getInertialModel());
  platformInterface.registerInputChanel(getIMUData());
  platformInterface.registerInputChanel(getBodyStatus());
  platformInterface.registerInputChanel(getGroundContactModel());
  platformInterface.registerInputChanel(getCollisionPercept());
}//end init


void Sensor::execute()
{
  // add calibration to inertial sensor
  //getInertialPercept().data = getInertialSensorData().data + getCalibrationData().inertialSensorOffset;

  GT_TRACE("Sensor:execute() end");

  // TODO: this needs to be cleaned up together with the whole debug network communication infrastructure
  // EVIL HACK: expect that only RemoteControlCommand are sent though RemoteMessageDataIn
  // read only the last message
  if (getRemoteMessageDataIn().data.size() > 0 ) {
    std::stringstream ss(getRemoteMessageDataIn().data.back());
    Serializer<RemoteControlCommand>::deserialize(ss, getRemoteControlCommand());

    getRemoteControlCommand().frameInfoWhenUpdated = getFrameInfo();
  }

}//end execute

