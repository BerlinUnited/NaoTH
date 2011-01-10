/* 
 * File:   AccelerometerData.h
 * Author: Oliver Welter
 *
 * Created on 2. März 2009, 00:47
 */

#ifndef _ACCELEROMETERDATA_H
#define	_ACCELEROMETERDATA_H

#include <string>
#include "Tools/DataStructures/Printable.h"
#include "PlatformInterface/PlatformInterchangeable.h"
#include "Tools/Math/Vector3.h"
#include "Tools/DataStructures/Serializer.h"

using namespace std;

namespace naoth
{

  class AccelerometerData : public Printable, public PlatformInterchangeable
  {
  public:
    enum AccelerometerID
    {
      X,
      Y,
      Z,
      numOfAccelerometer
    };
    double data[numOfAccelerometer];
    double rawData[numOfAccelerometer];

    AccelerometerData();
    ~AccelerometerData();

    Vector3<double> getAcceleration() const;

    static string getAccelerometerName(AccelerometerID acc);

    virtual void print(ostream& stream) const;
  };
  
  template<>
  class Serializer<AccelerometerData>
  {
    public:
    static void serialize(const AccelerometerData& representation, std::ostream& stream);
    static void deserialize(std::istream& stream, AccelerometerData& representation);
  };
  
}
#endif	/* _ACCELEROMETERDATA_H */

