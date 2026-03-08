/**
* @file BoosterBodyAdapter.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Mellmann, Heinrich</a>
*
* 
* Example for UNIX soccet communication.
* https://github.com/troydhanson/network/blob/master/unixdomain/01.basic/cli.c
*
*/

#ifndef BOOSTER_BODY_ADAPTER_H
#define BOOSTER_BODY_ADAPTER_H

#include <ModuleFramework/Module.h>
#include <Tools/Debug/NaoTHAssert.h>

#include <Representations/Infrastructure/FrameInfo.h>

#include <Representations/Body/HeadPose.h>

#include "Representations/Motion/Request/HeadMotionRequest.h"
#include "Representations/Motion/Request/MotionRequest.h"

#include "BoosterData.h" 
#include "SocketConnector.h" 


BEGIN_DECLARE_MODULE(BoosterBodyAdapter)
  //PROVIDE(DebugParameterList)

  REQUIRE(FrameInfo)

  // actions
  REQUIRE(HeadMotionRequest)
  REQUIRE(MotionRequest)

  PROVIDE(HeadPose)
END_DECLARE_MODULE(BoosterBodyAdapter)

class BoosterBodyAdapter : public BoosterBodyAdapterBase
{
private:
  SocketConnector bodyBridge;

  Booster::ActuatorData actuatorData;
  Booster::SensorData sensorData;

public:

  BoosterBodyAdapter() 
  {
    // activate the image bridge
    bodyBridge.connect("/tmp/naoth_body");
    ASSERT_MSG(!bodyBridge.hasError(), "[BoosterBodyAdapter] cannot connect to the body socket.")
  }  

  virtual ~BoosterBodyAdapter() {
    bodyBridge.disconnect();
  } 

  virtual void execute() 
  {
    actuatorData.headRotation.yaw       = static_cast<float>(getHeadMotionRequest().targetJointPosition.x); // yaw
    actuatorData.headRotation.pitch     = static_cast<float>(getHeadMotionRequest().targetJointPosition.y); // pitch

    actuatorData.walkVelocity.x         = static_cast<float>(getMotionRequest().walkRequest.target.translation.x);
    actuatorData.walkVelocity.y         = static_cast<float>(getMotionRequest().walkRequest.target.translation.y);
    actuatorData.walkVelocity.rotation  = static_cast<float>(getMotionRequest().walkRequest.target.rotation);

    bodyBridge.send_actuators(actuatorData);

    // receive the newest sensor data
    bodyBridge.receive_sensors(sensorData);

    // unpack the sensor data

    getHeadPose().pose.translation.x    = sensorData.headPose.position.x;
    getHeadPose().pose.translation.y    = sensorData.headPose.position.y;
    getHeadPose().pose.translation.z    = sensorData.headPose.position.z;

    getHeadPose().pose.rotation = RotationMatrix::fromQuaternion(
      { sensorData.headPose.orientation.x,
        sensorData.headPose.orientation.y,
        sensorData.headPose.orientation.z }, 
        sensorData.headPose.orientation.w
    );
  }
};

#endif // BOOSTER_BODY_ADAPTER_H