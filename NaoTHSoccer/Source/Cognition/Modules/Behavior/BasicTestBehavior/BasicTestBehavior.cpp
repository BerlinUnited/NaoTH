/**
 * @file BasicTestBehavior.cpp
 *
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
 * @author <a href="mailto:goehring@informatik.hu-berlin.de">Daniel Goehring</a>
 * Implementation of class BasicTestBehavior
 */

#include "BasicTestBehavior.h"

BasicTestBehavior::BasicTestBehavior() 
{
  // test head control
  DEBUG_REQUEST_REGISTER("BasicTestBehavior:head:Search", "Set the HeadMotion-Request to 'search'.", false);
  DEBUG_REQUEST_REGISTER("BasicTestBehavior:head:reverseSearchDirection", "Set the head search direction to counterclockwise.", false);
  DEBUG_REQUEST_REGISTER("BasicTestBehavior:head:LookAtBall_image", "Set the HeadMotion-Request to 'look_at_ball'.", false);
  DEBUG_REQUEST_REGISTER("BasicTestBehavior:head:LookAtBall_field", "Set the HeadMotion-Request to 'look_at_ball'.", false);
  DEBUG_REQUEST_REGISTER("BasicTestBehavior:head:Stabilize", "Set the HeadMotion-Request to 'stabilize'.", false);
  DEBUG_REQUEST_REGISTER("BasicTestBehavior:head:SwitchToBottomCamera", "Switch to bottom camera", true);
  DEBUG_REQUEST_REGISTER("BasicTestBehavior:head:look_at_ball_modell", "Search for ball if not seen", false);
  DEBUG_REQUEST_REGISTER("BasicTestBehavior:head:look_straight_ahead", "look straight ahead", false);
  DEBUG_REQUEST_REGISTER("BasicTestBehavior:head:goto_angle", "look at specific angle given as Modify", false);

  // test motion control
  DEBUG_REQUEST_REGISTER("BasicTestBehavior:motion:standard_stand", "stand as standard or not", true);

  // walk
  DEBUG_REQUEST_REGISTER("BasicTestBehavior:motion:walk_forward", "Walk foraward as fast as possible", false);
  DEBUG_REQUEST_REGISTER("BasicTestBehavior:motion:walk_backward", "Walk backward as fast as possible", false);
  DEBUG_REQUEST_REGISTER("BasicTestBehavior:motion:strafe_left", "Set the motion request to 'strafe'.", false);
  DEBUG_REQUEST_REGISTER("BasicTestBehavior:motion:strafe_right", "Set the motion request to 'strafe'.", false);
  DEBUG_REQUEST_REGISTER("BasicTestBehavior:motion:turn_left", "Set the motion request to 'turn_right'.", false);
  DEBUG_REQUEST_REGISTER("BasicTestBehavior:motion:turn_right", "Set the motion request to 'turn_right'.", false);
  DEBUG_REQUEST_REGISTER("BasicTestBehavior:motion:walk_forward", "Walk foraward as fast as possible", false);
  DEBUG_REQUEST_REGISTER("BasicTestBehavior:motion:stepping", "walk with zero speed", false);
  DEBUG_REQUEST_REGISTER("BasicTestBehavior:motion:step_control", "test step control", false);

  DEBUG_REQUEST_REGISTER("BasicTestBehavior:motion:walk_normal", "normal walk", true);
  DEBUG_REQUEST_REGISTER("BasicTestBehavior:motion:walk_stable", "fast walk", false);
  DEBUG_REQUEST_REGISTER("BasicTestBehavior:motion:walk_fast", "stable walk", false);

  DEBUG_REQUEST_REGISTER("BasicTestBehavior:motion:walk_hip", "hip as coordinate", true);
  DEBUG_REQUEST_REGISTER("BasicTestBehavior:motion:walk_lfoot", "left foot as coordinate", false);
  DEBUG_REQUEST_REGISTER("BasicTestBehavior:motion:walk_rfoot", "right foot as coordinate", false);


  // key frame motion
  DEBUG_REQUEST_REGISTER("BasicTestBehavior:motion:stand_up_from_front", "Set the motion request to 'stand_up_from_front'", false);
  DEBUG_REQUEST_REGISTER("BasicTestBehavior:motion:stand_up_from_back", "Set the motion request to 'stand_up_from_back'", false);
  DEBUG_REQUEST_REGISTER("BasicTestBehavior:motion:stand_up_from_back_arms_back", "Set the motion request to 'stand_up_from_back'", false);

  // other motions
  DEBUG_REQUEST_REGISTER("BasicTestBehavior:motion:dead", "Set the robot dead.", false);
  DEBUG_REQUEST_REGISTER("BasicTestBehavior:motion:stand", "The default motion, otherwise do nothing", true);
  DEBUG_REQUEST_REGISTER("BasicTestBehavior:motion:sit", "sit down, has a rest", false);
  DEBUG_REQUEST_REGISTER("BasicTestBehavior:motion:init", "Set the robot init.", false);
  DEBUG_REQUEST_REGISTER("BasicTestBehavior:motion:dance", "Let's dance", false);
  DEBUG_REQUEST_REGISTER("BasicTestBehavior:motion:protect_falling", "Don't hurt me!", false);

  // needed by the motion editor
  DEBUG_REQUEST_REGISTER("BasicTestBehavior:motion:play_editor_motionnet", "play the motion editor motion", false);

  DEBUG_REQUEST_REGISTER("BasicTestBehavior:sound:test", "it is what it is...", false);

  DEBUG_REQUEST_REGISTER("BasicTestBehavior:led:HeadFrontLeft0", "it is what it is...", false);
  DEBUG_REQUEST_REGISTER("BasicTestBehavior:led:HeadFrontLeft1", "it is what it is...", false);
  DEBUG_REQUEST_REGISTER("BasicTestBehavior:led:HeadMiddleRight0", "it is what it is...", false);
  DEBUG_REQUEST_REGISTER("BasicTestBehavior:led:HeadRearRight2", "it is what it is...", false);
  DEBUG_REQUEST_REGISTER("BasicTestBehavior:led:all", "it is what it is...", false);

  DEBUG_REQUEST_REGISTER("BasicTestBehavior:arms:01_arms_back", "set arms request to none", false);
  DEBUG_REQUEST_REGISTER("BasicTestBehavior:arms:02_arms_down", "set arms request to none", false);
  DEBUG_REQUEST_REGISTER("BasicTestBehavior:arms:03_arms_none", "set arms request to none", false);
}

