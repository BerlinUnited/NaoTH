/**
* @file ArmMotionRequest.h
*
* @author <a href="mailto:guido.schillaci@informatik.hu-berlin.de">Guido Schillaci</a>
* @author <a href="mailto:scheunem@informatik.hu-berlin.de">Marcus Scheunemann</a>
* Declaration of class ArmMotionRequest
*/

#ifndef __ArmMotionRequest_h_
#define __ArmMotionRequest_h_

#include "Tools/DataStructures/Printable.h"
#include "Tools/DataStructures/Serializer.h"
//#include "Tools/DataStructures/Streamable.h"
//#include "PlatformInterface/PlatformInterchangeable.h"

#include "Tools/Math/Vector2.h"
#include "Tools/Math/Vector3.h"
#include "Representations/Infrastructure/CameraInfo.h"

#include "Messages/Representations.pb.h"

/**
 * @class ArmMotionRequest
 * This describes the ArmMotionRequest
 */
class ArmMotionRequest : public naoth::Printable
{
public:
  
//  enum Coordinate
//  {
//    Hip,
//    LFoot,
//    RFoot,
//    numOfCoordinate
//  };


  /** ids for all motion types */
  enum ArmMotionID
  {
      hold,
      set_left_shoulder_position,
      set_left_shoulder_stiffness,
      set_left_elbow_position,
      set_left_elbow_stiffness,
      set_right_shoulder_position,
      set_right_shoulder_stiffness,
      set_right_elbow_position,
      set_right_elbow_stiffness,
      set_left_arm_joint_position,
      set_left_arm_joint_stiffness,
      set_right_arm_joint_position,
      set_right_arm_joint_stiffness,
      set_both_arms_joint_position,
      set_both_arms_joint_stiffness,

      arms_none,
      arms_back,
      arms_straight,
      
      numOfArmMotion
  };

  /** constructor */
  ArmMotionRequest() { }
  ~ArmMotionRequest() { }

  void reset() { }


  /** id of the motion to be executed */
  ArmMotionID id;

//  // id of the camera to use
//  naoth::CameraInfo::CameraID cameraID;

  // LEFT ARM
  // describes the target angle-position: x=Pitch, y=roll
  Vector2<double> lShoulderPosition;
  // describes the target stiffness: x=Pitch, y=roll
  Vector2<double> lShoulderStiffness;
  // describes the target angle-position: x=yaw, y=roll
  Vector2<double> lElbowPosition;
  // describes the target stiffness: x=yaw, y=roll
  Vector2<double> lElbowStiffness;

  // RIGHT ARM
  // describes the target angle-position: x=Pitch, y=roll
  Vector2<double> rShoulderPosition;
  // describes the target stiffness: x=Pitch, y=roll
  Vector2<double> rShoulderStiffness;
  // describes the target angle-position: x=yaw, y=roll
  Vector2<double> rElbowPosition;
  // describes the target stiffness: x=yaw, y=roll
  Vector2<double> rElbowStiffness;

  double stiffness;

  double max_velocity_deg_in_sec;

  /** return the name of the motion id */
  static std::string getName(ArmMotionID id);

  /** return the head motion id reprented by the name */
  static ArmMotionID getId(const std::string& name);

  virtual void print(std::ostream& stream) const;

};

namespace naoth
{
  template<>
  class Serializer<ArmMotionRequest>
  {
  public:
    static void serialize(const ArmMotionRequest& representation, std::ostream& stream);
    static void serialize(const ArmMotionRequest& representation, naothmessages::ArmMotionRequest* msg);
    static void deserialize(std::istream& stream, ArmMotionRequest& representation);
    static void deserialize(const naothmessages::ArmMotionRequest* msg, ArmMotionRequest& representation);
  };
}

#endif // __ArmMotionRequest_h_
