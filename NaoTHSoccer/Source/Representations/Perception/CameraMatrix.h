/**
* @file CameraMatrix.h
* 
* Declaration of representation CameraMatrix
*/ 

#ifndef CameraMatrix_h
#define CameraMatrix_h

#include "Tools/Math/Pose3D.h"

#include "Tools/DataStructures/Printable.h"
#include "Tools/DataStructures/Serializer.h"

/**
  The Camera Matrix is a 3D pose which describes the coordinate transformation 
  from camera coordinates to the local coordinates of the robot. 
  In other words: it describes the position and the rotation of the camera relative 
  to the local coordinates of the robot. 
  
  Local coordinate system of the robot:
    The origin of the local robot coordinates is located on the ground floor 
    at the projection point of the robots Hip. Z-Axis points upwards, X-axis to
    the front, and Y-axis to the left from the perspective of the robot.
  
  Camera coordinate system:
    The origin of the coordinate system of the camera is located at the 
    focal point of the camera. The orientation of the axes is the same as 
    for the local robot coordinates: Z-Axis points upwards, X-axis to the 
    front, and Y-axis to the left from the perspective of the camera.
 
  Examples:
   
  1. transform the position of the ball in camera coordinates to the local 
     coordinates of the robot:

  Vector3d ball_camera(1000.0, 0.0, 0.0);
  Vector3d ball_local = getCameraMatrix() * ball_camera;

 
  2. transform the position of the ball in local coordinates of the robot 
     into the camera coordinates:

  Vector3d ball_local(1000.0, 500.0, 50.0);
  Vector3d ball_camera = getCameraMatrix().invert() * ball_local;

*/
class CameraMatrix : public Pose3D, public naoth::Printable
{
public:
  bool valid;
  unsigned int timestamp;

  CameraMatrix(): valid(false), timestamp(0) {}
  CameraMatrix(const Pose3D& pose): Pose3D(pose), valid(false) {}

  virtual void print(std::ostream& stream) const;
};

/**
 * @brief A camera matrix from a secondary camera
 */
class CameraMatrixTop : public CameraMatrix
{
public:
  CameraMatrixTop() {}
  CameraMatrixTop(const Pose3D& pose): CameraMatrix(pose) {}
  // this is needed to make CameraMatrixTop accessible from cppyy library in python
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
  class Serializer<CameraMatrixTop> : public Serializer<CameraMatrix>
  {};
}

#endif // CameraMatrix_h
