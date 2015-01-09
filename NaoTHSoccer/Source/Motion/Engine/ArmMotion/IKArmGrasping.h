/**
 * @file:  IKArmGrasping.h
 * @author: peppone
 * @author: <a href="mailto:stadie@informatik.hu-berlin.de">Oliver Stadie</a>
 * @author: <a href="mailto:scheunem@informatik.hu-berlin.de">Marcus Scheunemann</a>
 *
 * First Created on 5 luglio 2010, 18.01
 */

#ifndef _IKArmGrasping_H
#define _IKArmGrasping_H

#include <ModuleFramework/Module.h>

//#include "Motion/AbstractMotion.h"
#include "Motion/Engine/InverseKinematicsMotion/InverseKinematicsMotionEngine.h"

// representations
#include <Representations/Infrastructure/RobotInfo.h>
#include <Representations/Infrastructure/FrameInfo.h>
#include "Representations/Motion/Request/MotionRequest.h"
#include <Representations/Infrastructure/JointData.h>
#include "Representations/Modeling/KinematicChain.h"

// Tools
#include "Tools/Math/Common.h"
#include "Tools/Math/Pose3D.h"
#include "Tools/NaoInfo.h"
#include "Tools/ReachibilityGrid.h"

// Debug
#include "Tools/Debug/DebugModify.h"
#include "Tools/Debug/DebugDrawings3D.h"
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugPlot.h" //For Plotting

BEGIN_DECLARE_MODULE(IKArmGrasping)
  PROVIDE(DebugModify)
  PROVIDE(DebugDrawings3D)
  PROVIDE(DebugRequest)
  PROVIDE(DebugPlot)


  REQUIRE(RobotInfo)
  REQUIRE(FrameInfo)
  REQUIRE(SensorJointData)
  REQUIRE(MotionRequest)
  REQUIRE(KinematicChainMotor)
  REQUIRE(InverseKinematicsMotionEngineService)
  
  PROVIDE(MotorJointData)
END_DECLARE_MODULE(IKArmGrasping)

class IKArmGrasping: public IKArmGraspingBase
{

public:

  IKArmGrasping();
  virtual ~IKArmGrasping();

  void calculateTrajectory(const MotionRequest& motionRequest);
  virtual void execute();

private:
  int initialTime;

  motion::State currentState;
  //ReachibilityGrid _pure_reachability_grid;

  //TransformedReachibilityGrid theReachibilityGridRight;
  //TransformedReachibilityGrid theReachibilityGridLeft;

  InverseKinematicsMotionEngine& getEngine() const {
    return getInverseKinematicsMotionEngineService().getEngine();
  }

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

