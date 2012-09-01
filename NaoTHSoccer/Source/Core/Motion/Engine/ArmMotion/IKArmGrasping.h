/**
 * @file:  IKArmGrasping.h
 * @author: peppone
 * @author: <a href="mailto:stadie@informatik.hu-berlin.de">Oliver Stadie</a>
 * @author: <a href="mailto:scheunem@informatik.hu-berlin.de">Marcus Scheunemann</a>
 *
 * First Created on 5 luglio 2010, 18.01
 */

#ifndef _IKArmGrasping_H
#define  _IKArmGrasping_H

#include "Motion/MotionBlackBoard.h"
#include "Motion/AbstractMotion.h"
#include "Motion/Engine/InverseKinematicsMotion/InverseKinematicsMotionEngine.h"

// Tools
#include "Tools/Math/Common.h"
#include "Tools/Math/Pose3D.h"
#include "Tools/NaoInfo.h"
#include "Tools/ReachibilityGrid.h"


class IKArmGrasping: public AbstractMotion 
{

public:

  IKArmGrasping();
  virtual ~IKArmGrasping();

  void calculateTrajectory(const MotionRequest& motionRequest);
  virtual void execute(const MotionRequest& motionRequest, MotionStatus& moitonStatus);

private:
  InverseKinematicsMotionEngine& theEngine;
  int initialTime;
  //ReachibilityGrid _pure_reachability_grid;

  //TransformedReachibilityGrid theReachibilityGridRight;
  //TransformedReachibilityGrid theReachibilityGridLeft;


  static const Vector3<double> defaultGraspingCenter;
  Vector3<double> graspingCenter;
  double ratio;
  static const double maxHandDistance;
  InverseKinematic::ChestArmsPose initialPose;

  // the currently executed pose
  InverseKinematic::ChestArmsPose theCurrentPose;


  // basic tools
  void setShoulderPitchStiffness(double value);
  void setArmStiffness(double value);
  static void hug(InverseKinematic::ChestArmsPose& p, const Vector3<double>& graspingCenter, double ratio);

  // controller
  double thresholdController(double input, double threshold, double step, bool reset = false);
  double integralController(double input, double outputDesired, double weight, bool reset = false);


  //
  void test_roundtrip_delay();

  //Debug
  void debugDraw3D();
  void drawReachabilityGrid() const;
};

#endif  /* _IKArmGrasping_H */

