/**
 * @file PathPlannerWalk.h
 *
 * @author <a href="mailto:akcayyig@hu-berlin.de">Yigit Can Akcay</a>
 * Definition of class PathPlannerWalk
 */

#ifndef _PathPlannerWalk_H_
#define _PathPlannerWalk_H_

#include <ModuleFramework/Module.h>

// representations
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Infrastructure/FieldInfo.h"
#include "Representations/Modeling/OdometryData.h"
#include "Representations/Perception/BallPercept.h"
#include "Representations/Perception/MultiBallPercept.h"
#include "Representations/Modeling/BallModel.h"

#include "Representations/Motion/Request/HeadMotionRequest.h"
#include "Representations/Motion/Request/MotionRequest.h"
#include "Representations/Motion/MotionStatus.h"

#include "Representations/Modeling/PathModel.h"

// debug
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugModify.h"

BEGIN_DECLARE_MODULE(PathPlannerWalk)
PROVIDE(DebugRequest)
PROVIDE(DebugModify)

REQUIRE(FrameInfo)
REQUIRE(FieldInfo)
REQUIRE(MotionStatus)
REQUIRE(BallPercept)
REQUIRE(BallPerceptTop)
REQUIRE(MultiBallPercept)
PROVIDE(BallModel)
REQUIRE(OdometryData)

PROVIDE(PathModel)

PROVIDE(HeadMotionRequest)
PROVIDE(MotionRequest)
END_DECLARE_MODULE(PathPlannerWalk)

class PathPlannerWalk: public PathPlannerWalkBase
{
public:
  PathPlannerWalk();
  ~PathPlannerWalk(){};

  virtual void execute();

  enum Foot
  {
    Right,
    Left
  };

  void walk_forward();
  unsigned int lastStepID;
  Foot executionFoot;
  bool firstStep;

private:
};

#endif // _PathPlannerWalk_H_
