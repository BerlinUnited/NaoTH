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
  DEBUG_REQUEST_REGISTER("BasicTestBehavior:head:look_at_ball_model", "Search for ball if not seen", false);
  DEBUG_REQUEST_REGISTER("BasicTestBehavior:head:look_at_multi_ball_percept", "", false);
  DEBUG_REQUEST_REGISTER("BasicTestBehavior:head:look_at_multi_ball_percept_image", "Set the HeadMotion-Request to 'look_at_ball'.", false);
  DEBUG_REQUEST_REGISTER("BasicTestBehavior:head:Stabilize", "Set the HeadMotion-Request to 'stabilize'.", false);
  DEBUG_REQUEST_REGISTER("BasicTestBehavior:head:SwitchToBottomCamera", "Switch to bottom camera", true);
  DEBUG_REQUEST_REGISTER("BasicTestBehavior:head:look_straight_ahead", "look straight ahead", false);
  DEBUG_REQUEST_REGISTER("BasicTestBehavior:head:goto_angle", "look at specific angle given as Modify", false);
  DEBUG_REQUEST_REGISTER("BasicTestBehavior:head:look_at_world_point", "", false);

  // test motion control
  DEBUG_REQUEST_REGISTER("BasicTestBehavior:motion:standard_stand", "stand as standard or not", true);

  // walk
  DEBUG_REQUEST_REGISTER("BasicTestBehavior:motion:force", "set the 'force' flag in the motion request true ", false);
  DEBUG_REQUEST_REGISTER("BasicTestBehavior:motion:walk:forward", "Walk forward as fast as possible", false);
  DEBUG_REQUEST_REGISTER("BasicTestBehavior:motion:walk:backward", "Walk backward as fast as possible", false);
  DEBUG_REQUEST_REGISTER("BasicTestBehavior:motion:walk:strafe_left", "Set the motion request to 'strafe'.", false);
  DEBUG_REQUEST_REGISTER("BasicTestBehavior:motion:walk:strafe_right", "Set the motion request to 'strafe'.", false);
  DEBUG_REQUEST_REGISTER("BasicTestBehavior:motion:walk:turn_left", "Set the motion request to 'turn_right'.", false);
  DEBUG_REQUEST_REGISTER("BasicTestBehavior:motion:walk:turn_right", "Set the motion request to 'turn_right'.", false);
  DEBUG_REQUEST_REGISTER("BasicTestBehavior:motion:walk:stepping", "walk with zero speed", false);

  DEBUG_REQUEST_REGISTER("BasicTestBehavior:motion:step_control", "test step control", false);

  DEBUG_REQUEST_REGISTER("BasicTestBehavior:motion:walk_style:normal", "normal walk", true);
  DEBUG_REQUEST_REGISTER("BasicTestBehavior:motion:walk_style:stable", "fast walk", false);
  DEBUG_REQUEST_REGISTER("BasicTestBehavior:motion:walk_style:fast", "stable walk", false);

  DEBUG_REQUEST_REGISTER("BasicTestBehavior:motion:walk_coordinate:hip", "hip as coordinate", true);
  DEBUG_REQUEST_REGISTER("BasicTestBehavior:motion:walk_coordinate:LFoot", "left foot as coordinate", false);
  DEBUG_REQUEST_REGISTER("BasicTestBehavior:motion:walk_coordinate:RFoot", "right foot as coordinate", false);

  // key frame motion
  DEBUG_REQUEST_REGISTER("BasicTestBehavior:motion:StandUp:from_front", "Set the motion request to 'stand_up_from_front'", false);
  DEBUG_REQUEST_REGISTER("BasicTestBehavior:motion:StandUp:from_back", "Set the motion request to 'stand_up_from_back'", false);
  DEBUG_REQUEST_REGISTER("BasicTestBehavior:motion:StandUp:from_back_arms_back", "Set the motion request to 'stand_up_from_back'", false);

  // goalie motions
  DEBUG_REQUEST_REGISTER("BasicTestBehavior:motion:id:block_middle","..",false);
  DEBUG_REQUEST_REGISTER("BasicTestBehavior:motion:id:goalie_get_down","..",false);
  DEBUG_REQUEST_REGISTER("BasicTestBehavior:motion:id:goalie_get_fast_down","..",false);
  DEBUG_REQUEST_REGISTER("BasicTestBehavior:motion:id:goalie_sit","..",false);
  DEBUG_REQUEST_REGISTER("BasicTestBehavior:motion:id:goalie_sit_small","..",false);
  DEBUG_REQUEST_REGISTER("BasicTestBehavior:motion:id:goalie_sit_block","..",false);
  DEBUG_REQUEST_REGISTER("BasicTestBehavior:motion:id:goalie_sit_n_dive_left","..",false);
  DEBUG_REQUEST_REGISTER("BasicTestBehavior:motion:id:goalie_sit_n_dive_right","..",false);
  DEBUG_REQUEST_REGISTER("BasicTestBehavior:motion:id:jump_indicator_left", "..", false);
  DEBUG_REQUEST_REGISTER("BasicTestBehavior:motion:id:jump_indicator_right", "..", false);

  // other motions
  DEBUG_REQUEST_REGISTER("BasicTestBehavior:motion:id:dead", "Set the robot dead.", false);
  DEBUG_REQUEST_REGISTER("BasicTestBehavior:motion:id:stand", "The default motion, otherwise do nothing", true);
  DEBUG_REQUEST_REGISTER("BasicTestBehavior:motion:id:sit", "sit down, has a rest", false);
  DEBUG_REQUEST_REGISTER("BasicTestBehavior:motion:id:init", "Set the robot init.", false);
  DEBUG_REQUEST_REGISTER("BasicTestBehavior:motion:id:dance", "Let's dance", false);
  DEBUG_REQUEST_REGISTER("BasicTestBehavior:motion:id:protect_falling", "Don't hurt me!", false);
  // needed by the motion editor
  DEBUG_REQUEST_REGISTER("BasicTestBehavior:motion:id:play_editor_motionnet", "play the motion editor motion", false);

  // parallelkinematik
  DEBUG_REQUEST_REGISTER("BasicTestBehavior:motion:ParallelKinematik:stepper", "parallel_stepper", false);
  DEBUG_REQUEST_REGISTER("BasicTestBehavior:motion:ParallelKinematik:dance", "parallel_dance", false);

  DEBUG_REQUEST_REGISTER("BasicTestBehavior:sound:test_play", "it is what it is...", false);
  DEBUG_REQUEST_REGISTER("BasicTestBehavior:sound:test_tts", "test text to speech with \":hallo world\"", false);

  DEBUG_REQUEST_REGISTER("BasicTestBehavior:led:HeadFrontLeft0", "it is what it is...", false);
  DEBUG_REQUEST_REGISTER("BasicTestBehavior:led:HeadFrontLeft1", "it is what it is...", false);
  DEBUG_REQUEST_REGISTER("BasicTestBehavior:led:HeadMiddleRight0", "it is what it is...", false);
  DEBUG_REQUEST_REGISTER("BasicTestBehavior:led:HeadRearRight2", "it is what it is...", false);
  DEBUG_REQUEST_REGISTER("BasicTestBehavior:led:all", "it is what it is...", false);

  DEBUG_REQUEST_REGISTER("BasicTestBehavior:arms:arms_back", "set arms request to back", false);
  DEBUG_REQUEST_REGISTER("BasicTestBehavior:arms:arms_down", "set arms request to down", false);
  DEBUG_REQUEST_REGISTER("BasicTestBehavior:arms:arms_none", "set arms request to none", false);
  DEBUG_REQUEST_REGISTER("BasicTestBehavior:arms:arms_synchronised_with_walk", "set arms request to none", false);

  DEBUG_REQUEST_REGISTER("BasicTestBehavior:whistle:listen", "start whistle detection", false);
  DEBUG_REQUEST_REGISTER("BasicTestBehavior:audio:capture", "start start audio capture for 4 channels", false);
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
  DEBUG_REQUEST("BasicTestBehavior:sound:test_play",
    getSoundPlayData().soundFile = "victory.wav";
  );

  DEBUG_REQUEST("BasicTestBehavior:sound:test_tts",
    getSoundPlayData().soundFile = ":hello world";
  );

  DEBUG_REQUEST("BasicTestBehavior:arms:arms_back",
    getMotionRequest().armMotionRequest.id = ArmMotionRequest::arms_back;);
  DEBUG_REQUEST("BasicTestBehavior:arms:arms_down",
    getMotionRequest().armMotionRequest.id = ArmMotionRequest::arms_down;);
  DEBUG_REQUEST("BasicTestBehavior:arms:arms_none",
    getMotionRequest().armMotionRequest.id = ArmMotionRequest::arms_none;);
  DEBUG_REQUEST("BasicTestBehavior:arms:arms_synchronised_with_walk",
    getMotionRequest().armMotionRequest.id = ArmMotionRequest::arms_synchronised_with_walk;);

  if (getRemoteControlCommand().action == RemoteControlCommand::WALK) {
    getMotionRequest().id = motion::walk;
    getMotionRequest().walkRequest.target.translation.x = 0;
  }

  testAudio();

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

  DEBUG_REQUEST("BasicTestBehavior:head:look_at_multi_ball_percept_image",
    if (getMultiBallPercept().wasSeen())
    {
      Vector2d pos = (*getMultiBallPercept().begin()).positionOnField;
      MultiBallPercept::BallPercept closestBallPercept(*getMultiBallPercept().begin());
      for (MultiBallPercept::ConstABPIterator iter = getMultiBallPercept().begin(); iter != getMultiBallPercept().end(); iter++) {
        if (pos.abs() > (*iter).positionOnField.abs()) {
          pos = (*iter).positionOnField;
          closestBallPercept = (*iter);
        }
      }
      getHeadMotionRequest().id = HeadMotionRequest::look_at_point;
      getHeadMotionRequest().targetPointInImage = closestBallPercept.centerInImage;
      getHeadMotionRequest().cameraID = closestBallPercept.cameraId;
    }
  );

  DEBUG_REQUEST("BasicTestBehavior:head:look_at_multi_ball_percept",
    if (getMultiBallPercept().wasSeen())
    {
      Vector2d pos = (*getMultiBallPercept().begin()).positionOnField;
      for(MultiBallPercept::ConstABPIterator iter = getMultiBallPercept().begin(); iter != getMultiBallPercept().end(); iter++) {
        if(pos.abs() > (*iter).positionOnField.abs()) {
          pos = (*iter).positionOnField;
        }
      }
      getHeadMotionRequest().id = HeadMotionRequest::look_at_world_point;
      getHeadMotionRequest().targetPointInTheWorld.x = pos.x;
      getHeadMotionRequest().targetPointInTheWorld.y = pos.y;
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
    double velocity = 300;
    MODIFY("BasicTestBehavior:head:headVelocity",velocity);
    getHeadMotionRequest().targetJointPosition.x = Math::fromDegrees(yaw);
    getHeadMotionRequest().targetJointPosition.y = Math::fromDegrees(pitch);
    getHeadMotionRequest().velocity = velocity;
  );

  DEBUG_REQUEST("BasicTestBehavior:head:look_at_ball_model",
    if(getFrameInfo().getTimeSince(getBallModel().getFrameInfoWhenBallWasSeen().getTime()) < 3000)
    {
      getHeadMotionRequest().id = HeadMotionRequest::look_at_world_point;
      getHeadMotionRequest().targetPointInTheWorld.x = getBallModel().position.x;
      getHeadMotionRequest().targetPointInTheWorld.y = getBallModel().position.y;
      getHeadMotionRequest().targetPointInTheWorld.z = getFieldInfo().ballRadius;
    }
  );

  DEBUG_REQUEST("BasicTestBehavior:head:look_at_world_point",
    double x = 1000;
    double y = 0;
    double z = 0;
    MODIFY("BasicTestBehavior:head:world_point:x",x);
    MODIFY("BasicTestBehavior:head:world_point:y",y);
    MODIFY("BasicTestBehavior:head:world_point:z",z);
    getHeadMotionRequest().id = HeadMotionRequest::look_at_world_point;
    getHeadMotionRequest().targetPointInTheWorld = Vector3d(x,y,z);
  );
}//end testHead