void BasicTestBehavior::execute() 
{
  // reset some stuff by default
  getMotionRequest().forced = false;
  getMotionRequest().standHeight = -1; // sit in a stable position

  testHead();

  testMotion();

  testLED();

  getSoundPlayData().soundFile = "";
  DEBUG_REQUEST("BasicTestBehavior:sound:test",
    getSoundPlayData().soundFile = "victory.wav";
  );

  DEBUG_REQUEST("BasicTestBehavior:arms:01_arms_back", 
    getMotionRequest().armMotionRequest.id = ArmMotionRequest::arms_back;);
  DEBUG_REQUEST("BasicTestBehavior:arms:02_arms_down", 
    getMotionRequest().armMotionRequest.id = ArmMotionRequest::arms_down;);
  DEBUG_REQUEST("BasicTestBehavior:arms:03_arms_none", 
    getMotionRequest().armMotionRequest.id = ArmMotionRequest::arms_none;);

}//end execute

void BasicTestBehavior::testHead() 
{
  getHeadMotionRequest().cameraID = CameraInfo::Top;
  DEBUG_REQUEST("BasicTestBehavior:head:SwitchToBottomCamera",
    getHeadMotionRequest().cameraID = CameraInfo::Bottom;
  );

  // keep the head as forced by default
  getHeadMotionRequest().id = HeadMotionRequest::hold;

  DEBUG_REQUEST("BasicTestBehavior:head:Search",
    getHeadMotionRequest().id = HeadMotionRequest::search;
    getHeadMotionRequest().searchCenter = Vector3<double>(2000, 0, 0);
    getHeadMotionRequest().searchSize = Vector3<double>(1500, 2000, 0);
  );

  getHeadMotionRequest().searchDirection = true;
  DEBUG_REQUEST("BasicTestBehavior:head:reverseSearchDirection",
    getHeadMotionRequest().searchDirection = false;
  );

  DEBUG_REQUEST("BasicTestBehavior:head:Stabilize",
    getHeadMotionRequest().id = HeadMotionRequest::stabilize;
  );

  DEBUG_REQUEST("BasicTestBehavior:head:LookAtBall_image",
    if (getBallPercept().ballWasSeen) 
    {
      getHeadMotionRequest().id = HeadMotionRequest::look_at_point;
      getHeadMotionRequest().targetPointInImage = getBallPercept().centerInImage;
    }
  );

  DEBUG_REQUEST("BasicTestBehavior:head:LookAtBall_field",
    if (getBallPercept().ballWasSeen) 
    {
      getHeadMotionRequest().id = HeadMotionRequest::look_at_world_point;
      getHeadMotionRequest().targetPointInTheWorld.x = getBallPercept().bearingBasedOffsetOnField.x;
      getHeadMotionRequest().targetPointInTheWorld.y = getBallPercept().bearingBasedOffsetOnField.y;
      getHeadMotionRequest().targetPointInTheWorld.z = getFieldInfo().ballRadius;
    }
  );
 
  DEBUG_REQUEST("BasicTestBehavior:head:look_straight_ahead",
    getHeadMotionRequest().id = HeadMotionRequest::look_straight_ahead;
  );



  DEBUG_REQUEST("BasicTestBehavior:head:goto_angle",
    getHeadMotionRequest().id = HeadMotionRequest::goto_angle;

    double yaw  = 0;
    MODIFY("BasicTestBehavior:head:headYaw_deg",yaw);
    double pitch = -14;
    MODIFY("BasicTestBehavior:head:headPitch_deg",pitch);
    getHeadMotionRequest().targetJointPosition.x = Math::fromDegrees(yaw);
    getHeadMotionRequest().targetJointPosition.y = Math::fromDegrees(pitch);
  );


  DEBUG_REQUEST("BasicTestBehavior:head:look_at_ball_modell",
    if(getFrameInfo().getTimeSince(getBallModel().frameInfoWhenBallWasSeen.getTime()) < 3000)
    {
      Vector2d xp = getBallModel().position;
      double d = 250;
      MODIFY("look_distance",d);
      xp.normalize(d);

      getHeadMotionRequest().id = HeadMotionRequest::look_at_world_point;
      getHeadMotionRequest().targetPointInTheWorld.x = xp.x; //getBallModel().position.x;
      getHeadMotionRequest().targetPointInTheWorld.y = xp.y; //getBallModel().position.y;
      getHeadMotionRequest().targetPointInTheWorld.z = getFieldInfo().ballRadius;
    }
  );

}//end testHead


