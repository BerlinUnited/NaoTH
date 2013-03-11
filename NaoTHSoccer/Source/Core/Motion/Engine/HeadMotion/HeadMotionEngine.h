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

#include <ModuleFramework/Module.h>

// representations
#include <Representations/Infrastructure/RobotInfo.h>
#include <Representations/Infrastructure/CameraInfo.h>
#include "Representations/Modeling/InertialModel.h"
#include "Representations/Modeling/KinematicChain.h"
#include "Representations/Motion/Request/HeadMotionRequest.h"
#include <Representations/Infrastructure/JointData.h>
#include <Representations/Perception/CameraMatrix.h>
#include "Representations/Motion/MotionStatus.h"

BEGIN_DECLARE_MODULE(HeadMotionEngine)
  REQUIRE(RobotInfo)
  REQUIRE(CameraInfo)
  REQUIRE(InertialModel)
  REQUIRE(KinematicChainSensor)
  REQUIRE(HeadMotionRequest)
  REQUIRE(SensorJointData)
  REQUIRE(CameraMatrix)

  PROVIDE(MotionStatus)
  PROVIDE(MotorJointData)
END_DECLARE_MODULE(HeadMotionEngine)


class HeadMotionEngine: private HeadMotionEngineBase
{
public:
  /** constructor */
  HeadMotionEngine();
  ~HeadMotionEngine(){};

  void execute();

private:

  // internal use
  naoth::JointData theJointData;
  KinematicChain theKinematicChain;
  

  bool trajectoryHeadMove(const std::vector<Vector3<double> >& points);
  void gotoPointOnTheGround(const Vector2<double>& target);

  void lookStraightAhead();
  void lookStraightAheadWithStabilization();
  void lookAtPoint();
  void simpleLookAtPoint();

  void hold();
  void search();
  void randomSearch();


  void moveByAngle(const Vector2<double>& target);
  void gotoAngle(const Vector2<double>& target);

  void lookAtWorldPoint(const Vector3<double>& target);
  void lookAtWorldPointSimple(const Vector3<double>& target);

  Vector3<double> g(double yaw, double pitch, const Vector3<double>& pointInWorld);
};

#endif  /* _HeadMotionEngine_H */

