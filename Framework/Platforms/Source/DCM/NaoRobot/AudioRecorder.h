#ifndef _AudioRecorder_H
#define _AudioRecorder_H
#include <thread>
#include <mutex>
#include <iostream>
#include <pulse/simple.h>

#include "SoundConfig.h"

#include "Representations/Infrastructure/AudioData.h"
#include "Representations/Infrastructure/AudioControl.h"

namespace naoth
{
  class AudioRecorder
  {
  public:
    AudioRecorder();
    virtual ~AudioRecorder();

    void get(naoth::AudioData& data);
    void set(const naoth::AudioControl& controlData);

    std::vector<short> audioReadBuffer;
    unsigned int recordingTimestamp;

  protected:
    std::thread audioRecorderThread;
    std::mutex getMutex;
    std::mutex setMutex;
      
    // turn capture on and off
    bool capture;

    //Audio Control Parameter:
    std::string activeChannels;
    int numChannels;
    int sampleRate;
    int buffer_size;

    volatile bool running;
    bool recording;
    bool resetting;
    int deinitCyclesCounter;

    void execute();      
    void initAudio();
    void deinitAudio();

    pa_simple* paSimple;
  };
}

#endif // _AudioRecorder_H