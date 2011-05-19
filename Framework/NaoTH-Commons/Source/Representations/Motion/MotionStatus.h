/**
* @file MotionStatus.h
*
* @author <a href="mailto:krause@informatik.hu-berlin.de">Thomas Krause</a>
* Definition of the class MotionStatus
*/

#ifndef __MotionStatus_h_
#define __MotionStatus_h_

#include "Tools/Math/Vector2.h"
#include "Tools/Math/Pose2D.h"
#include "Tools/DataStructures/Printable.h"
#include "MotionRequest/MotionRequestID.h"
#include "HeadMotionRequest.h"

#include <string>

/**
* This describes the MotionStatus
*/
class MotionStatus : public Printable
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
  lastMotion(MotionRequestID::numOfMotion),
  currentMotion(MotionRequestID::numOfMotion)
  {
  }

  ~MotionStatus(){}

  unsigned int time;
  MotionRequestID::MotionID lastMotion;
  MotionRequestID::MotionID currentMotion;
  HeadMotionRequest::HeadMotionID headMotionRequest;
  PlannedMotion plannedMotion;

  virtual void print(ostream& stream) const
  {
    stream << "time = " << time << '\n';
    stream << "lastMotion = " << MotionRequestID::getName(lastMotion) << '\n';
    stream << "currentMotion = " << MotionRequestID::getName(currentMotion) << '\n';
  }//end print
};

#endif // __MotionStatus_h_
