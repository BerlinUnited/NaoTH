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

  /** constructor */
  MotionStatus()
  :
  time(0),
  lastMotion(motion::num_of_motions),
  currentMotion(motion::num_of_motions)
  {
  }

  ~MotionStatus(){}

  unsigned int time;
  motion::MotionID lastMotion;
  motion::MotionID currentMotion;
  motion::State currentMotionState;
  HeadMotionRequest::HeadMotionID headMotion;
  PlannedMotion plannedMotion;

  virtual void print(ostream& stream) const
  {
    stream << "time = " << time << '\n';
    stream << "lastMotion = " << motion::getName(lastMotion) << '\n';
    stream << "currentMotion = " << motion::getName(currentMotion) << '\n';
    stream << "currentMotionState = " << motion::getName(currentMotionState) << '\n';
    stream << "plannedMotion: \n";
    stream << "left foot = "<< plannedMotion.lFoot<<"\n";
    stream << "right foot = "<< plannedMotion.rFoot<<"\n";
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
