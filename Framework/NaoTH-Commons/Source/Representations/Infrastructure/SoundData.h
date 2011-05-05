/* 
 * File:   SoundData.h
 * Author: Oliver Welter
 *
 * Created on 16. Juni 2009, 21:29
 */

#ifndef _SOUNDDATA_H
#define	_SOUNDDATA_H

#include <string>
#include <iostream>
#include "Tools/DataStructures/Printable.h"
#include "Tools/DataStructures/RingBuffer.h"
#include "PlatformInterface/PlatformInterchangeable.h"

using namespace std;

namespace naoth
{

  class SoundData : public PlatformInterchangeable, public Printable
  {
  public:
    bool mute;
    string soundFile;
    string snd_ctl_dump;
    int period_size;            /* auto */
    int periods;            /* auto */

    int rate;
    int channels;

    SoundData();
    ~SoundData();

    virtual void print(ostream& stream) const = 0;
  };

  class SoundPlayData : public SoundData
  {
  public:
    enum SpeakerID
    {
      LeftSpeaker,
      RightSpeaker,
      numOfSpeaker
    };

    int volume[numOfSpeaker];

    SoundPlayData();
    ~SoundPlayData();

    virtual void print(ostream& stream) const;
  };

  class SoundCaptureData : public SoundData
  {
  public:
    enum MicrophoneID
    {
      LeftMicrophone,
      RightMicrophone,
      frontMicrophone,
      rearMicrophone,
      numOfMicrophone
    };

    RingBuffer<int,20000> buffers[numOfMicrophone];
    int volume[numOfMicrophone];
    unsigned int lastBytesCaptured;

    SoundCaptureData();
    ~SoundCaptureData();

    virtual void print(ostream& stream) const;
  };
}

#endif	/* _SOUNDDATA_H */

