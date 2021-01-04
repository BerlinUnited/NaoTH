
#ifndef AUDIO_CONTROL_H
#define AUDIO_CONTROL_H

#include <string>
#include "Tools/DataStructures/Printable.h"

namespace naoth
{
  class AudioControl: public Printable
  {
  public:
    bool capture;                 // controls if detection and audio capture is active or not
    unsigned int numChannels;
    unsigned int sampleRate;
    unsigned int buffer_size;

    AudioControl();
    virtual void print(std::ostream& stream) const;
  };
}

#endif  /* AUDIO_CONTROL_H */

