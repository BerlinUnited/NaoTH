/* 
 * File:   GyrometerData.h
 * Author: Oliver Welter
 *
 */

#ifndef _GyrometerData_H_
#define _GyrometerData_H_

#include <string>
#include "Tools/DataStructures/Printable.h"
#include "Tools/DataStructures/Serializer.h"
#include "Tools/Math/Vector2.h"


namespace naoth
{

  class GyrometerData : public Printable
  {
  public:
    GyrometerData();
    ~GyrometerData();

    // the GyrRef provided by the robot (what is it for?)
    double ref;
    // raw data as provided by the sensors
    Vector2d rawData;
    // rawData scaled to radian/s
    Vector2d data;

    virtual void print(std::ostream& stream) const;
  };
  
  template<>
  class Serializer<GyrometerData>
  {
    public:
      static void serialize(const GyrometerData& representation, std::ostream& stream);
      static void deserialize(std::istream& stream, GyrometerData& representation);
  };
  
}
#endif  /* _GyrometerData_H_ */

