
#ifndef _WHISTLE_CONTROL_H
#define _WHISTLE_CONTROL_H

#include <string>
#include "Tools/DataStructures/Printable.h"

namespace naoth
{
  class WhistleControl: public Printable
  {
  public:
    int onOffSwitch;              // controls if detection and audio capture is active or not
    std::string activeChannels;   // defines channels to be used in multi channel mode
    int numChannels;
    int sampleRate;
    bool onlySoundInSet;
    int buffer_size;

    WhistleControl();
    virtual void print(std::ostream& stream) const;

    virtual ~WhistleControl();
  };
}

#endif  /* _WHISTLE_CONTROL_H */