void BasicTestBehavior::testMotion() 
{
  getMotionRequest().walkRequest.target = Pose2D();

  DEBUG_REQUEST("BasicTestBehavior:motion:stand", 
    getMotionRequest().id = motion::stand;
  );
  
  getMotionRequest().standardStand = false;
  DEBUG_REQUEST("BasicTestBehavior:motion:standard_stand",
    getMotionRequest().standardStand = true;
      getMotionRequest().standHeight = -1; // minus means the same value as walk
  );

  DEBUG_REQUEST("BasicTestBehavior:motion:sit",
    getMotionRequest().id = motion::sit;
  );

  DEBUG_REQUEST("BasicTestBehavior:motion:walk_forward",
    getMotionRequest().id = motion::walk;
    getMotionRequest().walkRequest.target.translation.x = 500;
  );

  DEBUG_REQUEST("BasicTestBehavior:motion:walk_backward",
    getMotionRequest().id = motion::walk;
    getMotionRequest().walkRequest.target.translation.x = -500;
  );


  DEBUG_REQUEST("BasicTestBehavior:motion:strafe_right",
    getMotionRequest().id = motion::walk;
    getMotionRequest().walkRequest.target.translation.y = -500;
  );

  DEBUG_REQUEST("BasicTestBehavior:motion:strafe_left",
    getMotionRequest().id = motion::walk;
    getMotionRequest().walkRequest.target.translation.y = 500;
  );

  DEBUG_REQUEST("BasicTestBehavior:motion:turn_right",
    getMotionRequest().id = motion::walk;
    getMotionRequest().walkRequest.target.rotation = Math::fromDegrees(-60);
  );

  DEBUG_REQUEST("BasicTestBehavior:motion:turn_left",
    getMotionRequest().id = motion::walk;
    getMotionRequest().walkRequest.target.rotation = Math::fromDegrees(60);
  );

  DEBUG_REQUEST("BasicTestBehavior:motion:stepping",
      getMotionRequest().id = motion::walk;
      getMotionRequest().walkRequest.target = Pose2D();
    );

  DEBUG_REQUEST("BasicTestBehavior:motion:step_control",
   if ( getMotionStatus().stepControl.stepID % 5 == 0)
  {
    getMotionRequest().walkRequest.stepControl.stepID = getMotionStatus().stepControl.stepID;
    switch(getMotionStatus().stepControl.moveableFoot)
  {
    case MotionStatus::StepControlStatus::LEFT:
    case MotionStatus::StepControlStatus::BOTH:
    {
      getMotionRequest().walkRequest.stepControl.moveLeftFoot = true;
      getMotionRequest().walkRequest.coordinate = WalkRequest::LFoot;
      break;
    }
    case MotionStatus::StepControlStatus::RIGHT:
    {
      getMotionRequest().walkRequest.stepControl.moveLeftFoot = false;
      getMotionRequest().walkRequest.coordinate = WalkRequest::RFoot;
      break;
    }
    default: ASSERT(false);
      break;
    }
  double stepTime = 1000;
  double speedDirection = 0;
  MODIFY("StepControl.time",stepTime);
  MODIFY("StepControl.speedDirection",speedDirection);
    getMotionRequest().walkRequest.stepControl.target = Pose2D(0, 100, 0);
    getMotionRequest().walkRequest.stepControl.time = (unsigned int)stepTime;
    getMotionRequest().walkRequest.stepControl.speedDirection = Math::fromDegrees(speedDirection);
  }
    );

  DEBUG_REQUEST("BasicTestBehavior:motion:walk_normal",
              getMotionRequest().walkRequest.character = 0.5;
  );
  DEBUG_REQUEST("BasicTestBehavior:motion:walk_fast",
              getMotionRequest().walkRequest.character = 1;
  );
  DEBUG_REQUEST("BasicTestBehavior:motion:walk_stable",
              getMotionRequest().walkRequest.character = 0;
  );

  DEBUG_REQUEST("BasicTestBehavior:motion:walk_hip",
              getMotionRequest().walkRequest.coordinate = WalkRequest::Hip;
  );
  DEBUG_REQUEST("BasicTestBehavior:motion:walk_fast",
              getMotionRequest().walkRequest.coordinate = WalkRequest::LFoot;
  );
  DEBUG_REQUEST("BasicTestBehavior:motion:walk_stable",
              getMotionRequest().walkRequest.coordinate = WalkRequest::RFoot;
  );

  double offsetR = 0;
  MODIFY("walk.offset.r", offsetR);
  getMotionRequest().walkRequest.offset.rotation = Math::fromDegrees(offsetR);
  MODIFY("walk.offset.x", getMotionRequest().walkRequest.offset.translation.x);
  MODIFY("walk.offset.y", getMotionRequest().walkRequest.offset.translation.y);
  MODIFY("walk.character", getMotionRequest().walkRequest.character);

  DEBUG_REQUEST("BasicTestBehavior:motion:stand_up_from_front",
    getMotionRequest().id = motion::stand_up_from_front;
  );

  DEBUG_REQUEST("BasicTestBehavior:motion:stand_up_from_back",
    getMotionRequest().id = motion::stand_up_from_back;
  );

  DEBUG_REQUEST("BasicTestBehavior:motion:stand_up_from_back_arms_back",
    getMotionRequest().id = motion::stand_up_from_back_arms_back;
  );

  DEBUG_REQUEST("BasicTestBehavior:motion:dead",
    getMotionRequest().id = motion::dead;
    getMotionRequest().forced = true;
  );

  DEBUG_REQUEST("BasicTestBehavior:motion:init",
    getMotionRequest().id = motion::init;
  );

  DEBUG_REQUEST("BasicTestBehavior:motion:dance",
    getMotionRequest().id = motion::dance;
  );

  DEBUG_REQUEST("BasicTestBehavior:motion:protect_falling",
    getMotionRequest().id = motion::protect_falling;
  );

  DEBUG_REQUEST("BasicTestBehavior:motion:play_editor_motionnet",
    getMotionRequest().id = motion::play_editor_motionnet;
  );
          
}//end testMotion


