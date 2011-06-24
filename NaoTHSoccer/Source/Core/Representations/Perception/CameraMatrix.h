/**
 * @file CameraMatrix.h
 * 
 * Declaration of class CameraMatrix
 */ 

#ifndef __CameraMatrix_h_
#define __CameraMatrix_h_

#include "Tools/Math/Pose3D.h"
#include "Tools/Math/Line.h"
#include "Tools/DataStructures/Printable.h"

/**
* Matrix describing transformation from neck joint to camera.
*/
class CameraMatrix : public Pose3D, public naoth::Printable
{
public:
  CameraMatrix() {}
  CameraMatrix(const Pose3D& pose): Pose3D(pose), cameraNumber(-1), valid(false) {}

  int cameraNumber;

  Math::LineSegment horizon;

  bool valid;

  virtual void print(ostream& stream) const
  {
    stream << "camera = " << cameraNumber << endl;
    stream << "x-angle [deg] = " << Math::toDegrees(rotation.getXAngle()) << endl;
    stream << "y-angle [deg] = " << Math::toDegrees(rotation.getYAngle()) << endl;
    stream << "z-angle [deg] = " << Math::toDegrees(rotation.getZAngle()) << endl;
    stream << "x-translation [mm] = " << translation.x << endl;
    stream << "y-translation [mm] = " << translation.y << endl;
    stream << "z-translation [mm] = " << translation.z << endl;
    stream << "valid = " << valid << endl;
  }//end print
/*
  virtual void toDataStream(ostream& stream) const
  {
    naothmessages::CameraMatrix msg;
    msg.mutable_pose()->mutable_translation()->set_x( translation.x );
    msg.mutable_pose()->mutable_translation()->set_y( translation.y );
    msg.mutable_pose()->mutable_translation()->set_z( translation.z );
    for(int i=0; i<3; i++){
      msg.mutable_pose()->add_rotation()->set_x( rotation[i].x );
      msg.mutable_pose()->mutable_rotation(i)->set_y( rotation[i].y );
      msg.mutable_pose()->mutable_rotation(i)->set_z( rotation[i].z );
    }
    google::protobuf::io::OstreamOutputStreamLite buf(&stream);
    msg.SerializeToZeroCopyStream(&buf);
  }

  virtual void fromDataStream(istream& stream)
  {
    naothmessages::CameraMatrix msg;
    google::protobuf::io::IstreamInputStreamLite buf(&stream);
    msg.ParseFromZeroCopyStream(&buf);
    translation.x = msg.pose().translation().x();
    translation.y = msg.pose().translation().y();
    translation.z = msg.pose().translation().z();
    for(int i=0; i<3; i++) {
      rotation[i].x = msg.pose().rotation(i).x();
      rotation[i].y = msg.pose().rotation(i).y();
      rotation[i].z = msg.pose().rotation(i).z();
    }
  }
  */
};

#endif //__CameraMatrix_h_
