
#ifndef _BATTERY_DATA_H
#define	_BATTERY_DATA_H

#include <string>
#include "Tools/ModuleFramework/Representation.h"
#include "PlatformInterface/PlatformInterchangeable.h"
#include "Tools/DataStructures/Printable.h"

class BatteryData: public PlatformInterchangeable, public Printable
{
public:
  double charge;

  BatteryData();
  virtual void print(ostream& stream) const;
  
  virtual ~BatteryData();
};

REPRESENTATION_INTERFACE(BatteryData);

#endif	/* _BATTERY_DATA_H */

