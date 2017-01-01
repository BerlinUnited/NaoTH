
#ifndef _WHISTLE_CONTROL_H
#define _WHISTLE_CONTROL_H

#include <string>
#include "Tools/DataStructures/Printable.h"

namespace naoth
{
  class WhistleControl: public Printable
  {
  public:
    int onOffSwitch;

    WhistleControl();
    virtual void print(std::ostream& stream) const;

    virtual ~WhistleControl();
  };
}

#endif  /* _WHISTLE_CONTROL_H */

