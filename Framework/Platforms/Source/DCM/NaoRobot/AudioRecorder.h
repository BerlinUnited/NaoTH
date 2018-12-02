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

  private:
    void execute();      
    void initAudio();
    void deinitAudio();

  private:

    // thread control
    volatile bool exiting;
    std::thread audioRecorderThread;
    std::mutex getMutex;
    std::mutex setMutex;
      
    //audio control parameter
    std::string activeChannels; // TODO: no used here
    int numChannels;
    int sampleRate;
    int buffer_size;

    // turn capture on and off
    bool capture;
    
    bool resetting;
    int deinitCyclesCounter;

    // puls audio device
    bool initialized;
    pa_simple* paSimple;

    std::vector<short> audioReadBuffer;
    unsigned int recordingTimestamp;

  };
}

#endif // _AudioRecorder_H