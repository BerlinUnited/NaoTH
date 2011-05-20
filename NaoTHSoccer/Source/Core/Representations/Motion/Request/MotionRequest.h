/**
 * @file MotionRequest.h
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 * Definition of the class MotionRequest
 */

#ifndef __MotionRequest_h_
#define __MotionRequest_h_

#include "Tools/DataStructures/Printable.h"

#include "Tools/Math/Vector2.h"
#include "Tools/Math/Pose3D.h"
#include "Representations/Infrastructure/JointData.h"
#include <string>


#include "MotionID.h"
#include "KickRequest.h"
#include "WalkRequest.h"

/**
 * This describes the MotionRequest
 */
class MotionRequest : public naoth::Printable {
public:

  /** constructor */
  MotionRequest() :
  time(0),
  id(motion::INIT),
  forced(false),
  standHeight(-1)
  {
    reset();
  }

  ~MotionRequest() {
  }

  unsigned int time;

  /** id of the motion to be executed */
  motion::MotionID id;

  // force the motion be executed immediately
  bool forced;

  KickRequest kickRequest;
  WalkRequest walkRequest;

  // hich of the hip when "stand" is requested
  // if the value is < 0, then the default value is used 
  double standHeight;

  void reset() {
    forced = false;
    id = motion::EMPTY;
    standHeight = -1;
  }//end reset

  virtual void print(ostream& stream) const {
    stream << "time = " << time << '\n';
    stream << "MotionID = " << motion::getName(id) << endl;
    switch (id) {
      case motion::WALK:
        walkRequest.print(stream);
        break;
      case motion::KICK:
        kickRequest.print(stream);
        break;
      default:
        break;
    }//end switch
  }//end print
};

#endif // __MotionRequest_h_


