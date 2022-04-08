
#ifndef AUDIO_CONTROL_H
#define AUDIO_CONTROL_H

#include "Tools/DataStructures/Printable.h"
#include "Tools/DataStructures/ParameterList.h"

namespace naoth
{
  class AudioControl: public Printable, public ParameterList
  {
  public:
    // controls if audio capture is active or not
    bool capture;

    // parameters for the audio capture
    unsigned int numChannels;
    unsigned int sampleRate;

    // number of samples per channel, 
    // i.e., the actual size of the whole buffer is calculated as 
    //   buffer_size * numChannels
    unsigned int buffer_size;

    AudioControl();
    virtual void print(std::ostream& stream) const;
  };
}

#endif  /* AUDIO_CONTROL_H */

