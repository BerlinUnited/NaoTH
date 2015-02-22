/**
* @file GraspingBehaviorControl.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Definition of class GraspingBehaviorControl
*/

#ifndef _GraspingBehaviorControl_H_
#define _GraspingBehaviorControl_H_

#include "Cognition/Cognition.h"

// representations
#include "Representations/Modeling/BallModel.h"
#include "Representations/Perception/BallPercept.h"
#include "Representations/Perception/GoalPercept.h"
//#include "Representations/Modeling/GraspingBallModel.h"

#include "Representations/Motion/Request/HeadMotionRequest.h"
#include "Representations/Motion/Request/MotionRequest.h"
#include "Representations/Motion/MotionStatus.h"

#include "Representations/Infrastructure/JointData.h"
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Infrastructure/SoundData.h"
#include "Representations/Infrastructure/FieldInfo.h"
//#include "Representations/Infrastructure/SerialSensorData.h"

#include "Representations/Modeling/KinematicChain.h"

// Debug
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugModify.h"
#include "Tools/Debug/DebugDrawings.h"
#include "Tools/Debug/DebugPlot.h"

BEGIN_DECLARE_MODULE(GraspingBehaviorControl)
  PROVIDE(DebugRequest)
  PROVIDE(DebugPlot)
  PROVIDE(DebugModify)
  PROVIDE(DebugDrawings)

  REQUIRE(FieldInfo)
  REQUIRE(BallModel)
  //REQUIRE(GraspingBallModel)
  REQUIRE(BallPercept)
  REQUIRE(GoalPercept)
  REQUIRE(SensorJointData)
  REQUIRE(FrameInfo)
  REQUIRE(KinematicChain)
  REQUIRE(MotionStatus)
//  REQUIRE(SerialSensorData)

  PROVIDE(HeadMotionRequest)
  PROVIDE(MotionRequest)
  PROVIDE(SoundPlayData)
END_DECLARE_MODULE(GraspingBehaviorControl)

class GraspingBehaviorControl: public GraspingBehaviorControlBase
{
public:
  GraspingBehaviorControl();
  virtual ~GraspingBehaviorControl(){};

  virtual void execute();

private:
  double sitHeight;
  const Vector3d defaultGraspingCenter;

  void take_object_from_table();
  
};//end class GraspingBehaviorControl

#endif // __GraspingBehaviorControl_H_
