/**
 * @file ParallelDance.h
 *
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Xu, Yuan</a>
 */

#include "ParallelDance.h"

#include "Tools/Debug/DebugModify.h"
#include "Tools/Debug/DebugBufferedOutput.h"

using namespace naoth;

ParallelDance::ParallelDance() 
  :
  AbstractMotion(motion::parallel_dance),
  radius(0),
  speed(0),
  t(0)
{
  
}

void ParallelDance::execute(const MotionRequest& motionRequest, MotionStatus& /*motionStatus*/)
{
  if(motionRequest.id != getId())
  {
    currentState = motion::stopped;
    return;
  }//end if

  // some parameters
  double max_dance_radius = 0.25; // maximal radius of the motion in mm
  MODIFY("DanceMotion:max_dance_radius", max_dance_radius);
  double max_dance_speed = 100; // maximal speed of the motion in degrees / second
  MODIFY("DanceMotion:max_dance_speed", max_dance_speed);

  // increase or decrease the current radius slowly
  double radius_step = 0.001;
  if (radius < max_dance_radius - radius_step) radius += radius_step;
  if (radius > max_dance_radius + radius_step) radius -= radius_step;

  // increase or decrease the current speed slowly
  double speed_step = 1;
  if (speed < max_dance_speed - speed_step) speed += speed_step;
  if (speed > max_dance_speed + speed_step) speed -= speed_step;

  // increase the current time
  double s = speed*theBlackBoard.theRobotInfo.getBasicTimeStepInSecond();
  t += Math::fromDegrees(s);

  double alpha_z = 0.8;
  MODIFY("ParallelDance:alpha_z", alpha_z);

  double alpha_x = 0.0;
  alpha_x = cos(t) * radius;
  MODIFY("ParallelDance:alpha_x", alpha_x);

  double alpha_y = 0.0;
  alpha_y = sin(t) * radius;
  MODIFY("ParallelDance:alpha_y", alpha_y);

  double bs = Math::fromDegrees(s);
  PLOT("a:bs",bs);
  PLOT("a:alpha_y",alpha_y);

  Kinematics::ParallelKinematic::Pose pose;
  pose.alpha.x = alpha_x;
  pose.alpha.y = alpha_y;
  pose.alpha.z = alpha_z;

  // apply parallel kinematics to calculate the leg joints
  theParallelKinematic.calculateLegs(pose, theMotorJointData);

  for(int i = JointData::RHipPitch; i < JointData::numOfJoint; i++)
  {
    PLOT(JointData::getJointName((JointData::JointID)i), (theMotorJointData.position[i]));
  }

  currentState = motion::running;
}//end execute

double ParallelDance::factorial(int num){
        double fact = 1;
        for(int i = 1; i <= num; i++){
                fact *= i;
        }
        return fact;
}
 
//Taylor series sin(x) = (-1)^n*x^(2n+1)/(2n+1)!
//any precision less then 19 and your values will start to skew
double ParallelDance::taylorSine(double num, double precision){
        double value = 0;
        for(int n = 0; n < precision; n++){
                value += pow(-1.0, n) * pow(num, 2*n+1) / factorial(2*n + 1);
        }
        return value;
}
