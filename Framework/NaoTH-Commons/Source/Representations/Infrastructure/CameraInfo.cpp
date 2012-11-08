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

double CameraInfo::getFocalLength() const
{
  double d2 = resolutionWidth * resolutionWidth + resolutionHeight * resolutionHeight;
  double halfDiagLength = 0.5 * sqrt(d2);

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
  // TODO: shouldn't we cast to double here? Now we get a double that is acutally
  // an int...
  return resolutionWidth / 2;
}

double CameraInfo::getOpticalCenterY() const
{
  // TODO: shouldn't we cast to double here? Now we get a double that is acutally
  // an int...
  return resolutionHeight / 2;
}

unsigned long CameraInfo::getSize() const
{
  return resolutionHeight * resolutionWidth;
}

double CameraInfo::getOpeningAngleDiagonal() const
{
  return Math::fromDegrees(openingAngleDiagonal);
}

void CameraInfo::print(ostream& stream) const
{
  stream << "Camera selection: " << cameraID << endl
         << "Roll Offset: "<< cameraRollOffset << " rad" << endl
         << "Tilt Offset: "<< cameraTiltOffset << " rad" <<  endl
         << "Transformation: " << "x=" << transformation[cameraID].translation.x << ", "
            << " y=" <<transformation[cameraID].translation.y << ", "
            << " z=" <<transformation[cameraID].translation.z << ", "
            << " angleX=" <<transformation[cameraID].rotation.getXAngle() << " rad,"
            << " angleY=" <<transformation[cameraID].rotation.getYAngle() << " rad,"
            << " angleZ=" <<transformation[cameraID].rotation.getZAngle() << " rad" << endl
         << "Opening Angle (calculated): " << getOpeningAngleWidth() << " rad, " << getOpeningAngleHeight() << " rad" << endl
         << "Optical Center (calculated): " << getOpticalCenterX() << " Pixel, " << getOpticalCenterY() << " Pixel" << endl
         << "Focal Length (calculated): "<< getFocalLength() << " Pixel"<< endl
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
  PARAMETER_REGISTER(openingAngleDiagonal) = 72.6;
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

  PARAMETER_REGISTER(cameraTrans[Top].offset.x) = 58.71;
  PARAMETER_REGISTER(cameraTrans[Top].offset.y) = 0;
  PARAMETER_REGISTER(cameraTrans[Top].offset.z) = 63.64;
  PARAMETER_REGISTER(cameraTrans[Top].rotationY) = 0;
  PARAMETER_REGISTER(cameraTrans[Bottom].offset.x) = 50.71;
  PARAMETER_REGISTER(cameraTrans[Bottom].offset.y) = 0;
  PARAMETER_REGISTER(cameraTrans[Bottom].offset.z) = 17.74;
  PARAMETER_REGISTER(cameraTrans[Bottom].rotationY) = 39.7;

  init();
}

void CameraInfoParameter::init()
{
  // init config and values
  syncWithConfig();
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


void Serializer<CameraInfo>::serialize(const CameraInfo& representation, std::ostream& stream)
{
  naothmessages::CameraInfo msg;
  msg.set_resolutionwidth(representation.resolutionWidth);
  msg.set_resolutionheight(representation.resolutionHeight);
  msg.set_cameraid((naothmessages::CameraID) representation.cameraID);
  msg.set_focallength(representation.getFocalLength());
  msg.set_openinganglewidth(representation.getOpeningAngleWidth());
  msg.set_openingangleheight(representation.getOpeningAngleHeight());
  msg.set_opticalcenterx(representation.getOpticalCenterX());
  msg.set_opticalcentery(representation.getOpticalCenterY());
  msg.set_size(representation.getSize());
  msg.set_camerarolloffset(representation.cameraRollOffset);
  msg.set_cameratiltoffset(representation.cameraTiltOffset);
  
  // set transformations
  for(int camID=0; camID < CameraInfo::numOfCamera; camID++)
  {
    naothmessages::Pose3D* pose3d = msg.add_transformation();
    pose3d->mutable_translation()->set_x( representation.transformation[camID].translation.x );
    pose3d->mutable_translation()->set_y( representation.transformation[camID].translation.y );
    pose3d->mutable_translation()->set_z( representation.transformation[camID].translation.z );
    for(int i=0; i<3; i++){
      pose3d->add_rotation()->set_x( representation.transformation[camID].rotation[i].x );
      pose3d->mutable_rotation(i)->set_y( representation.transformation[camID].rotation[i].y );
      pose3d->mutable_rotation(i)->set_z( representation.transformation[camID].rotation[i].z );
    }
  }

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
  r.cameraRollOffset = msg.camerarolloffset();
  r.cameraTiltOffset = msg.cameratiltoffset();
  
  if(msg.transformation_size() == CameraInfo::numOfCamera)
  {
    for(int camID = 0; camID < CameraInfo::numOfCamera; camID++)
    {
      r.transformation[camID].translation.x = msg.transformation(camID).translation().x();
      r.transformation[camID].translation.y = msg.transformation(camID).translation().y();
      r.transformation[camID].translation.z = msg.transformation(camID).translation().z();

      for(int i=0; i<3; i++) {
        r.transformation[camID].rotation[i].x = msg.transformation(camID).rotation(i).x();
        r.transformation[camID].rotation[i].y = msg.transformation(camID).rotation(i).y();
        r.transformation[camID].rotation[i].z = msg.transformation(camID).rotation(i).z();
      }
    }
  }

}

