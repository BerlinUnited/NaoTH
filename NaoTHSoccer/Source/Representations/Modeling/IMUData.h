#ifndef IMUDATA_H
#define IMUDATA_H

#include <string>
#include <iomanip>

#include "Tools/DataStructures/Printable.h"
#include "Tools/DataStructures/Serializer.h"

#include "Tools/Math/Common.h"
#include "Tools/Math/Vector2.h"
#include "Tools/Math/Vector3.h"
#include "Tools/Math/RotationMatrix.h"

namespace naoth
{
class IMUData: public naoth::Printable
{
public:
  Vector3d location;            // simply integrated velocity
  Vector3d velocity;            // simply integrated acceleration
  Vector3d acceleration;        // gravity adjusted, UKF-filtered acceleration of body frame in "global" inertial frame
  Vector3d acceleration_sensor; // sensor values (TODO: offset adjustment)

  Vector3d rotation;                   // rotation vector representation (angle * unit_length_axis_vector) of UKF-filtered body rotation in inertial frame,
                                       // it rotates body coordinates into the "global" inertial frame
  Vector3d rotational_velocity;        // 3d vector representing UKF-filtered rotational velocity around each body frame axis
  Vector3d rotational_velocity_sensor; // 3d vector representing rotational velocity around each sensor frame axis

  Vector2d orientation;        // angles around x and y axis, only for high level decisions, deprecated for usage in coordinate transformations, use orientation_rotvec or rotation instead
  Vector3d orientation_rotvec; // rotation vector representation (angle * unit_length_axis_vector) of rotation considering only rotation around x and y axis,
                               // it rotates body coordinates into the "global" inertial frame neglecting any rotation around the global z axis

  bool has_been_reset;

  void reset(){
      location = Vector3d();
      velocity = Vector3d();
      acceleration = Vector3d();
      acceleration_sensor = Vector3d();
      rotation = Vector3d();
      rotational_velocity = Vector3d();
      rotational_velocity_sensor = Vector3d();
      orientation = Vector2d();
      orientation_rotvec = Vector3d();
      has_been_reset = true;
  }

  virtual void print(std::ostream& stream) const {

      stream << std::fixed << std::setprecision(4);

      stream << "all values in the local robot coordinate frame" << std::endl;
      stream << "has been reset: " << has_been_reset << std::endl;
      stream << "----location-----------------------" << std::endl;
      stream << "x   : " << std::setw(9) << location.x << " m"<< std::endl;
      stream << "y   : " << std::setw(9) << location.y << " m"<< std::endl;
      stream << "z   : " << std::setw(9) << location.z << " m"<< std::endl;
      stream << "----velocity-----------------------" << std::endl;
      stream << "x   : " << std::setw(8) << velocity.x << " m/s"<< std::endl;
      stream << "y   : " << std::setw(8) << velocity.y << " m/s"<< std::endl;
      stream << "z   : " << std::setw(8) << velocity.z << " m/s"<< std::endl;
      stream << "|v| : " << std::setw(8) << velocity.abs() << " m/s" << std::endl;
      stream << "----acceleration w/o g (sensor)----" << std::endl;
      stream << "x   : " << std::setw(7) << acceleration.x     << " (" << acceleration_sensor.x << ")" << " m/s^2" << std::endl;
      stream << "y   : " << std::setw(7) << acceleration.y     << " (" << acceleration_sensor.y << ")" << " m/s^2" << std::endl;
      stream << "z   : " << std::setw(7) << acceleration.z     << " (" << acceleration_sensor.z << ")" << " m/s^2" << std::endl;
      stream << "|a| : " << std::setw(7) << acceleration.abs() << " (" << acceleration_sensor.abs() << ")" << " m/s^2" << std::endl;
      stream << "----rotation-----------------------" << std::endl;
      RotationMatrix bodyIntoGlobalMapping(rotation);
      stream << "x : " << std::setw(7) << bodyIntoGlobalMapping.getXAngle() << " rad"<< std::endl;
      stream << "y : " << std::setw(7) << bodyIntoGlobalMapping.getYAngle() << " rad"<< std::endl;
      stream << "z : " << std::setw(7) << bodyIntoGlobalMapping.getZAngle() << " rad"<< std::endl;
      stream << "----rotational velocity------------" << std::endl;
      stream << "x : " << std::setw(7) << rotational_velocity.x << " (" << std::setw(7) << rotational_velocity_sensor.x << ")" << " rad/s"<< std::endl;
      stream << "y : " << std::setw(7) << rotational_velocity.y << " (" << std::setw(7) << rotational_velocity_sensor.y << ")" << " rad/s"<< std::endl;
      stream << "z : " << std::setw(7) << rotational_velocity.z << " (" << std::setw(7) << rotational_velocity_sensor.z << ")" << " rad/s"<< std::endl;
      stream << "----orientation--------------------" << std::endl;
      stream << "x : " << std::setw(9) << Math::toDegrees(orientation.x) << " °"<< std::endl;
      stream << "y : " << std::setw(9) << Math::toDegrees(orientation.y) << " °"<< std::endl;
      stream << "----orientation as rotation matrix-" << std::endl;
      RotationMatrix orientationAsRotMat(orientation_rotvec);
      stream << "x : " << std::setw(7) << orientationAsRotMat.getXAngle() << " rad"<< std::endl;
      stream << "y : " << std::setw(7) << orientationAsRotMat.getYAngle() << " rad"<< std::endl;
      stream << "z : " << std::setw(7) << orientationAsRotMat.getZAngle() << " rad"<< std::endl;
  }
};

  template<>
  class Serializer<IMUData>
  {
  public:
    static void serialize(const IMUData& representation, std::ostream& stream);
    static void deserialize(std::istream& stream, IMUData& representation);
  };
}

#endif // IMUDATA_H