void BasicTestBehavior::testMotion()
{
  getMotionRequest().walkRequest.target = Pose2D();
  getMotionRequest().forced = false;

  DEBUG_REQUEST("BasicTestBehavior:motion:id:stand",
    getMotionRequest().id = motion::stand;
  );

  getMotionRequest().standardStand = false;
  DEBUG_REQUEST("BasicTestBehavior:motion:standard_stand",
    getMotionRequest().standardStand = true;
      getMotionRequest().standHeight = -1; // minus means the same value as walk
  );

  DEBUG_REQUEST("BasicTestBehavior:motion:id:sit",
    getMotionRequest().id = motion::sit;
  );

  DEBUG_REQUEST("BasicTestBehavior:motion:walk:forward",
    getMotionRequest().id = motion::walk;
    getMotionRequest().walkRequest.target.translation.x = 500;
  );

  DEBUG_REQUEST("BasicTestBehavior:motion:walk:backward",
    getMotionRequest().id = motion::walk;
    getMotionRequest().walkRequest.target.translation.x = -500;
  );

  DEBUG_REQUEST("BasicTestBehavior:motion:walk:strafe_right",
    getMotionRequest().id = motion::walk;
    getMotionRequest().walkRequest.target.translation.y = -500;
  );

  DEBUG_REQUEST("BasicTestBehavior:motion:walk:strafe_left",
    getMotionRequest().id = motion::walk;
    getMotionRequest().walkRequest.target.translation.y = 500;
  );

  DEBUG_REQUEST("BasicTestBehavior:motion:walk:turn_right",
    getMotionRequest().id = motion::walk;
    getMotionRequest().walkRequest.target.rotation = Math::fromDegrees(-60);
  );

  DEBUG_REQUEST("BasicTestBehavior:motion:walk:turn_left",
    getMotionRequest().id = motion::walk;
    getMotionRequest().walkRequest.target.rotation = Math::fromDegrees(60);
  );

  DEBUG_REQUEST("BasicTestBehavior:motion:walk:stepping",
      getMotionRequest().id = motion::walk;
      getMotionRequest().walkRequest.target = Pose2D();
    );

  DEBUG_REQUEST("BasicTestBehavior:motion:step_control",
   if ( getMotionStatus().stepControl.stepID % 5 == 0){
    getMotionRequest().walkRequest.stepControl.stepID = getMotionStatus().stepControl.stepID;
    switch(getMotionStatus().stepControl.moveableFoot){
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

  DEBUG_REQUEST("BasicTestBehavior:motion:walk_style:normal",
              getMotionRequest().walkRequest.character = 0.5;
  );
  DEBUG_REQUEST("BasicTestBehavior:motion:walk_style:fast",
              getMotionRequest().walkRequest.character = 1;
  );
  DEBUG_REQUEST("BasicTestBehavior:motion:walk_style:stable",
              getMotionRequest().walkRequest.character = 0;
  );

  DEBUG_REQUEST("BasicTestBehavior:motion:walk_coordinate:hip",
              getMotionRequest().walkRequest.coordinate = WalkRequest::Hip;
  );
  DEBUG_REQUEST("BasicTestBehavior:motion:walk_coordinate:LFoot",
              getMotionRequest().walkRequest.coordinate = WalkRequest::LFoot;
  );
  DEBUG_REQUEST("BasicTestBehavior:motion:walk_coordinate:RFoot",
              getMotionRequest().walkRequest.coordinate = WalkRequest::RFoot;
  );

  double offsetR = 0;
  MODIFY("walk.offset.r", offsetR);
  getMotionRequest().walkRequest.offset.rotation = Math::fromDegrees(offsetR);
  MODIFY("walk.offset.x", getMotionRequest().walkRequest.offset.translation.x);
  MODIFY("walk.offset.y", getMotionRequest().walkRequest.offset.translation.y);
  MODIFY("walk.character", getMotionRequest().walkRequest.character);

  DEBUG_REQUEST("BasicTestBehavior:motion:StandUp:from_front",
    getMotionRequest().id = motion::stand_up_from_front;
  );

  DEBUG_REQUEST("BasicTestBehavior:motion:StandUp:from_back",
    getMotionRequest().id = motion::stand_up_from_back;
  );

  DEBUG_REQUEST("BasicTestBehavior:motion:StandUp:from_back_arms_back",
    getMotionRequest().id = motion::stand_up_from_back_arms_back;
  );

  DEBUG_REQUEST("BasicTestBehavior:motion:id:dead",
    getMotionRequest().id = motion::dead;
    getMotionRequest().forced = true;
  );

  DEBUG_REQUEST("BasicTestBehavior:motion:id:init",
    getMotionRequest().id = motion::init;
  );

  DEBUG_REQUEST("BasicTestBehavior:motion:id:dance",
    getMotionRequest().id = motion::dance;
  );

  DEBUG_REQUEST("BasicTestBehavior:motion:id:protect_falling",
    getMotionRequest().id = motion::protect_falling;
  );

  DEBUG_REQUEST("BasicTestBehavior:motion:id:play_editor_motionnet",
    getMotionRequest().id = motion::play_editor_motionnet;
  );

  DEBUG_REQUEST("BasicTestBehavior:motion:force",
    getMotionRequest().forced = true;
  );

  DEBUG_REQUEST("BasicTestBehavior:motion:ParallelKinematik:stepper",
    getMotionRequest().id = motion::parallel_stepper;
  );
  DEBUG_REQUEST("BasicTestBehavior:motion:ParallelKinematik:dance",
    getMotionRequest().id = motion::parallel_dance;
  );


  DEBUG_REQUEST("BasicTestBehavior:motion:id:block_middle",
    getMotionRequest().id = motion::block_middle;
  );
  DEBUG_REQUEST("BasicTestBehavior:motion:id:goalie_get_down",
    getMotionRequest().id = motion::goalie_get_down;
  );
  DEBUG_REQUEST("BasicTestBehavior:motion:id:goalie_get_fast_down",
    getMotionRequest().id = motion::goalie_get_fast_down;
  );
  DEBUG_REQUEST("BasicTestBehavior:motion:id:goalie_sit",
    getMotionRequest().id = motion::goalie_sit;
  );
  DEBUG_REQUEST("BasicTestBehavior:motion:id:goalie_sit_block",
    getMotionRequest().id = motion::goalie_sit_block;
  );
  DEBUG_REQUEST("BasicTestBehavior:motion:id:goalie_sit_small",
    getMotionRequest().id = motion::goalie_sit_small;
  );
  DEBUG_REQUEST("BasicTestBehavior:motion:id:goalie_sit_n_dive_left",
    getMotionRequest().id = motion::goalie_sit_n_dive_left;
  );
  DEBUG_REQUEST("BasicTestBehavior:motion:id:goalie_sit_n_dive_right",
    getMotionRequest().id = motion::goalie_sit_n_dive_right;
  );

  DEBUG_REQUEST("BasicTestBehavior:motion:id:jump_indicator_left",
    getMotionRequest().id = motion::jump_indicator_left;
  );

  DEBUG_REQUEST("BasicTestBehavior:motion:id:jump_indicator_right",
    getMotionRequest().id = motion::jump_indicator_right;
  );

}//end testMotion

void BasicTestBehavior::testLED() {

  //init some Mono LEDs
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

void BasicTestBehavior::testAudio()
{
  getAudioControl().capture = false;

  DEBUG_REQUEST("BasicTestBehavior:whistle:listen",
    getAudioControl().capture = true;

    if(getWhistlePercept().whistleDetected) {
      getSoundPlayData().soundFile = "victory.wav";
    } else {
      getSoundPlayData().soundFile.clear();
    }
  );

  DEBUG_REQUEST("BasicTestBehavior:audio:capture",
    getAudioControl().capture = true;
  );
}