void BasicTestBehavior::testLED() {

  //init LEDs
  for (int i = LEDData::HeadFrontLeft0; i <= LEDData::HeadRearRight2; i++) {
    getBehaviorLEDRequest().request.theMonoLED[i] = 0.0;
  }

  //set LEDS
  DEBUG_REQUEST("BasicTestBehavior:led:HeadFrontLeft0",
    getBehaviorLEDRequest().request.theMonoLED[LEDData::HeadFrontLeft0] = 1.0;
  );

  DEBUG_REQUEST("BasicTestBehavior:led:HeadFrontLeft1",
    getBehaviorLEDRequest().request.theMonoLED[LEDData::HeadFrontLeft1] = 1.0;
  );

  DEBUG_REQUEST("BasicTestBehavior:led:HeadMiddleRight0",
    getBehaviorLEDRequest().request.theMonoLED[LEDData::HeadMiddleRight0] = 1.0;
  );

  DEBUG_REQUEST("BasicTestBehavior:led:HeadRearRight2",
    getBehaviorLEDRequest().request.theMonoLED[LEDData::HeadRearRight2] = 1.0;
  );

  DEBUG_REQUEST("BasicTestBehavior:led:all",
    for (int i = LEDData::HeadFrontLeft0; i <= LEDData::HeadRearRight2; i++) {
      getBehaviorLEDRequest().request.theMonoLED[i] = 1.0;
    }
  );

} //testLED

