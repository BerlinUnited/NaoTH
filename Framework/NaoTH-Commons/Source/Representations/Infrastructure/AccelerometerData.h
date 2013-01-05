/* 
 * File:   AccelerometerData.h
 * Author: Oliver Welter
 *
 * Created on 2. März 2009, 00:47
 */

#ifndef _AccelerometerData_H_
#define _AccelerometerData_H_

#include <string>
#include "Tools/DataStructures/Printable.h"
#include "Tools/Math/Vector3.h"
#include "Tools/DataStructures/Serializer.h"

namespace naoth
{

  class AccelerometerData : public Printable
  {
  public:
    // normalized to m/s^2
    Vector3<double> data;

    // raw data read from sensors
    Vector3<double> rawData;


    AccelerometerData();
    ~AccelerometerData();

    Vector3<double> getAcceleration() const;

    virtual void print(std::ostream& stream) const;
  };
  
  template<>
  class Serializer<AccelerometerData>
  {
    public:
    static void serialize(const AccelerometerData& representation, std::ostream& stream);
    static void deserialize(std::istream& stream, AccelerometerData& representation);
  };
  
}
#endif  /* _AccelerometerData_H_ */

