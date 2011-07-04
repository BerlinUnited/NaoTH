/**
 * @file SimpleMotionBehaviorControl.cpp
 *
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
 * @author <a href="mailto:goehring@informatik.hu-berlin.de">Daniel Goehring</a>
 * Implementation of class SimpleMotionBehaviorControl
 */

#include "SimpleMotionBehaviorControl.h"

#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugModify.h"

SimpleMotionBehaviorControl::SimpleMotionBehaviorControl() 
{
  // test head control
  DEBUG_REQUEST_REGISTER("SimpleMotionBehaviorControl:head:Search", "Set the HeadMotion-Request to 'search'.", false);
  DEBUG_REQUEST_REGISTER("SimpleMotionBehaviorControl:head:reverseSearchDirection", "Set the head search direction to counterclockwise.", false);
  DEBUG_REQUEST_REGISTER("SimpleMotionBehaviorControl:head:LookAtBall_image", "Set the HeadMotion-Request to 'look_at_ball'.", false);
  DEBUG_REQUEST_REGISTER("SimpleMotionBehaviorControl:head:LookAtBall_field", "Set the HeadMotion-Request to 'look_at_ball'.", false);
  DEBUG_REQUEST_REGISTER("SimpleMotionBehaviorControl:head:Stabilize", "Set the HeadMotion-Request to 'stabilize'.", false);
  DEBUG_REQUEST_REGISTER("SimpleMotionBehaviorControl:head:SwitchToBottomCamera", "Switch to bottom camera", true);
  DEBUG_REQUEST_REGISTER("SimpleMotionBehaviorControl:head:look_at_ball_modell", "Search for ball if not seen", false);
  DEBUG_REQUEST_REGISTER("SimpleMotionBehaviorControl:head:look_straight_ahead", "look straight ahead", false);
	DEBUG_REQUEST_REGISTER("SimpleMotionBehaviorControl:head:look_at_attention_point", "look at attention point", false);

  // test motion control
  DEBUG_REQUEST_REGISTER("SimpleMotionBehaviorControl:motion:standard_stand", "stand as standard or not", true);

  // walk
  DEBUG_REQUEST_REGISTER("SimpleMotionBehaviorControl:motion:walk_forward", "Walk foraward as fast as possible", false);
  DEBUG_REQUEST_REGISTER("SimpleMotionBehaviorControl:motion:walk_backward", "Walk backward as fast as possible", false);
  DEBUG_REQUEST_REGISTER("SimpleMotionBehaviorControl:motion:strafe_left", "Set the motion request to 'strafe'.", false);
  DEBUG_REQUEST_REGISTER("SimpleMotionBehaviorControl:motion:strafe_right", "Set the motion request to 'strafe'.", false);
  DEBUG_REQUEST_REGISTER("SimpleMotionBehaviorControl:motion:turn_left", "Set the motion request to 'turn_right'.", false);
  DEBUG_REQUEST_REGISTER("SimpleMotionBehaviorControl:motion:turn_right", "Set the motion request to 'turn_right'.", false);
  DEBUG_REQUEST_REGISTER("SimpleMotionBehaviorControl:motion:walk_forward", "Walk foraward as fast as possible", false);
  DEBUG_REQUEST_REGISTER("SimpleMotionBehaviorControl:motion:stepping", "walk with zero speed", false);

  // key frame motion
  DEBUG_REQUEST_REGISTER("SimpleMotionBehaviorControl:motion:stand_up_from_front", "Set the motion request to 'stand_up_from_front'", false);
  DEBUG_REQUEST_REGISTER("SimpleMotionBehaviorControl:motion:stand_up_from_back", "Set the motion request to 'stand_up_from_back'", false);

  // other motions
  DEBUG_REQUEST_REGISTER("SimpleMotionBehaviorControl:motion:dead", "Set the robot dead.", false);
  DEBUG_REQUEST_REGISTER("SimpleMotionBehaviorControl:motion:stand", "The default motion, otherwise do nothing", true);
  DEBUG_REQUEST_REGISTER("SimpleMotionBehaviorControl:motion:sit", "sit down, has a rest", false);
  DEBUG_REQUEST_REGISTER("SimpleMotionBehaviorControl:motion:init", "Set the robot init.", false);
  DEBUG_REQUEST_REGISTER("SimpleMotionBehaviorControl:motion:dance", "Let's dance", false);
  DEBUG_REQUEST_REGISTER("SimpleMotionBehaviorControl:motion:protect_falling", "Don't hurt me!", false);
}

void SimpleMotionBehaviorControl::execute() 
{
  // reset some stuff by default
  getMotionRequest().forced = false;
  getMotionRequest().standHeight = -1; // sit in a stable position

  testHead();

  testMotion();

}//end execute


