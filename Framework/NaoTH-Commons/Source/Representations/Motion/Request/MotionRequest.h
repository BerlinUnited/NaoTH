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

namespace naoth
{
  namespace motion
  {
/**
 * This describes the MotionRequest
 */
class MotionRequest : public Printable {
public:

  /** constructor */
  MotionRequest() :
  time(0),
  id(init),
  forced(false),
  receive_ssd(true),
  standHeight(-1),
  enableForcedJointDataHardness(false)
  {
    reset();
  }

  ~MotionRequest() {
  }

  unsigned int time;

  /** id of the motion to be executed */
  MotionID id;

  // force the motion be executed immediately
  bool forced;

  /** enable receive the serial sensor data */
  bool receive_ssd;

  KickRequest kickRequest;
  WalkRequest walkRequest;

  // hich of the hip when "stand" is requested
  // if the value is < 0, then the default value is used 
  double standHeight;

  /** */
  struct TargetPose {
    Pose3D lFoot, rFoot, chest;
    int time;
  } targetPose;

  JointData forcedJointData;
  bool enableForcedJointDataHardness;

  void reset() {
    forced = false;
    id = empty;
    standHeight = -1;
  }//end reset

  virtual void print(ostream& stream) const {
    stream << "time = " << time << '\n';
    stream << "MotionID = " << getMotionNameById(id) << endl;
    switch (id) {
      case walk:
        walkRequest.print(stream);
        break;
      case kick:
        kickRequest.print(stream);
        break;
      default:
        break;
    }//end switch
  }//end print
};

  } // namespace motion
} // namespace naoth

#endif // __MotionRequest_h_


