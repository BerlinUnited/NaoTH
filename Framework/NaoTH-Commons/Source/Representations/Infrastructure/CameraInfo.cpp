/* 
 * File:   CameraInfo.cpp
 * Author: Claas-Norman Ritter
 *
 * Created on 1. Februar 2009, 20:07
 */


#include "CameraInfo.h"

#include <Messages/Representations.pb.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>

using namespace naoth;

void CameraInfo::setParameter(unsigned int resolutionWidth, unsigned int resolutionHeight, double openingAngleDiagonal)
{
  openingAngleDiagonal = Math::fromDegrees(openingAngleDiagonal);

  // calculate focal length
  double d2 = resolutionWidth * resolutionWidth + resolutionHeight * resolutionHeight;
  double halfDiagLength = 0.5 * sqrt(d2);

  focalLength = halfDiagLength / tan(0.5 * openingAngleDiagonal);

  // calculate opening angle
  openingAngleHeight = 2.0 * atan2((double)resolutionHeight,focalLength * 2.0);
  openingAngleWidth = 2.0 * atan2((double)resolutionWidth,focalLength * 2.0);

  // calculate optical senter
  opticalCenterY = resolutionHeight / 2;
  opticalCenterX = resolutionWidth / 2;

  // values needed by Image
  size = resolutionHeight * resolutionWidth;

}//end CameraInfo::setParameter

void CameraInfo::print(ostream& stream) const
{
  stream << "Roll Offset: "<< cameraRollOffset << " rad" << endl
         << "Tilt Offset: "<< cameraTiltOffset << " rad" <<  endl
         << "Opening Angle: " << openingAngleWidth << " rad, " << openingAngleHeight << " rad" << endl
         << "Optical Center: " << opticalCenterX << " Pixel, " << opticalCenterY << " Pixel" << endl
         << "Focal Length (calculated): "<< focalLength << " Pixel"<< endl
         << "Pixel Size: "<< pixelSize << " mm" << endl
         << "Focal Length: "<< focus << " mm" << endl
         << "Error to Center: " << xp << " mm, " << yp << " mm" << endl
         << "Radial Symmetric Error: " << k1 << " mm, " << k2 << " mm, " << k3 << " mm" << endl
         << "Radial Asymmetric and Tangential Error: " << p1 << " mm, " << p2 << " mm" << endl
         << "Affinity and Shearing Error: " << b1 << " mm, " << b2 << " mm" << endl
         ;
}//end CameraInfo::print


CameraInfoParameter::CameraInfoParameter():ParameterList("CameraInfo")
{
  PARAMETER_REGISTER(resolutionWidth) = 320;
  PARAMETER_REGISTER(resolutionHeight) = 240;
  PARAMETER_REGISTER(openingAngleDiagonal) = 58;
  PARAMETER_REGISTER(cameraRollOffset) = 0;
  PARAMETER_REGISTER(cameraTiltOffset) = 0;

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

  PARAMETER_REGISTER(cameraTrans[Top].offset.x) = 53.9;
  PARAMETER_REGISTER(cameraTrans[Top].offset.y) = 0;
  PARAMETER_REGISTER(cameraTrans[Top].offset.z) = 67.9;
  PARAMETER_REGISTER(cameraTrans[Top].rotationY) = 0;
  PARAMETER_REGISTER(cameraTrans[Bottom].offset.x) = 48.8;
  PARAMETER_REGISTER(cameraTrans[Bottom].offset.y) = 0;
  PARAMETER_REGISTER(cameraTrans[Bottom].offset.z) = 23.81;
  PARAMETER_REGISTER(cameraTrans[Bottom].rotationY) = 40.0;

  setParameter(resolutionWidth, resolutionHeight, openingAngleDiagonal);
  setCameraTrans();
}

void CameraInfoParameter::setCameraTrans()
{
  for(int i = 0; i < numOfCamera; i++)
  {
    transformation[i].translation = cameraTrans[i].offset;
    transformation[i].rotation = RotationMatrix::getRotationY(Math::fromDegrees(cameraTrans[i].rotationY));
  }
}//end CameraInfoParameter::setCameraTrans

void CameraInfoParameter::init()
{
  syncWithConfig();
  setParameter(resolutionWidth, resolutionHeight, openingAngleDiagonal);
  setCameraTrans();
}//end CameraInfoParameter::init


void Serializer<CameraInfo>::serialize(const CameraInfo& representation, std::ostream& stream)
{
  naothmessages::CameraInfo msg;
  msg.set_resolutionwidth(representation.resolutionWidth);
  msg.set_resolutionheight(representation.resolutionHeight);
  msg.set_cameraid((naothmessages::CameraID) representation.cameraID);
  msg.set_focallength(representation.focalLength);
  msg.set_openinganglewidth(representation.openingAngleWidth);
  msg.set_openingangleheight(representation.openingAngleHeight);
  msg.set_opticalcenterx(representation.opticalCenterX);
  msg.set_opticalcentery(representation.opticalCenterY);
  msg.set_size(representation.size);
  msg.set_camerarolloffset(representation.cameraRollOffset);
  msg.set_cameratiltoffset(representation.cameraTiltOffset);
  
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
  r.focalLength = msg.focallength();
  r.openingAngleWidth = msg.openinganglewidth();
  r.openingAngleHeight = msg.openingangleheight();
  r.opticalCenterX = msg.opticalcenterx();
  r.opticalCenterY = msg.opticalcentery();
  r.size = (unsigned long)msg.size();
  r.cameraRollOffset = msg.camerarolloffset();
  r.cameraTiltOffset = msg.cameratiltoffset();
  
}

