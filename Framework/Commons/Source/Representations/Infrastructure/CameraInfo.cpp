/* 
 * File:   CameraInfo.cpp
 * Author: Claas-Norman Ritter
 *
 * Created on 1. Februar 2009, 20:07
 */


#include "CameraInfo.h"

#include "Tools/DataConversion.h"
#include <Messages/Framework-Representations.pb.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>

using namespace naoth;
using namespace std;

double CameraInfo::getFocalLength() const
{
  double d2 = resolutionWidth * resolutionWidth + resolutionHeight * resolutionHeight;
  double halfDiagLength = 0.5 * sqrt(d2);
  
  // senity check
  ASSERT(halfDiagLength > 0.0 && getOpeningAngleDiagonal() > 0.0);
  return halfDiagLength / tan(0.5 * getOpeningAngleDiagonal());
}

double CameraInfo::getOpeningAngleHeight() const
{
  return 2.0 * atan2((double)resolutionHeight,getFocalLength() * 2.0);
}

double CameraInfo::getOpeningAngleWidth() const
{
  return 2.0 * atan2((double)resolutionWidth, getFocalLength() * 2.0);
}

double CameraInfo::getOpticalCenterX() const
{
  return double(resolutionWidth / 2);
}

double CameraInfo::getOpticalCenterY() const
{
  return double(resolutionHeight / 2);
}

unsigned long CameraInfo::getSize() const
{
  return resolutionHeight * resolutionWidth;
}

double CameraInfo::getOpeningAngleDiagonal() const
{
  return Math::fromDegrees(params.openingAngleDiagonal);
}

void CameraInfo::print(ostream& stream) const
{
  stream << "Camera selection: " << cameraID << endl
    << "Resolution Width: " << resolutionWidth << ", Height: " << resolutionHeight << endl
         << "Opening Angle (calculated): " << getOpeningAngleWidth() << " rad, " << getOpeningAngleHeight() << " rad" << endl
         << "Optical Center (calculated): " << getOpticalCenterX() << " Pixel, " << getOpticalCenterY() << " Pixel" << endl
         << "Focal Length (calculated): "<< getFocalLength() << " Pixel"<< endl
         << "Pixel Size: "<< params.pixelSize << " mm" << endl
         << "Focal Length: "<< params.focus << " mm" << endl
         << "Error to Center: " << params.xp << " mm, " << params.yp << " mm" << endl
         << "Radial Symmetric Error: " << params.k1 << " mm, " << params.k2 << " mm, " << params.k3 << " mm" << endl
         << "Radial Asymmetric and Tangential Error: " << params.p1 << " mm, " << params.p2 << " mm" << endl
         << "Affinity and Shearing Error: " << params.b1 << " mm, " << params.b2 << " mm" << endl
         ;
}

CameraInfoParameter::CameraInfoParameter(std::string idName)
:
  ParameterList("CameraInfo" + idName)
{
  PARAMETER_REGISTER(openingAngleDiagonal) = 72.6;

  //size of an Pixel on the chip
  PARAMETER_REGISTER(pixelSize) = 0.0036;
  //measured focus
  PARAMETER_REGISTER(focus) = 1.37;
  //moved middle point
  PARAMETER_REGISTER(xp) = 0.0;
  PARAMETER_REGISTER(yp) = 0.0;
  //radial symmetric distortion parameters
  PARAMETER_REGISTER(k1) = 0.0;
  PARAMETER_REGISTER(k2) = 0.0;
  PARAMETER_REGISTER(k3) = 0.0;
  //radial asymmetric and tangential distortion parameters
  PARAMETER_REGISTER(p1) = 0.0;
  PARAMETER_REGISTER(p2) = 0.0;
  //affinity and shearing distortion parameters
  PARAMETER_REGISTER(b1) = 0.0;
  PARAMETER_REGISTER(b2) = 0.0;

  syncWithConfig();
}

void Serializer<CameraInfo>::serialize(const CameraInfo& representation, std::ostream& stream)
{
  naothmessages::CameraInfo msg;
  msg.set_resolutionwidth(representation.resolutionWidth);
  msg.set_resolutionheight(representation.resolutionHeight);
  msg.set_cameraid((naothmessages::CameraID) representation.cameraID);
  msg.set_openinganglediagonal(representation.params.openingAngleDiagonal);
  msg.set_focus(representation.params.focus);
  msg.set_pixelsize(representation.params.pixelSize);

  google::protobuf::io::OstreamOutputStream buf(&stream);
  msg.SerializeToZeroCopyStream(&buf);
}

void Serializer<CameraInfo>::deserialize(std::istream& stream, CameraInfo& r)
{
  naothmessages::CameraInfo msg;
  google::protobuf::io::IstreamInputStream buf(&stream);
  msg.ParseFromZeroCopyStream(&buf);
  
  r.resolutionWidth = msg.resolutionwidth();
  r.resolutionHeight = msg.resolutionheight();
  r.cameraID = (CameraInfo::CameraID) msg.cameraid();
  r.params.focus = msg.focus();
  r.params.openingAngleDiagonal = msg.openinganglediagonal();
  r.params.pixelSize = msg.pixelsize();
  
}
