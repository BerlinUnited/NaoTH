/**
 * @file ParallelStepper.h
 *
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Xu, Yuan</a>
 */

#include "ParallelStepper.h"
#include "Tools/NaoInfo.h"


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
  }

  // some parameters
  double max_shift = 25; // maximal radius of the motion in mm
  MODIFY("ParallelStepper:max_shift", max_shift);

  double max_shift_speed = 100; // maximal speed of the motion in degrees / second
  MODIFY("ParallelStepper:max_shift_speed", max_shift_speed);

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

  // y > 0 ==> left, y < 0 ==> right
  // feet trajectory
  double foot_height_left = height * (1.0-cos(2.0*t))*0.5;
  double foot_height_right = height* (1.0-cos(2.0*t))*0.5;

  if(y < 0) { // right
    foot_height_right = 0;
  } else {
    foot_height_left = 0;
  }

  double alpha_z_left  = Math::pi - 2.0*asin((z-foot_height_left )/(NaoInfo::ThighLength + NaoInfo::TibiaLength));
  double alpha_z_right = Math::pi - 2.0*asin((z-foot_height_right)/(NaoInfo::ThighLength + NaoInfo::TibiaLength));

  double alpha_y = asin(y/z);


  // x-motion for feet
  double step_length = 0;
  MODIFY("ParallelStepper:step_length", step_length);

  double alpha_x_left = step_length * sin(t+ Math::pi_2);
  double alpha_x_right = step_length * sin(t + Math::pi+Math::pi_2);


  Kinematics::ParallelKinematic::Pose poseLeft;
  poseLeft.alpha.x = alpha_x_left;
  poseLeft.alpha.y = alpha_y;
  poseLeft.alpha.z = alpha_z_left;


  Kinematics::ParallelKinematic::Pose poseRight;
  poseRight.alpha.x = alpha_x_right;
  poseRight.alpha.y = alpha_y;
  poseRight.alpha.z = alpha_z_right;

  PLOT("ParallelStepper:alpha_y", alpha_y);
  PLOT("ParallelStepper:poseRight.alpha.z",poseRight.alpha.z);
  PLOT("ParallelStepper:poseLeft.alpha.z",poseLeft.alpha.z);

  // apply parallel kinematics to calculate the leg joints
  theParallelKinematic.calculateLegs(poseLeft, poseRight, getMotorJointData());

  setCurrentState(motion::running);
}//end execute
