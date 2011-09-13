/**
 * @file Dance.cpp
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 * @brief a test motion
 *
 */

#ifndef DANCE_H
#define  DANCE_H

#include "IKMotion.h"

class DanceMotion: public IKMotion
{
public:

  DanceMotion()
  : IKMotion(motion::dance),
    radius(0),
    speed(0),
    t(0)
  {
  }

  void calculateTrajectory(const MotionRequest& motionRequest)
  {
    currentState = motion::running; // set the motion to running by default

    //-- BEGIN -- generate the trajectory --//

    // some parameters
    double max_dance_radius = 30; // maximal radius of the motion in mm
    //MODIFY("DanceMotion:max_dance_radius", max_dance_radius);
    double max_dance_speed = 100; // maximal speed of the motion in degrees / second
    //MODIFY("DanceMotion:max_dance_speed", max_dance_speed);

    // stop the motion if it is not requested
    if (motionRequest.id != getId()) {
      max_dance_radius = 0;
      max_dance_speed = 0;
    }//end if

    // increase or decrease the current radius slowly
    double dr = max_dance_radius - radius;
    dr = Math::clamp(dr, -0.1, 0.1);
    radius += dr;

    // increase or decrease the current speed slowly
    double ds = max_dance_speed - speed;
    ds = Math::clamp(ds, -1.0, 1.0);
    speed += ds;

    if (motionRequest.id != getId()
      && abs(speed) < 1.0 && radius < 0.1 ) {
      speed = 0;
      radius = 0;
      currentState = motion::stopped;
    }

    // increase the current time
    t += Math::fromDegrees(speed * theBlackBoard.theRobotInfo.getBasicTimeStepInSecond());
    
    // calculate the xy-coordinates for the chest
    p.com.translation.x = cos(t) * radius;
    p.com.translation.y = sin(t) * radius;

    // plot the generated trajectory
    //PLOT("DanceMotion:x",p.chest.translation.x);
    //PLOT("DanceMotion:y",p.chest.translation.y);

    //-- END -- generate the trajectory --//

  }//end calculateTrajectory

  virtual void execute(const MotionRequest& motionRequest, MotionStatus& /*motionStatus*/)
  {
    if ( currentState != motion::running ) {
      // create the initial pose of the robot
      p = theEngine.getCurrentCoMFeetPose();
      p.localInCoM();
    }

    calculateTrajectory(motionRequest);
    InverseKinematic::HipFeetPose c = theEngine.controlCenterOfMass(p);
    theEngine.solveHipFeetIK(c);
    theEngine.copyLegJoints(theMotorJointData.position);
  }
  
private:
  double radius; // current radius of the circle in mm
  double speed; // current speed of the motion in degreees
  double t; // current time
  InverseKinematic::CoMFeetPose p; // the basic pose
};

#endif  /* DANCE_H */

