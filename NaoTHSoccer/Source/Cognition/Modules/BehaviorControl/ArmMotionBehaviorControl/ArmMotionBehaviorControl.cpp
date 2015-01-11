/**
 * @file ArmMotionBehaviorControl.cpp
 *
 * @author <a href="mailto:scheunem@informatik.hu-berlin.de">Marcus Scheunemann</a>
 * Implementation of class ArmMotionBehaviorControl
 */

#include "ArmMotionBehaviorControl.h"

#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugModify.h"

ArmMotionBehaviorControl::ArmMotionBehaviorControl() 
{

  // arms
  DEBUG_REQUEST_REGISTER("ArmMotionBehaviorControl:set_left_arm_position", "", false);
  DEBUG_REQUEST_REGISTER("ArmMotionBehaviorControl:set_left_arm_position2", "", false);
}

void ArmMotionBehaviorControl::execute() 
{
  // reset some stuff by default
  getMotionRequest().forced = false;
  getMotionRequest().standHeight = -1; // sit in a stable position

  execArms();

}//end execute

void ArmMotionBehaviorControl::execArms()
{
  DEBUG_REQUEST("ArmMotionBehaviorControl:set_left_arm_position",
   getMotionRequest().id = motion::stand;
   getMotionRequest().standHeight = -1; // sit in a stable position
   getMotionRequest().armMotionRequest.id = ArmMotionRequest::set_left_arm_joint_position;
   getMotionRequest().armMotionRequest.max_velocity_deg_in_sec = 10.0;
   getMotionRequest().armMotionRequest.lShoulderPosition = Vector2<double>(Math::fromDegrees(90.0), Math::fromDegrees(90.0));//Vector2<double>(ShoulderPitch, ShoulderRoll);
   getMotionRequest().armMotionRequest.lElbowPosition = Vector2<double>(Math::fromDegrees(10.0), Math::fromDegrees(10.0));//Vector2<double>(ElbowYaw, ElbowRoll);
   getMotionRequest().armMotionRequest.stiffness = 0.5;
  );

   DEBUG_REQUEST("ArmMotionBehaviorControl:set_left_arm_position2",
    getMotionRequest().id = motion::stand;
    getMotionRequest().standHeight = -1; // sit in a stable position
    getMotionRequest().armMotionRequest.id = ArmMotionRequest::set_left_arm_joint_position;
    getMotionRequest().armMotionRequest.max_velocity_deg_in_sec = 10.0;
    getMotionRequest().armMotionRequest.lShoulderPosition = Vector2<double>(Math::fromDegrees(10.0), Math::fromDegrees(10.0));//Vector2<double>(ShoulderPitch, ShoulderRoll);
    getMotionRequest().armMotionRequest.lElbowPosition = Vector2<double>(Math::fromDegrees(10.0), Math::fromDegrees(10.0));//Vector2<double>(ElbowYaw, ElbowRoll);
    getMotionRequest().armMotionRequest.stiffness = 0.5;
   );
} //execArms
