/**
 * @file SensorBehaviorControl.cpp
 *
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
 * @author <a href="mailto:goehring@informatik.hu-berlin.de">Daniel Goehring</a>
 * Implementation of class SensorBehaviorControl
 */

#include "SensorBehaviorControl.h"

#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugModify.h"

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
  getMotionRequest().forced = false;
  getMotionRequest().standHeight = -1; // sit in a stable position

  // init if necessary
  if( getMotionStatus().lastMotion == motion::empty &&
      getMotionStatus().currentMotion != motion::init )
    getMotionRequest().id = motion::init;
  else
    getMotionRequest().id = motion::stand;


  // execute the test behavior
  testBehavior();

  // perform the kick experiment
  kickExercise();

  // play some test sound :)
  getSoundPlayData().soundFile = "";
  DEBUG_REQUEST("SensorBehaviorControl:sound",
    getSoundPlayData().soundFile = "beebeep.wav";
  );
}//end execute


void SensorBehaviorControl::testBehavior() 
{
  double kickDirection = 0.0;
  MODIFY("SensorBehaviorControl:KickExercise:kickDirection", kickDirection);

  DEBUG_REQUEST("SensorBehaviorControl:behavior:goto_ball",
    int timeSinceBallWasSeen = getFrameInfo().getTimeSince(getBallPercept().frameInfoWhenBallWasSeen.getTime());

    if (timeSinceBallWasSeen < 1000) 
    {
      // look at the seen ball
      getHeadMotionRequest().id = HeadMotionRequest::look_at_world_point;
      getHeadMotionRequest().targetPointInTheWorld.x = getBallPercept().bearingBasedOffsetOnField.x;
      getHeadMotionRequest().targetPointInTheWorld.y = getBallPercept().bearingBasedOffsetOnField.y;
      getHeadMotionRequest().targetPointInTheWorld.z = getFieldInfo().ballRadius;

      // calculate the previewd position of the seen ball in the left foot coordinates
      const Pose3D& lFoot = getKinematicChain().theLinks[KinematicChain::LFoot].M;
      Pose2D lFootPose(lFoot.rotation.getZAngle(), lFoot.translation.x, lFoot.translation.y);
      Vector2<double> ballLeftFoot  = lFootPose/getBallPercept().bearingBasedOffsetOnField;
      Vector2<double> ballLeftFootPreview = getMotionStatus().plannedMotion.lFoot / ballLeftFoot;

      // draw ball
      //FIELD_DRAWING_CONTEXT;
      //PEN("FF0000", 20);
      //CIRCLE(ballLeftFootPreview.x, ballLeftFootPreview.y, 45);

      double approachDistance = 120 + getFieldInfo().ballRadius;
      double diffX = ballLeftFootPreview.x - approachDistance;

      // set the motion request
      getMotionRequest().id = motion::walk;
      getMotionRequest().walkRequest.target = Pose2D(ballLeftFootPreview.angle(), diffX * 0.7, ballLeftFootPreview.y);
      getMotionRequest().walkRequest.coordinate = WalkRequest::LFoot;

      DEBUG_REQUEST("SensorBehaviorControl:behavior:kick",
        if (diffX > -getFieldInfo().ballRadius && diffX < 10 &&
            fabs(getMotionRequest().walkRequest.target.translation.y) < 10 &&
            fabs(getMotionRequest().walkRequest.target.rotation) < Math::fromDegrees(10)
            )
        {
          getMotionRequest().id = motion::kick;
          getMotionRequest().kickRequest.kickPoint.x = getBallPercept().bearingBasedOffsetOnField.x;
          getMotionRequest().kickRequest.kickPoint.y = getBallPercept().bearingBasedOffsetOnField.y;
          getMotionRequest().kickRequest.kickPoint.z = getFieldInfo().ballRadius;
          getMotionRequest().kickRequest.kickFoot = KickRequest::right;
          getMotionRequest().kickRequest.finishKick = true;
          getMotionRequest().kickRequest.kickDirection = Math::fromDegrees(kickDirection);
        }
      );
    } 
    else 
    {
      getHeadMotionRequest().id = HeadMotionRequest::hold;
      getMotionRequest().id = motion::stand;

      DEBUG_REQUEST("SensorBehaviorControl:behavior:search_ball",
        getMotionRequest().id = motion::walk;
        getMotionRequest().walkRequest.target = Pose2D(Math::fromDegrees(20), 0, 0);
        getMotionRequest().walkRequest.coordinate = WalkRequest::Hip;

        getHeadMotionRequest().id = HeadMotionRequest::search;
        getHeadMotionRequest().searchCenter = Vector3d(2000, 0, 0);
        getHeadMotionRequest().searchSize = Vector3d(800, 2000, 0);
      );
    }
  );


  DEBUG_REQUEST("SensorBehaviorControl:behavior:walk_cycle",
    getMotionRequest().id = motion::walk;
    getMotionRequest().walkRequest.target = Pose2D(Math::fromDegrees(20), 100, 0);
    getMotionRequest().walkRequest.coordinate = WalkRequest::Hip;
  );


  DEBUG_REQUEST("SensorBehaviorControl:behavior:stability_test",
    static unsigned int last_time = getFrameInfo().getTime();
    getMotionRequest().id = motion::walk;
    getMotionRequest().walkRequest.coordinate = WalkRequest::Hip;

    if(last_time < 3000)
    {
      getMotionRequest().walkRequest.target = Pose2D(0, 1000, 0);
    }else
    {
      getMotionRequest().walkRequest.target = Pose2D(Math::fromDegrees(90), 1000, 0);
    }
  );
}//end testBehavior

void SensorBehaviorControl::kickExercise() 
{

  double kickDirection = 0.0;
  MODIFY("SensorBehaviorControl:KickExercise:kickDirection", kickDirection);

  DEBUG_REQUEST("SensorBehaviorControl:motion:kick",
          int timeSinceBallWasSeen = getFrameInfo().getTimeSince(getBallPercept().frameInfoWhenBallWasSeen.getTime());

    if (timeSinceBallWasSeen < 1000 &&
        fabs(getBallPercept().bearingBasedOffsetOnField.x) < 500 &&
        fabs(getBallPercept().bearingBasedOffsetOnField.y) < 500) 
    {
      getMotionRequest().id = motion::kick;
      getMotionRequest().kickRequest.kickPoint.x = getBallPercept().bearingBasedOffsetOnField.x;
      getMotionRequest().kickRequest.kickPoint.y = getBallPercept().bearingBasedOffsetOnField.y;
      getMotionRequest().kickRequest.kickPoint.z = getFieldInfo().ballRadius;
      getMotionRequest().kickRequest.finishKick = false;
      getMotionRequest().kickRequest.kickDirection = Math::fromDegrees(kickDirection);
      getMotionRequest().kickRequest.kickFoot = KickRequest::right;
      DEBUG_REQUEST("SensorBehaviorControl:motion:left_kick",
        getMotionRequest().kickRequest.kickFoot = KickRequest::left;
      );

      DEBUG_REQUEST("SensorBehaviorControl:motion:finish_kick",
        getMotionRequest().kickRequest.finishKick = true;
      );
    }
  );
}//end kickExercise


