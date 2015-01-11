/**
* @file OdometryData.h
* Contains the OdometryData class.
*
* @author Max Risler (GT - Implementation)
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
*/

#ifndef _OdometryData_h_
#define _OdometryData_h_

#include <Tools/DataStructures/Printable.h>
#include <Tools/Math/Pose2D.h>
#include <Tools/DataStructures/Serializer.h>

/**
* OdometryData
* OdometryData contains an approximation of overall movement the robot has done.
* @attention Only use differences of OdometryData at different times.
* Position in mm
*/
class OdometryData : public Pose2D, public naoth::Printable
{
public:
  OdometryData() {}
  ~OdometryData(){}

  virtual void print(std::ostream& stream) const
  {
    stream << "x = " << translation.x << std::endl;
    stream << "y = " << translation.y << std::endl;
    stream << "rotation = " << rotation << std::endl;
  }//end print
};

namespace naoth
{
  template<>
  class Serializer<OdometryData>
  {
  public:
    static void serialize(const OdometryData& representation, std::ostream& stream);
    static void deserialize(std::istream& stream, OdometryData& representation);
  };
}

#endif //_OdometryData_h_
