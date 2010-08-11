
#ifndef _BATTERY_DATA_H
#define	_BATTERY_DATA_H

#include <string>
#include "naorunner/PlatformInterface/PlatformInterchangeable.h"
#include "naorunner/Tools/DataStructures/Printable.h"

class BatteryData: public PlatformInterchangeable, public Printable
{
public:
  double charge;

  BatteryData();
  virtual void print(ostream& stream) const;
  
  virtual ~BatteryData();
};


#endif	/* _BATTERY_DATA_H */

