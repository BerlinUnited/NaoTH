/**
* The file declares a struct that stores audio data of up to four channels.
* On a V4, the four channels are:
* 0: left microphone
* 1: right microphone
* 2: front microphone
* 3: rear microphone

* And on a V5:
* 0: back left microphone
* 1: back right microphone
* 2: front left microphone
* 3: front right microphone
* @author <a href="mailto:schlottb@informatik.hu-berlin.de">Benjamin Schlotter</a>
*/

#ifndef _AUDIO_DATA_H
#define _AUDIO_DATA_H

#include "Tools/DataStructures/Printable.h"
#include <vector>
namespace naoth
{
  class AudioData : public Printable
  {
  public:
    int sampleRate;
    int numChannels;
    bool onlySoundInSet;

    std::vector<short> samples; /* Samples are interleaved. */

    AudioData();
    virtual ~AudioData();
    virtual void print(std::ostream& stream) const;
  };
}

#endif  /* _AUDIO_DATA_H */