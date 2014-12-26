/**
 * @file HeadMotionRequest.h
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 * Definition of HeadMotionReqeuest
 */

#ifndef _HeadMotionRequest_h_
#define _HeadMotionRequest_h_

#include "Tools/DataStructures/Printable.h"
#include "Tools/DataStructures/Serializer.h"

#include "Tools/Math/Vector2.h"
#include "Tools/Math/Vector3.h"
#include "Representations/Infrastructure/CameraInfo.h"

/**
 * @class HeadMotionRequest
 * This describes the HeadMotionRequest
 */
class HeadMotionRequest : public naoth::Printable
{
public:
  
  enum Coordinate
  {
    Hip,
    LFoot,
    RFoot,
    numOfCoordinate
  };


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
    cameraID(naoth::CameraInfo::Bottom),
    coordinate(Hip),
    searchCenter(0, 0, 0),
    searchSize(90, 45, 0),
    searchDirection(true)
  {
  }

  ~HeadMotionRequest()
  {
  }

  void reset()
  {
    id = hold;
    cameraID = naoth::CameraInfo::Bottom;
    targetJointPosition = Vector2d();
    targetPointInImage = Vector2d();
    targetPointInTheWorld = Vector3d();
    targetPointOnTheGround = Vector2d();
    coordinate = Hip;
    searchCenter = Vector3d();
    searchSize = Vector3d(90, 45, 0),
    searchDirection = true;
  }


  /** id of the motion to be executed */
  HeadMotionID id;

  // id of the camera to use
  naoth::CameraInfo::CameraID cameraID;

  // describes the target angle-position: x=yaw, y=pitch 
  Vector2<double> targetJointPosition;

  /** target point in image which the camera should point to with the center*/
  Vector2<double> targetPointInImage;

  /** see coordinate */
  Vector3<double> targetPointInTheWorld;

  /** see coordinate */
  Vector2<double> targetPointOnTheGround;


  /** the coordinate system for the targetPointInTheWorld and targetPointOnTheGround */
  Coordinate coordinate;


  // parameters for head search motion
  Vector3<double> searchCenter;
  Vector3<double> searchSize;
  bool searchDirection;


  /** return the name of the motion id */
  static std::string getName(HeadMotionID id);

  /** return the head motion id reprented by the name */
  static HeadMotionID getId(const std::string& name);


  virtual void print(std::ostream& stream) const;

};

namespace naoth
{
  template<>
  class Serializer<HeadMotionRequest>
  {
  public:
    static void serialize(const HeadMotionRequest& representation, std::ostream& stream);
    static void deserialize(std::istream& stream, HeadMotionRequest& representation);
  };
}

#endif // __HeadMotionRequest_h_
