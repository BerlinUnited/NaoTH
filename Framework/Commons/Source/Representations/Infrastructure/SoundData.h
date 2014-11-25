/* 
 * File:   SoundData.h
 * Author: Oliver Welter
 *
 * Created on 16. Juni 2009, 21:29
 */

#ifndef _SoundData_H_
#define _SoundData_H_

#include <string>
#include <iostream>
#include "Tools/DataStructures/Printable.h"
#include "Tools/DataStructures/RingBuffer.h"

namespace naoth
{

  class SoundData : public Printable
  {
  public:
    bool mute;
    std::string soundFile;
    std::string snd_ctl_dump;
    int period_size;        /* auto */
    int periods;            /* auto */

    int rate;
    int channels;

    SoundData();
    ~SoundData();

    virtual void print(std::ostream& stream) const = 0;
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

    virtual void print(std::ostream& stream) const;
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

    virtual void print(std::ostream& stream) const;
  };
}

#endif  /* _SoundData_H_ */

