/**
* The file declares a struct that stores audio data of up to four channels.
*
* On a V4, the four channels are:
* 0: front-left microphone
* 1: front-right microphone
* 2: front-center microphone
* 3: rear-center microphone

* And on a V5:
* /usr/share/pulseaudio/alsa-mixer/profile-sets/aldebaran-robotics/tangential-trapeze.conf
* 0: rear-left microphone
* 1: rear-right microphone
* 2: front-left microphone
* 3: front-right microphone
* (map: rear-left,rear-right,front-left,front-right)
*
* @author <a href="mailto:schlottb@informatik.hu-berlin.de">Benjamin Schlotter</a>
*/

#ifndef _AUDIO_DATA_H_
#define _AUDIO_DATA_H_

#include "Tools/DataStructures/Printable.h"
#include "Tools/DataStructures/Serializer.h"

#include <vector>

namespace naoth
{
class AudioData : public Printable
{
public:
    // this is the map of channel for NAO V5 and V6
  enum ChannelMap {
    REAR_LEFT   = 0,
    REAR_RIGHT  = 1,
    FRONT_LEFT  = 2,
    FRONT_RIGHT = 3,
    NUMBER_OF_CHANNELS = 4
  };

public:
  unsigned int sampleRate;
  unsigned int numChannels;
  unsigned int timestamp;

  std::vector<short> samples; /* Samples are interleaved. */

  AudioData();
  virtual void print(std::ostream& stream) const;
};

template<>
class Serializer<AudioData>
{
public:
  static void serialize(const AudioData& representation, std::ostream& stream);
  static void deserialize(std::istream& stream, AudioData& representation);
};
}//end namespace naoth

#endif  /* _AUDIO_DATA_H_ */
