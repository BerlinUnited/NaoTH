/* 
 * File:   GyrometerData.h
 * Author: Oliver Welter
 *
 * Created on 2. März 2009, 00:46
 */

#ifndef _GYROMETERDATA_H
#define	_GYROMETERDATA_H

#include <string>
#include "Tools/ModuleFramework/Representation.h"
#include "Tools/DataStructures/Streamable.h"
#include "PlatformInterface/PlatformInterchangeable.h"
#include "Messages/naothMessages.pb.h"
#include "Tools/DataStructures/Printable.h"

using namespace std;

class GyrometerData : public Streamable, public PlatformInterchangeable, public Printable
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

  virtual void toDataStream(ostream& stream) const;
  virtual void fromDataStream(istream& stream);
  virtual void print(ostream& stream) const;
};

REPRESENTATION_INTERFACE(GyrometerData);

#endif	/* _GYROMETERDATA_H */

