/**
* @file CameraMatrixOffset.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
*/

#ifndef _CameraMatrixOffset_h_
#define _CameraMatrixOffset_h_

#include <Representations/Infrastructure/CameraInfo.h>
#include "Tools/Math/Vector2.h"
#include "Tools/DataStructures/Printable.h"
#include "Tools/DataStructures/ParameterList.h"
#include <Tools/DataStructures/Serializer.h>

//serialization
#include "Tools/DataConversion.h"
#include "Messages/Representations.pb.h"
#include <google/protobuf/io/zero_copy_stream_impl.h>


class CameraMatrixOffset: public ParameterList, public naoth::Printable
{
public:

  CameraMatrixOffset() : ParameterList("CameraMatrixOffset")
  {
    PARAMETER_REGISTER(correctionOffset[naoth::CameraInfo::Top].x) = 0;
    PARAMETER_REGISTER(correctionOffset[naoth::CameraInfo::Top].y) = 0;
    PARAMETER_REGISTER(correctionOffset[naoth::CameraInfo::Bottom].x) = 0;
    PARAMETER_REGISTER(correctionOffset[naoth::CameraInfo::Bottom].y) = 0;

    PARAMETER_REGISTER(body_rot.x) = 0;
    PARAMETER_REGISTER(body_rot.y) = 0;

    PARAMETER_REGISTER(head_rot.x) = 0;
    PARAMETER_REGISTER(head_rot.y) = 0;
    PARAMETER_REGISTER(head_rot.z) = 0;

    PARAMETER_REGISTER(cam_rot[naoth::CameraInfo::Top].x) = 0;
    PARAMETER_REGISTER(cam_rot[naoth::CameraInfo::Top].y) = 0;
    PARAMETER_REGISTER(cam_rot[naoth::CameraInfo::Top].z) = 0;

    PARAMETER_REGISTER(cam_rot[naoth::CameraInfo::Bottom].x) = 0;
    PARAMETER_REGISTER(cam_rot[naoth::CameraInfo::Bottom].y) = 0;
    PARAMETER_REGISTER(cam_rot[naoth::CameraInfo::Bottom].z) = 0;

    syncWithConfig();
  }
  
  virtual ~CameraMatrixOffset(){}

  //experimental
  Vector2d offsetByGoalModel;
  Vector2d offset;

  Vector2d correctionOffset[naoth::CameraInfo::numOfCamera];

  Vector2d body_rot;
  Vector3d head_rot;
  Vector3d cam_rot[naoth::CameraInfo::numOfCamera];

  virtual void print(std::ostream& stream) const
  {
//    stream << "x = " << offset.x << std::endl;
//    stream << "y = " << offset.y << std::endl;
//    stream << "Roll Offset Camera Top (x):"<< correctionOffset[naoth::CameraInfo::Top].x << " rad" << std::endl;
//    stream << "Tilt Offset Camera Top (y):"<< correctionOffset[naoth::CameraInfo::Top].y << " rad" <<  std::endl;
//    stream << "Roll Offset Camera Bottom (x): "<< correctionOffset[naoth::CameraInfo::Bottom].x << " rad" << std::endl;
//    stream << "Tilt Offset Camera Bottom (y):"<< correctionOffset[naoth::CameraInfo::Bottom].y << " rad" <<  std::endl;

    stream << "----Offsets-------------" << std::endl;
    stream << "----Body----------------" << std::endl;
    stream << "Roll  (x): "<< body_rot.x << " rad" << std::endl;
    stream << "Pitch (y): "<< body_rot.y << " rad" << std::endl;
    stream << "----Head----------------" << std::endl;
    stream << "Roll  (x): "<< head_rot.x << " rad" << std::endl;
    stream << "Pitch (y): "<< head_rot.y << " rad" << std::endl;
    stream << "Yaw   (z): "<< head_rot.z << " rad" << std::endl;
    stream << "----TopCam--------------" << std::endl;
    stream << "Roll  (x): "<< cam_rot[naoth::CameraInfo::Top].x  << " rad" << std::endl;
    stream << "Pitch (y): "<< cam_rot[naoth::CameraInfo::Top].y  << " rad" << std::endl;
    stream << "Yaw   (z): "<< cam_rot[naoth::CameraInfo::Top].z  << " rad" << std::endl;

    stream << "----BottomCam-----------" << std::endl;
    stream << "Roll  (x): "<< cam_rot[naoth::CameraInfo::Bottom].x  << " rad" << std::endl;
    stream << "Pitch (y): "<< cam_rot[naoth::CameraInfo::Bottom].y  << " rad" << std::endl;
    stream << "Yaw   (z): "<< cam_rot[naoth::CameraInfo::Bottom].z  << " rad" << std::endl;
  }
};

class CameraMatrixOffsetV3: public ParameterList, public naoth::Printable
{
public:

