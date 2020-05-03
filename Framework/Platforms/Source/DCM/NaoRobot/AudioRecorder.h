
#ifndef AudioRecorder_H
#define AudioRecorder_H

#include <thread>
#include <mutex>
#include <iostream>
#include <pulse/simple.h>
#include <condition_variable>

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
    std::mutex dataMutex;
    std::condition_variable new_command_avaliable;
      
    //audio control parameter
    AudioControl control;
    
    // puls audio device
    pa_simple* paSimple;

    // double buffering for the recording
    std::vector<short> audioBuffer[2];
    size_t readIdx;
    size_t writeIdx;

    unsigned int recordingTimestamp;
  };
}

#endif // AudioRecorder_H