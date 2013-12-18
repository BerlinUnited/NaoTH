/**
 * @file CameraMatrix.h
 * 
 * Declaration of class CameraMatrix
 */ 

#ifndef _CameraMatrix_h_
#define _CameraMatrix_h_

#include "Tools/Math/Pose3D.h"
#include "Tools/Math/Line.h"
#include "Representations/Infrastructure/CameraInfo.h"

#include "Tools/DataStructures/Printable.h"
#include "Tools/DataStructures/Serializer.h"

#include <Tools/DataStructures/RingBuffer.h>

/**
* Matrix describing transformation from neck joint to camera.
*/
class CameraMatrix : public Pose3D, public naoth::Printable
{
private:
  naoth::CameraInfo::CameraID cameraID;
public:
  CameraMatrix(): cameraID(naoth::CameraInfo::Bottom), valid(false), timestamp(0) {}
  CameraMatrix(const Pose3D& pose): Pose3D(pose), cameraID(naoth::CameraInfo::Bottom), valid(false) {}

  // TODO: does anybody need it?!
  
  bool valid;
  unsigned int timestamp;

  virtual void print(std::ostream& stream) const
  {
    stream << "camera = " << cameraID << std::endl;
    stream << "x-angle [deg] = " << Math::toDegrees(rotation.getXAngle()) << std::endl;
    stream << "y-angle [deg] = " << Math::toDegrees(rotation.getYAngle()) << std::endl;
    stream << "z-angle [deg] = " << Math::toDegrees(rotation.getZAngle()) << std::endl;
    stream << "x-translation [mm] = " << translation.x << std::endl;
    stream << "y-translation [mm] = " << translation.y << std::endl;
    stream << "z-translation [mm] = " << translation.z << std::endl;
    stream << "valid = " << valid << std::endl;
    stream << "timestamp = " << timestamp << std::endl;
  }//end print
};

/**
 * @brief A camera matrix from a secondary camera
 */
class CameraMatrixTop : public CameraMatrix
{
public:
  virtual ~CameraMatrixTop() {}
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

  template<>
  class Serializer<CameraMatrixTop>
  {
  public:
    static void serialize(const CameraMatrixTop& representation, std::ostream& stream);
    static void deserialize(std::istream& stream, CameraMatrixTop& representation);
  };
}

#endif // _CameraMatrix_h_
