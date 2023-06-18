/**
* @file MotionStatus.h
*
* @author <a href="mailto:krause@informatik.hu-berlin.de">Thomas Krause</a>
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* Definition of the class MotionStatus
*/

#ifndef __MotionStatus_h_
#define __MotionStatus_h_

#include "Tools/Math/Vector2.h"
#include "Tools/Math/Pose2D.h"
#include "Tools/DataStructures/Printable.h"
#include "Request/MotionID.h"
#include "Request/HeadMotionRequest.h"

#include <string>

/**
* This describes the MotionStatus
*/
class MotionStatus : public naoth::Printable
{
public:

  struct PlannedMotion
  {
    Pose2D hip;
    Pose2D lFoot;
    Pose2D rFoot;
  };

  struct StepControlStatus
  {
    StepControlStatus() : stepID(0), stepRequestID(0), moveableFoot(NONE) {}

    enum MoveableFoot
    {
      NONE,
      LEFT,
      RIGHT,
      BOTH
    };

    unsigned int stepID;        // the walk only accept request with this id
    unsigned int stepRequestID; // for PathPlanner
    MoveableFoot moveableFoot;  // which foot can be controlled
  };

  /** constructor */
  MotionStatus()
  :
  time(0),
  lastMotion(motion::num_of_motions),
  currentMotion(motion::num_of_motions),
  currentMotionState(motion::stopped),
  headMotion(HeadMotionRequest::numOfHeadMotion),
  target_reached(false),
  head_target_reached(false),
  head_got_stuck(false),
  walk_emergency_stop(false)
  {
  }

  ~MotionStatus(){}

  // TODO: add comments!!!
  unsigned int time;
  motion::MotionID lastMotion;
  motion::MotionID currentMotion;
  motion::State currentMotionState;
  HeadMotionRequest::HeadMotionID headMotion;
  PlannedMotion plannedMotion;
  StepControlStatus stepControl;
  bool target_reached; //TODO: which motion do set that? Maybe better stand_target reached
  
  bool head_target_reached;
  bool head_at_rest;
  // deprecated: head_got_stuck can be calculated as: !head_target_reached && head_at_rest
  bool head_got_stuck; 

  bool walk_emergency_stop;

  virtual void print(std::ostream& stream) const
  {
    stream << "time = " << time << '\n';
    stream << "lastMotion = " << motion::getName(lastMotion) << '\n';
    stream << "currentMotion = " << motion::getName(currentMotion) << '\n';
    stream << "currentMotionState = " << motion::getName(currentMotionState) << '\n';
    stream << "plannedMotion: \n";
    stream << "hip = "<< plannedMotion.hip <<"\n";
    stream << "left foot = "<< plannedMotion.lFoot<<"\n";
    stream << "right foot = "<< plannedMotion.rFoot<<"\n";
    stream << "step control = "<< stepControl.stepID << " " << stepControl.moveableFoot <<"\n";
    stream << "target_reached = " << target_reached << '\n';
    stream << "head_target_reached = " << head_target_reached << '\n';
    stream << "head_at_rest = " << head_at_rest << '\n';
    stream << "head_got_stuck = " << head_got_stuck << '\n';
    stream << "walk_emergency_stop = " << walk_emergency_stop << '\n';
  }//end print
};

namespace naoth
{
  template<>
  class Serializer<MotionStatus>
  {
  public:
    static void serialize(const MotionStatus& representation, std::ostream& stream);
    static void deserialize(std::istream& stream, MotionStatus& representation);
  };
}

#endif // __MotionStatus_h_
