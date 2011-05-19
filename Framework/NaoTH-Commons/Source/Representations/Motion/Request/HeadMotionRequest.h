/**
 * @file HeadMotionRequest.h
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 * Definition of HeadMotionReqeuest
 */

#ifndef __HeadMotionRequest_h_
#define __HeadMotionRequest_h_

#include "Tools/DataStructures/Printable.h"
#include "Tools/DataStructures/Serializer.h"
#include "Tools/DataStructures/Streamable.h"
#include "PlatformInterface/PlatformInterchangeable.h"

#include "Tools/Math/Vector2.h"
#include "Tools/Math/Vector3.h"
#include "Representations/Infrastructure/CameraInfo.h"

namespace naoth {
namespace motion {
/**
 * @class HeadMotionRequest
 * This describes the HeadMotionRequest
 */
class HeadMotionRequest : public PlatformInterchangeable, public Printable
{
public:
  
  /** ids for all motion types */
  enum HeadMotionID
  {
    hold,
    search,
    stabilize,
    look_straight_ahead,
    look_at_world_point,
    look_at_point_on_the_ground,

    goto_angle,
    look_at_point,
    numOfHeadMotion
  };

  /** constructor */
  HeadMotionRequest()
  :
  id(hold),
  cameraID(CameraInfo::Bottom),
  searchCenter(0, 0, 0),
  searchSize(90, 45, 0),
  searchDirection(true)
  {
  }

  ~HeadMotionRequest()
  {
  }

  /** id of the motion to be executed */
  HeadMotionID id;

  CameraInfo::CameraID cameraID;

  // describes the target angle-position: x=yaw, y=pitch 
  Vector2<double> targetJointPosition;


  Vector2<double> targetPointInImage;
  Vector3<double> targetPointInTheWorld;
//  Vector2<double> targetPointOnTheGround;

  // parameters for head search motion
  Vector3<double> searchCenter;
  Vector3<double> searchSize;
  bool searchDirection;

  string getName() const;

  virtual void print(ostream& stream) const
  {
    stream << "Current MotionRequest = " << getName() << endl;
  }//end print

/*  template<>
  class Serializer<HeadMotionRequest>
  {
  public:
    static void serialize(const HeadMotionRequest& representation, std::ostream& stream);
    static void deserialize(std::istream& stream, HeadMotionRequest& representation);
  };*/
};
} // namespace motion
}// namespace naoth
#endif // __HeadMotionRequest_h_
