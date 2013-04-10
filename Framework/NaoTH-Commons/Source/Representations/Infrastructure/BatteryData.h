
#ifndef _BATTERY_DATA_H
#define _BATTERY_DATA_H

#include <string>
#include "Tools/DataStructures/Printable.h"

namespace naoth
{
  class BatteryData: public Printable
  {
  public:
    double charge;

    BatteryData();
    virtual void print(std::ostream& stream) const;

    virtual ~BatteryData();
  };
}

#endif  /* _BATTERY_DATA_H */

