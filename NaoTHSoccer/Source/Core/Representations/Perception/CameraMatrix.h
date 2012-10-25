/**
 * @file CameraMatrix.h
 * 
 * Declaration of class CameraMatrix
 */ 

#ifndef _CameraMatrix_h_
#define _CameraMatrix_h_

#include "Tools/Math/Pose3D.h"
#include "Tools/Math/Line.h"

#include "Tools/DataStructures/Printable.h"
#include "Tools/DataStructures/Serializer.h"

/**
* Matrix describing transformation from neck joint to camera.
*/
class CameraMatrix : public Pose3D, public naoth::Printable
{
public:
  CameraMatrix() {}
  CameraMatrix(const Pose3D& pose): Pose3D(pose), cameraNumber(-1), valid(false) {}

  int cameraNumber;
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
};

namespace naoth
{
  template<>
  class Serializer<CameraMatrix>
  {
  public:
    static void serialize(const CameraMatrix& representation, std::ostream& stream);
    static void deserialize(std::istream& stream, CameraMatrix& representation);
  };
}

#endif // _CameraMatrix_h_
