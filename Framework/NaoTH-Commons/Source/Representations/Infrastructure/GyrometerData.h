/* 
 * File:   GyrometerData.h
 * Author: Oliver Welter
 *
 * Created on 2. März 2009, 00:46
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
    double ref;
    Vector2<double> data;
    Vector2<double> rawData;
    Vector2<double> rawZero;

    GyrometerData();
    ~GyrometerData();

    void calibrate();

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

