
#ifndef _AUDIO_CONTROL_H
#define _AUDIO_CONTROL_H

#include <string>
#include "Tools/DataStructures/Printable.h"

namespace naoth
{
  class AudioControl: public Printable
  {
  public:
    int onOffSwitch;              // controls if detection and audio capture is active or not
    std::string activeChannels;   // defines channels to be used in multi channel mode
    int numChannels;
    int sampleRate;
    bool onlySoundInSet;
    int buffer_size;

    AudioControl();
    virtual void print(std::ostream& stream) const;

    virtual ~AudioControl();
  };
}

#endif  /* _AUDIO_CONTROL_H */

