/**
  @file AudioRecorder.h
  @author Heinrich Mellmann <mellmann@informatik.hu-berlin.de>
  
  Records raw sound using PulseAudio. Recording is threaded and is performed blockwise with double buffering.
  
  INFO: Get Information about sound devices and configs:

    pacmd list-sources

  TEST: test recording by using pulse audio recorder

    parecord -r --raw --format=s16le --rate=8000 --channel-map=rear-left,rear-right,front-left,front-right --channels=4 > test.raw

  profiles:
    NAO V5: /usr/share/pulseaudio/alsa-mixer/profile-sets/aldebaran-robotics/tangential-trapeze.conf
    NAO V6: /usr/share/pulseaudio/alsa-mixer/profile-sets/aldebaran-robotics/PCH_trapeze.conf    
*/

#ifndef AudioRecorder_H
#define AudioRecorder_H

// cout
#include <iostream>

// threading
#include <thread>
#include <mutex>
#include <condition_variable>

// pulseaudio: we use the simple API for synchroneous recording
// example for using pa_simple API:
//   http://0pointer.de/lennart/projects/pulseaudio/doxygen/parec-simple_8c-example.html
#include <pulse/simple.h>

// representations
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