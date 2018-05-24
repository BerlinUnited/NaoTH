/**
* @file PathPlanner2018.h
*
* @author <a href="mailto:akcayyig@hu-berlin.de">Yigit Can Akcay</a>
* Definition of class PathPlanner2018
*/

#ifndef _PathPlanner2018_H_
#define _PathPlanner2018_H_

#include <ModuleFramework/Module.h>

#include "Tools/Math/Geometry.h"

// debug
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugModify.h"
#include "Tools/Debug/DebugParameterList.h"

// representations
#include "Representations/Infrastructure/FieldInfo.h"
#include "Representations/Motion/Request/HeadMotionRequest.h"
#include "Representations/Motion/Request/MotionRequest.h"
#include "Representations/Motion/MotionStatus.h"
#include "Representations/Perception/BallPercept.h"
#include "Representations/Perception/MultiBallPercept.h"
#include "Representations/Modeling/BallModel.h"
#include "Representations/Modeling/PathModel.h"
#include "Representations/Debug/Stopwatch.h"


BEGIN_DECLARE_MODULE(PathPlanner2018)
PROVIDE(DebugRequest)
PROVIDE(DebugModify)
PROVIDE(DebugParameterList)

REQUIRE(FieldInfo)
REQUIRE(BallPercept)
REQUIRE(MultiBallPercept)
REQUIRE(MotionStatus)
REQUIRE(BallModel)

PROVIDE(PathModel)
PROVIDE(MotionRequest)
PROVIDE(HeadMotionRequest)
PROVIDE(StopwatchManager)
END_DECLARE_MODULE(PathPlanner2018)



class PathPlanner2018 : public PathPlanner2018Base
{
public:
  PathPlanner2018();
  ~PathPlanner2018();

  virtual void execute();

private:
  class Parameters : public ParameterList
  {
  public:
    Parameters() : ParameterList("PathPlanner2018")
    {
      PARAMETER_REGISTER(stepLength) = 80;

      syncWithConfig();
    }
    virtual ~Parameters(){}

    double stepLength;
  } params;

  // NONE means hip
  enum Foot
  {
    RIGHT,
    LEFT,
    NONE
  };
  typedef WalkRequest::StepControlRequest::StepType StepType;

  void forwardKick(const Foot& foot);

  // Stepcontrol
  struct StepBufferElement 
  {
    void setPose(const Pose2D& p)
    {
      pose = p;
    }
    void setSpeedDirection(const double s)
    {
      speedDirection = s;
    }
    void setStepType(const StepType& t)
    {
      type = t;
    }
    void setTime(const int t)
    {
      time = t;
    }
    void setCharacter(const double c)
    {
      character = c;
    }
    void setScale(const double s)
    {
      scale = s;
    }
    void setFoot(const Foot& f)
    {
      foot = f;
    }
    void setCoordinate(const WalkRequest::Coordinate& c)
    {
      coordinate = c;
    }
    void setRestriction(const WalkRequest::StepControlRequest::RestrictionMode& r)
    {
      restriction = r;
    }
    void setProtected(const bool p)
    {
      isProtected = p;
    }
    Pose2D pose;
    double speedDirection;
    StepType type;
    int time;
    double character;
    double scale;
    Foot foot;
    WalkRequest::Coordinate coordinate;
    WalkRequest::StepControlRequest::RestrictionMode restriction;
    bool isProtected;
  };
  std::vector<StepBufferElement> stepBuffer;

  // Used to alternate between left and right foot
  // or to specify which foot to use
  Foot footToUse;

  // Used to synchronize stepIDs of WalkEngine to take control
  unsigned int lastStepRequestID;

  void addStep(const StepBufferElement& newStep);
  void updateSpecificStep(const unsigned int index, StepBufferElement& step);
  void manageStepBuffer();
  void executeStepBuffer();


private:
  unsigned int numStepsGeometric;
  unsigned int numStepsCorrection;
  unsigned int numStepsPlanned;
};

#endif // _PathPlanner2018_H_
