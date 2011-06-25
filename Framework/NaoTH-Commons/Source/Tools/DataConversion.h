/**
* @file DataConversion.h
* 
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* convert data to string or string to data
* or data to protobuf message
*/

#ifndef _DATACONVERSION_H
#define	_DATACONVERSION_H

#include <cstring>
#include <sstream>
#include "Messages/CommonTypes.pb.h"
#include "Tools/Math/Pose2D.h"

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

  // Pose2D
  void toMessage(const Pose2D& data, naothmessages::Pose2D& msg);
  void fromMessage(const naothmessages::Pose2D& msg, Pose2D& data);

} // namespace DataConversion

} // namespace naoth

#endif	/* _DATACONVERSION_H */


