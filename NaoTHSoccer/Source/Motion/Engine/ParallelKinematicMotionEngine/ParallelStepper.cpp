/**
 * @file ParallelStepper.h
 *
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Xu, Yuan</a>
 */

#include "ParallelStepper.h"
#include "Tools/NaoInfo.h"

#include "Tools/Debug/DebugModify.h"
#include "Tools/Debug/DebugBufferedOutput.h"

ParallelStepper::ParallelStepper() 
  :
  AbstractMotion(motion::parallel_stepper, getMotionLock()),
  shift(0),
  speed(0),
  t(0)
{
  
}

void ParallelStepper::execute()
{
  if(getMotionRequest().id != getId())
  {
    setCurrentState(motion::stopped);
    return;
  }//end if

  // some parameters
  double max_shift = 25; // maximal radius of the motion in mm
  MODIFY("DanceMotion:max_shift", max_shift);

  double max_shift_speed = 100; // maximal speed of the motion in degrees / second
  MODIFY("DanceMotion:max_shift_speed", max_shift_speed);

  // increase or decrease the current max_shift slowly
  double shift_step = 1;
  if (shift < max_shift - shift_step) shift += shift_step;
  if (shift > max_shift + shift_step) shift -= shift_step;

  // increase or decrease the current speed slowly
  double speed_step = 1;
  if (speed < max_shift_speed - speed_step) speed += speed_step;
  if (speed > max_shift_speed + speed_step) speed -= speed_step;

  // increase the current time
  double s = speed*getRobotInfo().getBasicTimeStepInSecond();
  t += Math::fromDegrees(s);


  double z = 210;
  MODIFY("ParallelStepper:z", z);
  // calculate the height of the robot in mm
  z -= NaoInfo::FootHeight;
  Math::clamp(z, 0.0, NaoInfo::ThighLength + NaoInfo::TibiaLength);

  double alpha_x = 0.0;
  MODIFY("ParallelStepper:alpha_x", alpha_x);

  double height = 0.0;
  MODIFY("ParallelStepper:height", height);



  // hip trajectory
  double y = sin(t) * shift;

  // feet trajectory
  double foot_shift = 0.0;
  // t \in [pi,2pi]
  double foot_height_left = height * (1.0-cos(2.0*(t + Math::pi_2 + foot_shift)))*0.5;
  // t \in [0,pi]
  double foot_height_right = height* (1.0-cos(2.0*(t - Math::pi_2 + foot_shift)))*0.5;


  double alpha_z_left  = Math::pi - 2.0*asin((z-foot_height_left )/(NaoInfo::ThighLength + NaoInfo::TibiaLength));
  double alpha_z_right = Math::pi - 2.0*asin((z-foot_height_right)/(NaoInfo::ThighLength + NaoInfo::TibiaLength));

  double alpha_y = asin(y/z);


  Kinematics::ParallelKinematic::Pose poseLeft;
  poseLeft.alpha.x = alpha_x;
  poseLeft.alpha.y = alpha_y;
  poseLeft.alpha.z = alpha_z_left;


  Kinematics::ParallelKinematic::Pose poseRight;
  poseRight.alpha.x = alpha_x;
  poseRight.alpha.y = alpha_y;
  poseRight.alpha.z = alpha_z_right;

  PLOT("ParallelStepper:alpha_y", alpha_y);
  PLOT("ParallelStepper:poseRight.alpha.z",poseRight.alpha.z);
  PLOT("ParallelStepper:poseLeft.alpha.z",poseLeft.alpha.z);

  // apply parallel kinematics to calculate the leg joints
  theParallelKinematic.calculateLegs(poseLeft, poseRight, getMotorJointData());


  setCurrentState(motion::running);
}//end execute
