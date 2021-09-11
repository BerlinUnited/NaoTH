/**
 * @file NaoInfo.h
 *
 * Declaration of class NaoInfo
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Yuan Xu</a>
 * @author <a href="mailto:kaptur@informatik.hu-berlin.de">Christian Kaptur</a>
 */

#ifndef _NaoInfo_h_
#define _NaoInfo_h_

// TODO: this should not be here
#include <Representations/Infrastructure/FSRData.h>
#include <Representations/Infrastructure/CameraInfo.h>
#include <Tools/DataStructures/ParameterList.h>
#include <Tools/Math/Pose3D.h>

class NaoInfo
{
public:
  struct CameraTransformation
  {
    Vector3d offset;
    double rotationY;
  };

  class RobotDimensions : public ParameterList
  {
  public:
    RobotDimensions() : ParameterList("RobotDimensions")
    {
      // http://doc.aldebaran.com/2-4/family/robots/video_robot.html
      // http://doc.aldebaran.com/2-8/family/nao_technical/kinematics_naov6.html#naov6-urdf-files
      // Valid for NAO 4,5 & 6. For NAO V6 see the URDF file.
      PARAMETER_REGISTER(cameraTrans[naoth::CameraInfo::Top].offset.x)     = 58.71;
      PARAMETER_REGISTER(cameraTrans[naoth::CameraInfo::Top].offset.y)     = 0;
      PARAMETER_REGISTER(cameraTrans[naoth::CameraInfo::Top].offset.z)     = 63.64;
      PARAMETER_REGISTER(cameraTrans[naoth::CameraInfo::Top].rotationY)    = 1.2;

      PARAMETER_REGISTER(cameraTrans[naoth::CameraInfo::Bottom].offset.x)  = 50.71;
      PARAMETER_REGISTER(cameraTrans[naoth::CameraInfo::Bottom].offset.y)  = 0;
      PARAMETER_REGISTER(cameraTrans[naoth::CameraInfo::Bottom].offset.z)  = 17.74;
      PARAMETER_REGISTER(cameraTrans[naoth::CameraInfo::Bottom].rotationY) = 39.7;

      syncWithConfig();
      setCameraTransformation();
    }
    
   // TODO: clean this mess up 
   CameraTransformation cameraTransform[naoth::CameraInfo::numOfCamera];
   Pose3D cameraTransformation[naoth::CameraInfo::numOfCamera];

  private:

    // offset to the neck joint    
    CameraTransformation cameraTrans[naoth::CameraInfo::numOfCamera];

    void setCameraTransformation()
    {
      for(int i = 0; i < naoth::CameraInfo::numOfCamera; i++) {
        cameraTransform[i].offset = cameraTrans[i].offset;
        cameraTransform[i].rotationY = Math::fromDegrees(cameraTrans[i].rotationY);

        cameraTransformation[i].translation = cameraTrans[i].offset;
        cameraTransformation[i].rotation = RotationMatrix::getRotationY(Math::fromDegrees(cameraTrans[i].rotationY));
      }
    }
  };

public:
  //length of the links (arm, leg, etc.) - from the doc in sdk
  static const double NeckOffsetZ;
  static const double ShoulderOffsetY;
  static const double UpperArmLength;
  static const double LowerArmLength;
  static const double ShoulderOffsetZ;
  static const double HandOffsetX;
  static const double HipOffsetZ;
  static const double HipOffsetY;
  static const double ThighLength;
  static const double TibiaLength;
  static const double FootHeight;
  static const double HandOffsetZ;
  static const double ElbowOffsetY;

  static const double FSRMinForce;
  static const double FSRMaxForce;

  static const Vector3d FSRPositionsLeft[naoth::FSRData::numOfFSR];
  static const Vector3d FSRPositionsRight[naoth::FSRData::numOfFSR];
  
  static const RobotDimensions robotDimensions;

}; //end namespace NaoInfo

#endif //_NaoInfo_h_

