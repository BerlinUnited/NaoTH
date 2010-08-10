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

using namespace std;

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

  virtual void toDataStream(ostream& stream) const;
  virtual void fromDataStream(istream& stream);
  virtual void print(ostream& stream) const;
};

#endif	/* _ACCELEROMETERDATA_H */

