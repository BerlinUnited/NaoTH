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

#include <Representations/Infrastructure/FrameInfo.h>

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

  class SayRequest : public Printable
  {
  public:

    SayRequest() : message_len(0) {}

    const std::string getMessage() const {
      return std::string(message,message_len);
    }

    const FrameInfo& getUpdateTime() const {
      return updateTime;
    }
    
    void setMessage(const std::string& message, const FrameInfo& updateTime) {
      if (message.size() < 1024) {
        strcpy(this->message, message.c_str());
        message_len = message.size();
      }
      this->updateTime = updateTime;
    }

    virtual void print(std::ostream& stream) const {
      if(message_len == 0) {
        stream << "nothing to say\n";
      } else {
        stream << "last message: " << getMessage() << "\n";
      }
    }

  private:
    FrameInfo updateTime;

    // HACK: this is needed for shared memory
    char message[1024];
    size_t message_len;

  };
}

#endif  /* _SoundData_H_ */

