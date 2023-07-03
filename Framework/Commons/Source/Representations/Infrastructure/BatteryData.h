
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
    double charge;  // charge of the battery (% in [0,1])
    double current; // electric current (charging or discharging) (A in [-2.0, 2.1])   
    
    // NOTE: on NAO6 the temperature seems to be always around 3.0 no matter if it's charging or not.
    // TODO: docu says the temperature is measured in "%". What does it mean?
    double temperature; // Temperature (%)

    BatteryData();
    virtual ~BatteryData() {}

    virtual void print(std::ostream& stream) const;
  };
}

#endif  /* BATTERY_DATA_H */
