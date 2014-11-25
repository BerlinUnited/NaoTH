/* 
 * File:   InertialSensorData.h
 * Author: Oliver Welter
 *
 */

#ifndef _INERTIALSENSORDATA_H
#define _INERTIALSENSORDATA_H

#include <string>

#include "Tools/DataStructures/Printable.h"
#include "Tools/DataStructures/Serializer.h"

#include "Tools/Math/Common.h"
#include "Tools/Math/Vector2.h"

namespace naoth
{

  class InertialSensorData : public Printable
  {
  public:

    Vector2d data;

    virtual void print(std::ostream& stream) const;

  };
  
  template<>
  class Serializer<InertialSensorData>
  {
    public:
    static void serialize(const InertialSensorData& representation, std::ostream& stream);
    static void deserialize(std::istream& stream, InertialSensorData& representation);
  };
  
}

#endif  /* _INERTIALSENSORDATA_H */