void SimpleMotionBehaviorControl::testHead() 
{
  getHeadMotionRequest().cameraID = CameraInfo::Top;
  DEBUG_REQUEST("SimpleMotionBehaviorControl:head:SwitchToBottomCamera",
    getHeadMotionRequest().cameraID = CameraInfo::Bottom;
  );

  // keep the head as forced by default
  getHeadMotionRequest().id = HeadMotionRequest::hold;

  DEBUG_REQUEST("SimpleMotionBehaviorControl:head:Search",
    getHeadMotionRequest().id = HeadMotionRequest::search;
    getHeadMotionRequest().searchCenter = Vector3<double>(2000, 0, 0);
    getHeadMotionRequest().searchSize = Vector3<double>(1500, 2000, 0);
  );

  getHeadMotionRequest().searchDirection = true;
  DEBUG_REQUEST("SimpleMotionBehaviorControl:head:reverseSearchDirection",
    getHeadMotionRequest().searchDirection = false;
  );

  DEBUG_REQUEST("SimpleMotionBehaviorControl:head:Stabilize",
    getHeadMotionRequest().id = HeadMotionRequest::stabilize;
  );

  DEBUG_REQUEST("SimpleMotionBehaviorControl:head:LookAtBall_image",
    if (getBallPercept().ballWasSeen) 
    {
      getHeadMotionRequest().id = HeadMotionRequest::look_at_point;
      getHeadMotionRequest().targetPointInImage = getBallPercept().centerInImage;
    }
  );

  DEBUG_REQUEST("SimpleMotionBehaviorControl:head:LookAtBall_field",
    if (getBallPercept().ballWasSeen) 
    {
      getHeadMotionRequest().id = HeadMotionRequest::look_at_world_point;
      getHeadMotionRequest().targetPointInTheWorld.x = getBallPercept().bearingBasedOffsetOnField.x;
      getHeadMotionRequest().targetPointInTheWorld.y = getBallPercept().bearingBasedOffsetOnField.y;
      getHeadMotionRequest().targetPointInTheWorld.z = getFieldInfo().ballRadius;
    }
  );
 
  DEBUG_REQUEST("SimpleMotionBehaviorControl:head:look_straight_ahead",
    getHeadMotionRequest().id = HeadMotionRequest::look_straight_ahead;
  );
  
	DEBUG_REQUEST("SimpleMotionBehaviorControl:head:look_at_attention_point",
		getHeadMotionRequest().id = HeadMotionRequest::look_at_point_on_the_ground;
		getHeadMotionRequest().targetPointOnTheGround = getAttentionModel().mostInterestingPoint;
  );

}//end testHead


void SimpleMotionBehaviorControl::testMotion() 
{
  getMotionRequest().walkRequest.target = Pose2D();

  DEBUG_REQUEST("SimpleMotionBehaviorControl:motion:stand", 
    getMotionRequest().id = motion::stand; 
  );
  
  getMotionRequest().standardStand = false;
  DEBUG_REQUEST("SimpleMotionBehaviorControl:motion:standard_stand",
    getMotionRequest().standardStand = true;
      getMotionRequest().standHeight = -1; // minus means the same value as walk
  );

  DEBUG_REQUEST("SimpleMotionBehaviorControl:motion:sit",
    getMotionRequest().standardStand = true;
    getMotionRequest().standHeight = 160;
  );

  DEBUG_REQUEST("SimpleMotionBehaviorControl:motion:walk_forward",
    getMotionRequest().id = motion::walk;
    getMotionRequest().walkRequest.target.translation.x = 500;
    getMotionRequest().walkRequest.coordinate = WalkRequest::Hip;
  );

  DEBUG_REQUEST("SimpleMotionBehaviorControl:motion:walk_backward",
    getMotionRequest().id = motion::walk;
    getMotionRequest().walkRequest.target.translation.x = -500;
    getMotionRequest().walkRequest.coordinate = WalkRequest::Hip;
  );


  DEBUG_REQUEST("SimpleMotionBehaviorControl:motion:strafe_right",
    getMotionRequest().id = motion::walk;
    getMotionRequest().walkRequest.target.translation.y = -500;
    getMotionRequest().walkRequest.coordinate = WalkRequest::Hip;
  );

  DEBUG_REQUEST("SimpleMotionBehaviorControl:motion:strafe_left",
    getMotionRequest().id = motion::walk;
    getMotionRequest().walkRequest.target.translation.y = 500;
    getMotionRequest().walkRequest.coordinate = WalkRequest::Hip;
  );

  DEBUG_REQUEST("SimpleMotionBehaviorControl:motion:turn_right",
    getMotionRequest().id = motion::walk;
    getMotionRequest().walkRequest.target.rotation = Math::fromDegrees(-60);
    getMotionRequest().walkRequest.coordinate = WalkRequest::Hip;
  );

  DEBUG_REQUEST("SimpleMotionBehaviorControl:motion:turn_left",
    getMotionRequest().id = motion::walk;
    getMotionRequest().walkRequest.target.rotation = Math::fromDegrees(60);
    getMotionRequest().walkRequest.coordinate = WalkRequest::Hip;
  );

  DEBUG_REQUEST("SimpleMotionBehaviorControl:motion:stepping",
      getMotionRequest().id = motion::walk;
      getMotionRequest().walkRequest.target = Pose2D();
      getMotionRequest().walkRequest.coordinate = WalkRequest::Hip;
    );

  DEBUG_REQUEST("SimpleMotionBehaviorControl:motion:stand_up_from_front",
    getMotionRequest().id = motion::stand_up_from_front;
  );

  DEBUG_REQUEST("SimpleMotionBehaviorControl:motion:stand_up_from_back",
    getMotionRequest().id = motion::stand_up_from_back;
  );

  DEBUG_REQUEST("SimpleMotionBehaviorControl:motion:dead",
    getMotionRequest().id = motion::dead;
    getMotionRequest().forced = true;
  );

  DEBUG_REQUEST("SimpleMotionBehaviorControl:motion:init",
    getMotionRequest().id = motion::init;
  );

  DEBUG_REQUEST("SimpleMotionBehaviorControl:motion:dance",
    getMotionRequest().id = motion::dance;
  );

  DEBUG_REQUEST("SimpleMotionBehaviorControl:motion:protect_falling",
    getMotionRequest().id = motion::protect_falling;
  );
          
}//end testMotion

