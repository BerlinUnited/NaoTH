/* 
 * File:   GyrometerData.h
 * Author: Oliver Welter
 *
 * Created on 2. März 2009, 00:46
 */

#ifndef _GYROMETERDATA_H
#define	_GYROMETERDATA_H

#include <string>
#include "PlatformInterface/PlatformInterchangeable.h"
#include "Tools/DataStructures/Printable.h"
#include "Tools/DataStructures/Streamable.h"
#include "Tools/DataStructures/Serializer.h"

using namespace std;

namespace naoth
{

  class GyrometerData : public PlatformInterchangeable, public Printable, public Streamable
  {
  public:
    enum GyrometerID
    {
      X,
      Y,
      numOfGyrometer
    };

    double data[numOfGyrometer];
    double rawData[numOfGyrometer+1];
    double rawZero[numOfGyrometer];

    GyrometerData();
    ~GyrometerData();

    void calibrate();

    static string getGyrometerName(GyrometerID gyro);

    virtual void print(ostream& stream) const;
  };
  
  template<>
  class Serializer<GyrometerData>
  {
    public:
      static void serialize(const GyrometerData& representation, std::ostream& stream);
      static void deserialize(std::istream& stream, GyrometerData& representation);
  };
  
}
#endif	/* _GYROMETERDATA_H */

