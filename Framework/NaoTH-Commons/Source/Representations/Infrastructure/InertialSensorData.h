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
#include "Tools/DataStructures/Serializer.h"

#include "Tools/Math/Common.h"

using namespace std;

namespace naoth
{

  class InertialSensorData : public Streamable, public Printable, public PlatformInterchangeable
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
        return Math::normalizeAngle(data[id] + offset[id]);
    }

    void init();

    void calibrate();
    void stopCalibrating();
    
    static string getInertialSensorName(InertialSensorID angle);

    virtual void print(ostream& stream) const;

  private:
    // member for calibration
    static int calibrateNum;
    static string configGroup;
  };
  
  template<>
  class Serializer<InertialSensorData>
  {
    public:
    static void serialize(const InertialSensorData& representation, std::ostream& stream);
    static void deserialize(std::istream& stream, InertialSensorData& representation);
  };
  
}

#endif	/* _INERTIALSENSORDATA_H */

