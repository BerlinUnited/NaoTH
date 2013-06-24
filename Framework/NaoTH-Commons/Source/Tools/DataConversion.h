/**
* @file DataConversion.h
* 
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* convert data to string or string to data
* or data to protobuf message
*/

#ifndef _DATACONVERSION_H
#define _DATACONVERSION_H

#include <cstring>
#include <sstream>
#include "Messages/CommonTypes.pb.h"
#include "Tools/Math/Pose2D.h"
#include "Tools/Math/Pose3D.h"
#include "Tools/Math/Vector3.h"

namespace naoth
{

namespace DataConversion
{
  template<typename T>
  bool strTo(const std::string& parameter, T& value)
  {
    std::stringstream ss(parameter);
    ss >> std::boolalpha >> value;
    return true;
  }//end strto

  template<typename T>
  std::string toStr(const T& value)
  {
    std::stringstream ss;
    ss << std::boolalpha << value;
    return ss.str();
  }

  // Vector2d
  void toMessage(const Vector2<double>& data, naothmessages::DoubleVector2& msg);
  void fromMessage(const naothmessages::DoubleVector2& msg, Vector2<double>& data);

  // Vevtor3d
  void toMessage(const Vector3<double>& data, naothmessages::DoubleVector3& msg);
  void fromMessage(const naothmessages::DoubleVector3& msg, Vector3<double>& data);

  // Pose2D
  void toMessage(const Pose2D& data, naothmessages::Pose2D& msg);
  void fromMessage(const naothmessages::Pose2D& msg, Pose2D& data);

  // Pose3D
  void toMessage(const Pose3D& data, naothmessages::Pose3D& msg);
  void fromMessage(const naothmessages::Pose3D& msg, Pose3D& data);

} // namespace DataConversion

} // namespace naoth

#endif  /* _DATACONVERSION_H */


