/**
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 */

#include "Sensor.h"
#include <PlatformInterface/Platform.h>

Sensor::Sensor():
theOdometryDataReader(NULL)
{
}

Sensor::~Sensor()
{
  if (theOdometryDataReader != NULL)
    delete theOdometryDataReader;
}


#define REG_INPUT(R) \
  platformInterface.registerCognitionInput(get##R())

void Sensor::init(naoth::PlatformInterfaceBase& platformInterface)
{
  // read RobotInfo (could be provided by the platform)
  RobotInfo& robot = getRobotInfo();
  robot.platform = platformInterface.getName();
  robot.bodyNickName = platformInterface.getBodyNickName();
  robot.bodyID = platformInterface.getBodyID();
  robot.basicTimeStep = platformInterface.getBasicTimeStep();

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
  theOdometryDataReader = new MessageReader(platformInterface.getMessageQueue("OdometryData"));
}//end init


// read the incoming data from the motion
void Sensor::execute()
{
  
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

  GT_TRACE("Sensor:execute() end");

}//end execute

