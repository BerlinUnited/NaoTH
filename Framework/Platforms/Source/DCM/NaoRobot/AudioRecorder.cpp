#include "AudioRecorder.h"

#include <Tools/ThreadUtil.h>
#include "Tools/NaoTime.h"

#include <pulse/error.h>

using namespace std;
using namespace naoth;

AudioRecorder::AudioRecorder()
  :
  exiting(false),
  paSimple(NULL),
  readIdx(1),
  writeIdx(0),
  recordingTimestamp(0)
{
  std::cout << "[INFO] AudioRecorder thread started" << std::endl;
  audioRecorderThread = std::thread(&AudioRecorder::execute, this);
  ThreadUtil::setPriority(audioRecorderThread, ThreadUtil::Priority::lowest);
  ThreadUtil::setName(audioRecorderThread, "AudioRecorder");
}

AudioRecorder::~AudioRecorder()
{ 
  std::cout << "[AudioRecorder] stop wait" << std::endl;
  exiting = true;

  // NOTE: wake up the thread in case it was waiting for the new data
  new_command_avaliable.notify_one();
  if(audioRecorderThread.joinable()) {
    audioRecorderThread.join();
  }

  if(paSimple != NULL) {
    deinitAudio();
  }

  std::cout << "[AudioRecorder] stop done" << std::endl;
}

void AudioRecorder::execute()
{
  while(!exiting)
  {
    // initialize the audio stream if necessary
    if(control.capture && paSimple == NULL)
    {
      std::cout << "[AudioRecorder] start recording" << std::endl;
      initAudio();
    }
    
    // stop capturing
    if(!control.capture && paSimple != NULL) 
    {
      std::cout << "[AudioRecorder] stop recording" << std::endl;
      deinitAudio();
    }

    // capture data as long as the device is initialized
    if(paSimple != NULL)
    {
      int bytesToRead = audioBuffer[writeIdx].size() * static_cast<int>(sizeof(short));
      int error = 0;
      if (pa_simple_read(paSimple, &(audioBuffer[writeIdx][0]), bytesToRead, &error) < 0)
      {
        std::cerr << "[PulseAudio] pa_simple_read() failed: " << pa_strerror(error) << std::endl;
        exiting = true;
      } 
      else 
      {
        std::lock_guard<std::mutex> lock(dataMutex);
        //recordingTimestamp = NaoTime::getNaoTimeInMilliSeconds();
        
        // manually iterate the time of the audio stream to get a consistent mononous increment
        const unsigned int delta = (control.buffer_size * 1000) / control.sampleRate; // buffer length in ms
        recordingTimestamp += delta;
        
        // chech if there are any jumps, e.g., because the buffer was too small etc.
        if(abs(static_cast<int64_t>(recordingTimestamp) - static_cast<int64_t>(NaoTime::getNaoTimeInMilliSeconds())) > delta) {
          std::cout << "[AudioRecorder] WARNING: lost audio frame" << std::endl;
        }
        std::swap(writeIdx, readIdx);
      }
    } 
    else // not initialized, wait until new command data arrives
    { 
      std::cout << "[AudioRecorder] wait for new command" << std::endl;
      std::unique_lock<std::mutex> lock(dataMutex);
      new_command_avaliable.wait(lock);
      std::cout << "[AudioRecorder] continue thread" << std::endl;
    }

  } // end while
  
} // end execute


void AudioRecorder::set(const naoth::AudioControl& newControl)
{
  std::unique_lock<std::mutex> lock(dataMutex, std::try_to_lock);
  if ( lock.owns_lock() ) {
    // NOTE: changes in the recording parameters like sampleRate or numChannels 
    //       will only have an effect when capture is started. It means the parameters
    //       will not change during an ongoing recording.
    if(!control.capture) {
      control = newControl;
    } else {
      control.capture = newControl.capture;
    }

    if(control.capture) {
      // release the data lock and notify the thread (in case it was waiting)
      lock.unlock();
      new_command_avaliable.notify_one();
    }
  }
}

void AudioRecorder::get(AudioData& data)
{
  // if there is new data, copy it 
  std::unique_lock<std::mutex> lock(dataMutex, std::try_to_lock);
  if ( lock.owns_lock() && recordingTimestamp > data.timestamp) 
  {
    data.sampleRate  = control.sampleRate;
    data.numChannels = control.numChannels;
    data.samples     = audioBuffer[readIdx];
    data.timestamp   = recordingTimestamp;
  }
} // end AudioRecorder::get


void AudioRecorder::initAudio()
{
  std::lock_guard<std::mutex> lock(dataMutex);

  pa_sample_spec paSampleSpec = {
    .format   = PA_SAMPLE_S16LE,
    .rate     = (uint32_t)control.sampleRate,
    .channels = (uint8_t)control.numChannels
  };

  // NOTE: manually configure the channel map (because the default config seems to be different for NAO <= V4 and NAO >= V5)
  // channel-map = rear-left,rear-right,front-left,front-right
  pa_channel_map paChannelMap;
  paChannelMap.channels = (uint8_t)control.numChannels;
  paChannelMap.map[0] = PA_CHANNEL_POSITION_REAR_LEFT;
  paChannelMap.map[1] = PA_CHANNEL_POSITION_REAR_RIGHT;
  paChannelMap.map[2] = PA_CHANNEL_POSITION_FRONT_LEFT;
  paChannelMap.map[3] = PA_CHANNEL_POSITION_FRONT_RIGHT;
  
  // Create the recording stream
  int error = 0;
  paSimple = pa_simple_new(NULL, "AudioRecorder", PA_STREAM_RECORD, NULL, "AudioRecorder", &paSampleSpec, &paChannelMap, NULL, &error);

  if (!paSimple) {
    std::cerr << "[PulseAudio] pa_simple_new() failed: " << pa_strerror(error) << "\n" << std::endl;
  } else {
    std::cout << "[PulseAudio] device opened" << std::endl;
    std::cout << "[PulseAudio] Rate: " << paSampleSpec.rate <<std::endl;
    std::cout << "[PulseAudio] Channels: " << (int) paSampleSpec.channels <<std::endl;
    std::cout << "[PulseAudio] Buffer Size: " << control.buffer_size <<std::endl;

    // print the channel map
    char cm[PA_CHANNEL_MAP_SNPRINT_MAX];
    pa_channel_map_snprint(cm, sizeof(cm),&paChannelMap);
    std::cout << "[PulseAudio] channel map: " << cm << std::endl;
    
    // resize the buffers if necessary
    audioBuffer[readIdx].resize(control.buffer_size*control.numChannels);
    audioBuffer[writeIdx].resize(control.buffer_size*control.numChannels);

    // mark the time when recording started
    recordingTimestamp = NaoTime::getNaoTimeInMilliSeconds();
    
    // TODO: do we need that?
    // give the device a bit time (moved here from the main while-loop)
    usleep(128);
  }
} //end initAudio

void AudioRecorder::deinitAudio()
{
  if (paSimple != NULL)
  {
    std::cout << "[PulseAudio] device closed" << std::endl;
    pa_simple_free(paSimple);
    paSimple = NULL;
    
    // TODO: do we need that?
    // give the device a bit time (moved here from the main while-loop
    usleep(128);
  }
} //end deinitAudio