  CameraMatrixOffsetV3() : ParameterList("CameraMatrixOffsetV3")
  {
    PARAMETER_REGISTER(body_rot.x) = 0;
    PARAMETER_REGISTER(body_rot.y) = 0;

    PARAMETER_REGISTER(head_rot.x) = 0;
    PARAMETER_REGISTER(head_rot.y) = 0;
    PARAMETER_REGISTER(head_rot.z) = 0;

    PARAMETER_REGISTER(cam_rot[naoth::CameraInfo::Top].x) = 0;
    PARAMETER_REGISTER(cam_rot[naoth::CameraInfo::Top].y) = 0;
    PARAMETER_REGISTER(cam_rot[naoth::CameraInfo::Top].z) = 0;

    PARAMETER_REGISTER(cam_rot[naoth::CameraInfo::Bottom].x) = 0;
    PARAMETER_REGISTER(cam_rot[naoth::CameraInfo::Bottom].y) = 0;
    PARAMETER_REGISTER(cam_rot[naoth::CameraInfo::Bottom].z) = 0;

    syncWithConfig();
  }

  virtual ~CameraMatrixOffsetV3(){}

  Vector2d body_rot;
  Vector3d head_rot;
  Vector3d cam_rot[naoth::CameraInfo::numOfCamera];

  Pose2D global_pose;

  virtual void print(std::ostream& stream) const
  {
    stream << "----Offsets-------------" << std::endl;
    stream << "----Body----------------" << std::endl;
    stream << "Roll  (x): "<< Math::toDegrees(body_rot.x) << " °" << std::endl;
    stream << "Pitch (y): "<< Math::toDegrees(body_rot.y) << " °" << std::endl;
    stream << "----Head----------------" << std::endl;
    stream << "Roll  (x): "<< Math::toDegrees(head_rot.x) << " °" << std::endl;
    stream << "Pitch (y): "<< Math::toDegrees(head_rot.y) << " °" << std::endl;
    stream << "Yaw   (z): "<< Math::toDegrees(head_rot.z) << " °" << std::endl;
    stream << "----TopCam--------------" << std::endl;
    stream << "Roll  (x): "<< Math::toDegrees(cam_rot[naoth::CameraInfo::Top].x)  << " °" << std::endl;
    stream << "Pitch (y): "<< Math::toDegrees(cam_rot[naoth::CameraInfo::Top].y)  << " °" << std::endl;
    stream << "Yaw   (z): "<< Math::toDegrees(cam_rot[naoth::CameraInfo::Top].z)  << " °" << std::endl;
    stream << "----BottomCam-----------" << std::endl;
    stream << "Roll  (x): "<< Math::toDegrees(cam_rot[naoth::CameraInfo::Bottom].x)  << " °" << std::endl;
    stream << "Pitch (y): "<< Math::toDegrees(cam_rot[naoth::CameraInfo::Bottom].y)  << " °" << std::endl;
    stream << "Yaw   (z): "<< Math::toDegrees(cam_rot[naoth::CameraInfo::Bottom].z)  << " °" << std::endl;
    stream << "----Global Pose---------" << std::endl;
    stream << "x: "<< global_pose.translation.x << " mm" << std::endl;
    stream << "y: "<< global_pose.translation.y << " mm" << std::endl;
    stream << "rot: "<< Math::toDegrees(global_pose.rotation)  << " °" << std::endl;
  }
};

namespace naoth
{
template<>
class Serializer<CameraMatrixOffset>
{
  public:
  static void serialize(const CameraMatrixOffset& representation, std::ostream& stream)
  {
    naothmessages::CameraMatrixCalibration msg;
    for(int id=0; id < naoth::CameraInfo::numOfCamera; id++) {
      naoth::DataConversion::toMessage(representation.correctionOffset[id], *msg.add_correctionoffset());
      naoth::DataConversion::toMessage(representation.cam_rot[id], *msg.add_correctionoffsetcam());
    }

    msg.mutable_correctionoffsetbody()->set_x(representation.body_rot.x);
    msg.mutable_correctionoffsetbody()->set_y(representation.body_rot.y);

    msg.mutable_correctionoffsethead()->set_x(representation.head_rot.x);
    msg.mutable_correctionoffsethead()->set_y(representation.head_rot.y);
    msg.mutable_correctionoffsethead()->set_z(representation.head_rot.z);

    google::protobuf::io::OstreamOutputStream buf(&stream);
    msg.SerializeToZeroCopyStream(&buf);
  }

  static void deserialize(std::istream& stream, CameraMatrixOffset& representation)
  {
    naothmessages::CameraMatrixCalibration msg;
    google::protobuf::io::IstreamInputStream buf(&stream);
    msg.ParseFromZeroCopyStream(&buf);
    
    ASSERT(msg.correctionoffset().size() == naoth::CameraInfo::numOfCamera);
    for(int id=0; id < naoth::CameraInfo::numOfCamera; id++) {
        naoth::DataConversion::fromMessage(msg.correctionoffset(id), representation.correctionOffset[id]);
        naoth::DataConversion::fromMessage(msg.correctionoffsetcam(id), representation.cam_rot[id]);
    }

    representation.body_rot.x = msg.mutable_correctionoffsetbody()->x();
    representation.body_rot.y = msg.mutable_correctionoffsetbody()->y();

    representation.head_rot.x = msg.mutable_correctionoffsethead()->x();
    representation.head_rot.y = msg.mutable_correctionoffsethead()->y();
    representation.head_rot.z = msg.mutable_correctionoffsethead()->z();
  }
};
}

#endif // _CameraMatrixOffset_h_
