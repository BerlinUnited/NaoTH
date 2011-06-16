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
#include "Tools/Math/Vector2.h"

using namespace std;

namespace naoth
{

  class InertialSensorData : public Streamable, public Printable, public PlatformInterchangeable
  {
  public:

    InertialSensorData();
    ~InertialSensorData();

    Vector2<double> data;
    static Vector2<double> offset;

    Vector2<double> get() const {
      Vector2<double> v = data + offset;
      v.x = Math::normalizeAngle(v.x);
      v.y = Math::normalizeAngle(v.y);
      return v;
    }

    void init();

    void calibrate();
    void stopCalibrating();

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

