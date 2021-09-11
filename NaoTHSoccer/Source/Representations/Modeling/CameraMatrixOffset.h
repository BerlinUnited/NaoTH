/**
* @file CameraMatrixOffset.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* @author <a href="mailto:kaden@informatik.hu-berlin.de">Steffen Kaden</a>
*/

#ifndef CameraMatrixOffset_H
#define CameraMatrixOffset_H

#include <Representations/Infrastructure/CameraInfo.h>

#include <Tools/Math/Vector2.h>
#include <Tools/DataStructures/Printable.h>
#include <Tools/DataStructures/ParameterList.h>
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
    PARAMETER_REGISTER(body_rot.x) = 0;
    PARAMETER_REGISTER(body_rot.y) = 0;

    PARAMETER_REGISTER(head_rot.x) = 0;
    PARAMETER_REGISTER(head_rot.y) = 0;
    PARAMETER_REGISTER(head_rot.z) = 0;

    /*
    PARAMETER_REGISTER(cam_rot[naoth::CameraInfo::Top].x) = 0;
    PARAMETER_REGISTER(cam_rot[naoth::CameraInfo::Top].y) = 0;
    PARAMETER_REGISTER(cam_rot[naoth::CameraInfo::Top].z) = 0;

    PARAMETER_REGISTER(cam_rot[naoth::CameraInfo::Bottom].x) = 0;
    PARAMETER_REGISTER(cam_rot[naoth::CameraInfo::Bottom].y) = 0;
    PARAMETER_REGISTER(cam_rot[naoth::CameraInfo::Bottom].z) = 0;
    */

    // NOTE: explicite naming of the parameters
    registerParameter("cam_rot.top.x", cam_rot[naoth::CameraInfo::Top].x) = 0;
    registerParameter("cam_rot.top.y", cam_rot[naoth::CameraInfo::Top].y) = 0;
    registerParameter("cam_rot.top.z", cam_rot[naoth::CameraInfo::Top].z) = 0;

    registerParameter("cam_rot.bottom.x", cam_rot[naoth::CameraInfo::Bottom].x) = 0;
    registerParameter("cam_rot.bottom.y", cam_rot[naoth::CameraInfo::Bottom].y) = 0;
    registerParameter("cam_rot.bottom.z", cam_rot[naoth::CameraInfo::Bottom].z) = 0;

    /*
    // TODO: Version 2 (see below)
    PARAMETER_REGISTER(cam_rot.top.x) = 0;
    PARAMETER_REGISTER(cam_rot.top.y) = 0;
    PARAMETER_REGISTER(cam_rot.top.z) = 0;

    PARAMETER_REGISTER(cam_rot.bottom.x) = 0;
    PARAMETER_REGISTER(cam_rot.bottom.y) = 0;
    PARAMETER_REGISTER(cam_rot.bottom.z) = 0;
    */

    syncWithConfig();
  }

  virtual ~CameraMatrixOffset(){}

  // Experimental
  Vector2d offsetByGoalModel;

  Vector2d body_rot;
  Vector3d head_rot;

  // TODO: do we need indexing here? It makes access difficult. There are only few places where indexed access is useful.
  Vector3d cam_rot[naoth::CameraInfo::numOfCamera];
  

  virtual void print(std::ostream& stream) const
  {
    stream << "----Offsets-------------" << std::endl;
    stream << "----Body----------------" << std::endl;
    stream << "Roll  (x): "<< Math::toDegrees(body_rot.x) << " deg" << std::endl;
    stream << "Pitch (y): "<< Math::toDegrees(body_rot.y) << " deg" << std::endl;
    stream << "----Head----------------" << std::endl;
    stream << "Roll  (x): "<< Math::toDegrees(head_rot.x) << " deg" << std::endl;
    stream << "Pitch (y): "<< Math::toDegrees(head_rot.y) << " deg" << std::endl;
    stream << "Yaw   (z): "<< Math::toDegrees(head_rot.z) << " deg" << std::endl;
    stream << "----TopCam--------------" << std::endl;
    stream << "Roll  (x): "<< Math::toDegrees(cam_rot[naoth::CameraInfo::Top].x)  << " deg" << std::endl;
    stream << "Pitch (y): "<< Math::toDegrees(cam_rot[naoth::CameraInfo::Top].y)  << " deg" << std::endl;
    stream << "Yaw   (z): "<< Math::toDegrees(cam_rot[naoth::CameraInfo::Top].z)  << " deg" << std::endl;
    stream << "----BottomCam-----------" << std::endl;
    stream << "Roll  (x): "<< Math::toDegrees(cam_rot[naoth::CameraInfo::Bottom].x)  << " deg" << std::endl;
    stream << "Pitch (y): "<< Math::toDegrees(cam_rot[naoth::CameraInfo::Bottom].y)  << " deg" << std::endl;
    stream << "Yaw   (z): "<< Math::toDegrees(cam_rot[naoth::CameraInfo::Bottom].z)  << " deg" << std::endl;
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

    naoth::DataConversion::toMessage(representation.cam_rot[naoth::CameraInfo::Top], *msg.add_correctionoffsetcam());
    naoth::DataConversion::toMessage(representation.cam_rot[naoth::CameraInfo::Bottom], *msg.add_correctionoffsetcam());

    naoth::DataConversion::toMessage(representation.body_rot, *msg.mutable_correctionoffsetbody());
    naoth::DataConversion::toMessage(representation.head_rot, *msg.mutable_correctionoffsethead());

    google::protobuf::io::OstreamOutputStream buf(&stream);
    msg.SerializeToZeroCopyStream(&buf);
  }

  static void deserialize(std::istream& stream, CameraMatrixOffset& representation)
  {
    naothmessages::CameraMatrixCalibration msg;
    google::protobuf::io::IstreamInputStream buf(&stream);
    msg.ParseFromZeroCopyStream(&buf);
    
    //TODO: check 
    naoth::DataConversion::fromMessage(msg.correctionoffsetcam(0), representation.cam_rot[naoth::CameraInfo::Top]);
    naoth::DataConversion::fromMessage(msg.correctionoffsetcam(1), representation.cam_rot[naoth::CameraInfo::Bottom]);

    naoth::DataConversion::fromMessage(msg.correctionoffsetbody(), representation.body_rot);
    naoth::DataConversion::fromMessage(msg.correctionoffsethead(), representation.head_rot);
  }
};
}

#endif // CameraMatrixOffset_H
