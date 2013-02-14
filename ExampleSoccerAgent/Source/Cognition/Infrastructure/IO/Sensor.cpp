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
  // read RobotInfo (could be provided by the platform)
  RobotInfo& robot = getRobotInfo();
  robot.platform = platform.getName();
  robot.bodyNickName = platform.getBodyNickName();
  robot.bodyID = platform.getBodyID();
  robot.basicTimeStep = platform.getBasicTimeStep();

  // init player number, team number and etc.
  getGameData().loadFromCfg( naoth::Platform::getInstance().theConfiguration );

  REG_INPUT(AccelerometerData);
  REG_INPUT(BatteryData);
  REG_INPUT(ButtonData);
  REG_INPUT(GyrometerData);
  REG_INPUT(IRReceiveData);
  REG_INPUT(Image);
  REG_INPUT(InertialSensorData);
  REG_INPUT(FSRData);
  REG_INPUT(SensorJointData);
  REG_INPUT(UltraSoundReceiveData);
  REG_INPUT(TeamMessageDataIn);
  REG_INPUT(GameData);
  REG_INPUT(FrameInfo);
  REG_INPUT(CurrentCameraSettings);
  REG_INPUT(VirtualVision);
  REG_INPUT(DebugMessageIn);
  
  // communication channels with motion
  platformInterface.registerInputChanel(getOdometryData());
}//end init


// read the incoming data from the motion
void Sensor::execute()
{
}//end execute

