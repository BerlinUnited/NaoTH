/* 
 * File:   GyrometerData.h
 * Author: Oliver Welter
 *
 * Created on 2. März 2009, 00:46
 */

#ifndef _GYROMETERDATA_H
#define	_GYROMETERDATA_H

#include <string>
#include "naorunner/PlatformInterface/PlatformInterchangeable.h"
#include "naorunner/Tools/DataStructures/Printable.h"

using namespace std;

namespace naorunner
{

  class GyrometerData : public PlatformInterchangeable, public Printable
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
}
#endif	/* _GYROMETERDATA_H */

