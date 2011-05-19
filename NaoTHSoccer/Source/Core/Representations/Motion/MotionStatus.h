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
  lastMotion(numOfMotion),
  currentMotion(numOfMotion)
  {
  }

  ~MotionStatus(){}

  unsigned int time;
  MotionID lastMotion;
  MotionID currentMotion;
  HeadMotionRequest::HeadMotionID headMotionRequest;
  PlannedMotion plannedMotion;

  virtual void print(ostream& stream) const
  {
    stream << "time = " << time << '\n';
    stream << "lastMotion = " << getMotionNameById(lastMotion) << '\n';
    stream << "currentMotion = " << getMotionNameById(currentMotion) << '\n';
  }//end print
};

#endif // __MotionStatus_h_
