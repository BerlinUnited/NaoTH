/**
* @file GraspRequest.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Definition of the class GraspRequest
*/

#ifndef _GraspRequest_h_
#define _GraspRequest_h_

#include <Tools/Math/Vector3.h>

#include "Tools/DataStructures/Printable.h"
#include <Messages/Representations.pb.h>
#include <Tools/DataStructures/Serializer.h>

/**
* This describes the GraspRequest
*/
class GraspRequest : public naoth::Printable 
{
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

  void print(ostream& stream) const 
  {
    stream << "graspingPoint: " << graspingPoint << endl;
    stream << "graspingState: " << graspingState << endl;
    stream << "graspDistState: " << graspDistState << endl;
    stream << "graspStiffState: " << graspStiffState << endl;
  }//end print

}; //end class GraspRequest


namespace naoth
{
  template<>
  class Serializer<GraspRequest>
  {
  public:
    static void serialize(const GraspRequest& representation, std::ostream& stream);
    static void serialize(const GraspRequest& representation, naothmessages::GraspRequest* msg);
    static void deserialize(std::istream& stream, GraspRequest& representation);
    static void deserialize(const naothmessages::GraspRequest* msg, GraspRequest& representation);
  };
}

#endif // __MotionRequest_h_
