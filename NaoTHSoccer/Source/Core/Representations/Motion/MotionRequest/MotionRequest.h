/**
 * @file MotionRequest.h
 *
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
 * Definition of the class MotionRequest
 */

#ifndef __MotionRequest_h_
#define __MotionRequest_h_

#include "Tools/DataStructures/Printable.h"

#include "Tools/Math/Vector2.h"
#include "Tools/Math/Pose3D.h"
#include <DebugCommunication/DebugCommandExecutor.h>
#include "Representations/Infrastructure/JointData.h"
#include <string>


#include "MotionRequestID.h"
#include "KickRequest.h"
#include "WalkRequest.h"

class GraspRequest : public Printable {
public:

  GraspRequest()
    :
    graspingState(none),
    graspDistState(GDS_none),
    graspStiffState(GSS_none) 
  {
  }

  enum GraspingState {
    none,
    open,
    empty,
    no_dist,
    stiff_eq_pressure,
    horizontal_hand,
    test_motion_model,
    test_roundtrip_delay,
    numberOfHandsState
  };

  enum GraspDistState {
    GDS_none, // default
    min_dist, 
    no_sensor_dist,
    thresh_dist_dist,
    thresh_curr_dist,
    thresh_force_dist
  };

  enum GraspStiffState {
    GSS_none, // default
    max_stiff,
    integ_curr_stiff,
    integ_force_stiff,
    p_force_stiff
  };

  Vector3<double> graspingPoint;
  GraspingState graspingState;
  GraspDistState graspDistState;
  GraspStiffState graspStiffState;

  void print(ostream& stream) const {
    stream << "graspingPoint: " << graspingPoint << endl;
    stream << "graspingState: " << graspingState << endl;
    stream << "graspDistState: " << graspDistState << endl;
    stream << "graspStiffState: " << graspStiffState << endl;
  }//end print
}; //end class GraspRequest

/**
 * This describes the MotionRequest
 */
class MotionRequest : public Printable, public Streamable {
public:

  /** constructor */
  MotionRequest() :
  time(0),
  id(MotionRequestID::init),
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
  MotionRequestID::MotionID id;

  // force the motion be executed immediately
  bool forced;

  /** enable receive the serial sensor data */
  bool receive_ssd;

  KickRequest kickRequest;
  WalkRequest walkRequest;
  GraspRequest graspRequest;

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
    id = MotionRequestID::empty;
    standHeight = -1;
  }//end reset

  virtual void print(ostream& stream) const {
    stream << "time = " << time << '\n';
    stream << "MotionID = " << MotionRequestID::getName(id) << endl;
    switch (id) {
      case MotionRequestID::walk:
        walkRequest.print(stream);
        break;
      case MotionRequestID::kick:
        kickRequest.print(stream);
        break;
      case MotionRequestID::grasp:
        graspRequest.print(stream);
        break;
      default:
        break;
    }//end switch
  }//end print

  virtual void toDataStream(ostream& stream) const;

  virtual void fromDataStream(istream& stream);
};

#endif // __MotionRequest_h_


