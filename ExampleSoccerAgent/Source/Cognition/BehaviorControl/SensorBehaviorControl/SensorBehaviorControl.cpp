/**
 * @file SensorBehaviorControl.cpp
 *
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
 * @author <a href="mailto:goehring@informatik.hu-berlin.de">Daniel Goehring</a>
 * Implementation of class SensorBehaviorControl
 */

#include "SensorBehaviorControl.h"

#include "Tools/Debug/DebugRequest.h"
#include "Tools/NaoInfo.h"

SensorBehaviorControl::SensorBehaviorControl() 
{
  // test behavior
  DEBUG_REQUEST_REGISTER("SensorBehaviorControl:behavior:goto_ball", "Executes a simple behavior: goto the ball.", false);
  DEBUG_REQUEST_REGISTER("SensorBehaviorControl:behavior:kick", "Executes a simple behavior: kick the ball after reaching the ball", false);
  DEBUG_REQUEST_REGISTER("SensorBehaviorControl:behavior:search_ball", "while the ball is missing, turnning and searching", false);

  DEBUG_REQUEST_REGISTER("SensorBehaviorControl:behavior:walk_cycle", "let the robot walk in a cycle", false);
  DEBUG_REQUEST_REGISTER("SensorBehaviorControl:behavior:stability_test", "perform a stability test", false);

  DEBUG_REQUEST_REGISTER("SensorBehaviorControl:sound", "Play a test sound", false);


  // kick exercise
  DEBUG_REQUEST_REGISTER("SensorBehaviorControl:motion:kick", "if the ball is seen then try to kick it", false);
  DEBUG_REQUEST_REGISTER("SensorBehaviorControl:motion:finish_kick", "execute the kick", false);
  DEBUG_REQUEST_REGISTER("SensorBehaviorControl:motion:left_kick", "kick the ball by left foot, otherwise right foot", false);
}

void SensorBehaviorControl::execute() 
{
  getMotionRequest().id = MotionRequest::stand;

  // execute the test behavior
  testBehavior();

  // play some test sound :)
  getSoundPlayData().soundFile = "";
  DEBUG_REQUEST("SensorBehaviorControl:sound",
    getSoundPlayData().soundFile = "beebeep.wav";
  );
}//end execute


void SensorBehaviorControl::testBehavior() 
{
  // TODO: do something useful here
  getMotionRequest().id = MotionRequest::walk_forward;
}//end testBehavior

