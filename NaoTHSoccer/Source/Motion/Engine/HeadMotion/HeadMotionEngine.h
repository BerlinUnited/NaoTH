/**
 * @file HeadMotionEngine.h
 *
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
 * @author Oliver Welter
 *
 * Created on 27. Januar 2009, 16:43
 */

#ifndef _HeadMotionEngine_H
#define _HeadMotionEngine_H

#include <Tools/Math/Vector2.h>
#include <Tools/Math/Vector3.h>
#include <vector>

#include <ModuleFramework/Module.h>

// representations
#include <Representations/Infrastructure/FrameInfo.h> // needed for debug plot
#include <Representations/Infrastructure/RobotInfo.h>
#include <Representations/Infrastructure/CameraInfo.h>
#include "Representations/Modeling/InertialModel.h"
#include "Representations/Infrastructure/InertialSensorData.h"
#include "Representations/Modeling/KinematicChain.h"
#include "Representations/Motion/Request/HeadMotionRequest.h"
#include <Representations/Infrastructure/JointData.h>
#include <Representations/Perception/CameraMatrix.h>
#include "Representations/Motion/MotionStatus.h"
#include <Representations/Modeling/CameraMatrixOffset.h>

#include "Tools/DataStructures/RingBufferWithSum.h"
#include "Tools/DataStructures/Spline.h"

// Debug
#include "Tools/Debug/DebugModify.h"
#include <Tools/Debug/DebugRequest.h>
#include <Tools/Debug/DebugPlot.h>
#include <Tools/Debug/DebugDrawings.h>
#include "Tools/Debug/DebugParameterList.h"

BEGIN_DECLARE_MODULE(HeadMotionEngine)
  PROVIDE(DebugModify)
  PROVIDE(DebugRequest)
  PROVIDE(DebugDrawings)
  PROVIDE(DebugParameterList)
  PROVIDE(DebugPlot)

  REQUIRE(RobotInfo)
  REQUIRE(InertialModel)
  REQUIRE(InertialSensorData)
  REQUIRE(KinematicChainSensor)
  REQUIRE(HeadMotionRequest)
  REQUIRE(SensorJointData)
  REQUIRE(FrameInfo)

  REQUIRE(CameraMatrix)
  REQUIRE(CameraMatrixTop)
  REQUIRE(CameraInfo)
  REQUIRE(CameraInfoTop)
  REQUIRE(CameraMatrixOffset)

  PROVIDE(MotionStatus)
  PROVIDE(MotorJointData)
END_DECLARE_MODULE(HeadMotionEngine)


class HeadMotionEngine: private HeadMotionEngineBase
{
public:
  HeadMotionEngine();
  ~HeadMotionEngine();

  void execute();

private:
  struct Parameters: public ParameterList
  {
    Parameters() : ParameterList("HeadMotionEngine")
    {
      PARAMETER_REGISTER(max_velocity_deg_in_second_fast) = 60;
      PARAMETER_REGISTER(max_velocity_deg_in_second_slow) = 90;
      PARAMETER_REGISTER(cutting_velocity) = 40;

      PARAMETER_ANGLE_REGISTER(at_rest_threshold) = 0.3;
      PARAMETER_ANGLE_REGISTER(at_rest_threshold_walking) = 10;
      PARAMETER_ANGLE_REGISTER(at_target_threshold) = 3;

      PARAMETER_REGISTER(stiffness) = 0.7;

      syncWithConfig();
    }

    double max_velocity_deg_in_second_fast;
    double max_velocity_deg_in_second_slow;
    double cutting_velocity;

    double at_rest_threshold;
    double at_rest_threshold_walking;
    double at_target_threshold;

    double stiffness;
  } params;

private:

  // HACK: limits.
  // TODO: make them parameters. (as soon as we have a config system that allows loading vectors :)
  // NOTE: V4, V5, V6 have the same limits
  // http://doc.aldebaran.com/2-8/family/nao_technical/joints_naov6.html#naov6-joints-head-joints
  // http://doc.aldebaran.com/2-1/family/robots/joints_robot.html
  std::vector<double> headLimitsHeadYaw  = 
  {
    -2.086017,
    -1.526988,
    -1.089958,
    -0.903033,
    -0.756077,
    -0.486074,
     0.000000,
     0.486074,
     0.756077,
     0.903033,
     1.089958,
     1.526988,
     2.086017
  };

  std::vector<double> headLimitsHeadPitchMin = 
  {
    -0.449073,
    -0.330041,
    -0.430049,
    -0.479965,
    -0.548033,
    -0.671951,
    -0.671951,
    -0.671951,
    -0.548033,
    -0.479965,
    -0.430049,
    -0.330041,
    -0.449073  
  };
  
  std::vector<double> headLimitsHeadPitchMax = 
  {
    0.330041,
    0.200015,
    0.300022,
    0.330041,
    0.370010,
    0.422021,
    0.515047,
    0.422021,
    0.370010,
    0.330041,
    0.300022,
    0.200015,
    0.330041
  };
  
  tk::spline headLimitFunctionMin;
  tk::spline headLimitFunctionMax;

  // internal use
  naoth::JointData theJointData;
  KinematicChain theKinematicChain;

  bool trajectoryHeadMove(const std::vector<Vector3d>& points);
  void gotoPointOnTheGround(const Vector2d& target);

  void lookStraightAhead();
  void lookStraightAheadWithStabilization();
  void lookAtPoint();
  void simpleLookAtPoint();

  void hold();
  void search();
  void randomSearch();

  void moveByAngle(const Vector2d& target);
  void gotoAngle(const Vector2d& target);

  void lookAtWorldPointCool(const Vector3d& target);
  void lookAtWorldPoint(const Vector3d& target);
  void lookAtWorldPointSimple(const Vector3d& target);

  Vector3d g(double yaw, double pitch, const Vector3d& pointInWorld);
  void export_g();

  // for providing head_target_reached and head_got_stuck
  Vector2d motion_target, last_motion_target;
  HeadMotionRequest::HeadMotionID last_id;
  RingBufferWithSum<Vector2d, 10> absolute_velocity_buffer;
};

#endif  /* _HeadMotionEngine_H */

