
#ifndef BATTERY_DATA_H
#define BATTERY_DATA_H

#include <string>
#include "Tools/DataStructures/Printable.h"

namespace naoth
{
  class BatteryData: public Printable
  {
  public:
    // http://doc.aldebaran.com/2-8/family/nao_technical/lola/actuator_sensor_names.html#battery
    double charge;      // Current Sensor (A)
    // TODO: [0,1] or [0,100]?
    double current;     // Charge Sensor (%)
    // TODO: in % of what?!
    double temperature; // Temperature Sensor (%)

    BatteryData();
    virtual ~BatteryData() {}

    virtual void print(std::ostream& stream) const;
  };
}

#endif  /* BATTERY_DATA_H */
