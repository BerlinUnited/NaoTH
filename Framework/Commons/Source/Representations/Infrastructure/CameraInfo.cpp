/* 
 * File:   CameraInfo.cpp
 * Author: Claas-Norman Ritter
 *
 * Created on 1. Februar 2009, 20:07
 */


#include "CameraInfo.h"

#include "Tools/Math/Common.h"

#include <Messages/Framework-Representations.pb.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>

using namespace naoth;
using namespace std;


void CameraInfo::print(ostream& stream) const
{
  stream << "Camera ID: " << cameraID << endl
         << "Resolution Width, Height: " << resolutionWidth << ", " << resolutionHeight << endl

         << "Opening Angle Diagonal (config): " << getOpeningAngleDiagonal() << "rad [" 
                                                << Math::toDegrees(getOpeningAngleDiagonal()) << " deg]" << endl

         << "Opening Angle Width (calculated): " << getOpeningAngleWidth() << " rad [" 
                                                 << Math::toDegrees(getOpeningAngleWidth()) << "deg]" << endl

         << "Opening Angle Height (calculated): " << getOpeningAngleHeight() << " rad [" 
                                                  << Math::toDegrees(getOpeningAngleHeight()) << "deg]" << endl

         << "Optical Center (calculated): " << getOpticalCenterX() << " Pixel, " << getOpticalCenterY() << " Pixel" << endl
         << "Focal Length (calculated): "<< getFocalLength() << " Pixel"<< endl
         /*
         << "Pixel Size: "<< params.pixelSize << " mm" << endl
         << "Focal Length: "<< params.focus << " mm" << endl
         << "Error to Center: " << params.xp << " mm, " << params.yp << " mm" << endl
         << "Radial Symmetric Error: " << params.k1 << " mm, " << params.k2 << " mm, " << params.k3 << " mm" << endl
         << "Radial Asymmetric and Tangential Error: " << params.p1 << " mm, " << params.p2 << " mm" << endl
         << "Affinity and Shearing Error: " << params.b1 << " mm, " << params.b2 << " mm" << endl
         */
         ;
}



void Serializer<CameraInfo>::serialize(const CameraInfo& representation, std::ostream& stream)
{
  naothmessages::CameraInfo msg;
  msg.set_resolutionwidth(representation.resolutionWidth);
  msg.set_resolutionheight(representation.resolutionHeight);
  msg.set_cameraid((naothmessages::CameraID) representation.cameraID);
  msg.set_openinganglediagonal(representation.params.openingAngleDiagonal);

  //msg.set_focus(representation.params.focus);
  //msg.set_pixelsize(representation.params.pixelSize);

  google::protobuf::io::OstreamOutputStream buf(&stream);
  msg.SerializeToZeroCopyStream(&buf);
}

void Serializer<CameraInfo>::deserialize(std::istream& stream, CameraInfo& r)
{
  naothmessages::CameraInfo msg;
  google::protobuf::io::IstreamInputStream buf(&stream);
  msg.ParseFromZeroCopyStream(&buf);
  
  r.resolutionWidth = static_cast<unsigned int>(msg.resolutionwidth());
  r.resolutionHeight = static_cast<unsigned int>(msg.resolutionheight());
  r.cameraID = (CameraInfo::CameraID) msg.cameraid();
  r.params.openingAngleDiagonal = msg.openinganglediagonal();

  //r.params.focus = msg.focus();
  //r.params.pixelSize = msg.pixelsize();
}
