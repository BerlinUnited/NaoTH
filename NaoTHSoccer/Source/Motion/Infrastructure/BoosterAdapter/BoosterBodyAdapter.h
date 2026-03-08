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
END_DECLARE_MODULE(BoosterBodyAdapter)

class BoosterBodyAdapter : public BoosterBodyAdapterBase
{
private:
  SocketConnector bodyBridge;
  Booster::ActuatorData actuatorData;

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
    actuatorData.headPose.yaw   = getHeadMotionRequest().targetJointPosition.x; // yaw
    actuatorData.headPose.pitch = getHeadMotionRequest().targetJointPosition.y; // pitch

    actuatorData.walkVelocity.x         = getMotionRequest().walkRequest.target.translation.x;
    actuatorData.walkVelocity.y         = getMotionRequest().walkRequest.target.translation.y;
    actuatorData.walkVelocity.rotation  = getMotionRequest().walkRequest.target.rotation;

    bodyBridge.send_actuators(actuatorData);
  }
};

#endif // BOOSTER_BODY_ADAPTER_H