/**
* @file ParallelDance.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Xu, Yuan</a>
*/

#ifndef _ParallelDance_H
#define	_ParallelDance_H

#include "Motion/AbstractMotion.h"
#include "Motion/MorphologyProcessor/ParallelKinematic.h"

class ParallelDance : public AbstractMotion
{
public:
  ParallelDance();
  
  virtual ~ParallelDance(){}

  virtual void execute(const MotionRequest& motionRequest, MotionStatus& /*motionStatus*/);
  
private:
  double radius; // current radius of the circle in mm
  double speed; // current speed of the motion in degreees
  double t; // current time

  Kinematics::ParallelKinematic theParallelKinematic;

  double taylorSine(double num, double precision = 19);
  double factorial(int num);
};

#endif	/* _ParallelDance_H */

