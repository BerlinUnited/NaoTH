/* 
 * File:   InertialSensorData.h
 * Author: Oliver Welter
 *
 * Created on 2. März 2009, 00:31
 */

#ifndef _INERTIALSENSORDATA_H
#define	_INERTIALSENSORDATA_H

#include <string>

#include "Tools/DataStructures/Printable.h"
#include "PlatformInterface/PlatformInterchangeable.h"

using namespace std;

class InertialSensorData : public Printable, public PlatformInterchangeable
{
public:
 
  enum InertialSensorID
  {
    X,
    Y,
    numOfInertialSensor
  };

  InertialSensorData();
  ~InertialSensorData();

  double data[numOfInertialSensor];
  double lastData[numOfInertialSensor][4];
  static double offset[numOfInertialSensor];

  double get(InertialSensorID id) const {
      return data[id] + offset[id];
  }

  void init();

  void calibrate();
  void stopCalibrating();
  
  static string getInertialSensorName(InertialSensorID angle);

  virtual void print(ostream& stream) const;

private:
  // member for calibration
  static int calibrateNum;
  static string configFileName;
};


#endif	/* _INERTIALSENSORDATA_H */

