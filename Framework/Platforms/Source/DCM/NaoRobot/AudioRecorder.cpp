#include "AudioRecorder.h"

#include <Tools/ThreadUtil.h>
#include "Tools/NaoTime.h"

#include <chrono>
#include <pulse/error.h>

using namespace std;
using namespace naoth;


AudioRecorder::AudioRecorder()
  :
  exiting(false),
  deinitCyclesCounter(0),
  paSimple(NULL),
  readIdx(1),
  writeIdx(0),
  recordingTimestamp(0)
{
  std::cout << "[INFO] AudioRecorder thread started" << std::endl;
  audioRecorderThread = std::thread([this] {this->execute();});
  ThreadUtil::setPriority(audioRecorderThread, ThreadUtil::Priority::lowest);

  //The following parameters are used by bhuman:
  //TODO implement them as controll in AudioData Representation
  /*
  (unsigned)(10) retries,      // Number of tries to open device. 
    (unsigned)(500) retryDelay,  //< Delay before a retry to open device. 
    (bool)(false) allChannels,   //< Use all 4 channels, instead of only two
    (unsigned)(8000) sampleRate, //< Sample rate to capture. This variable will contain the framerate the driver finally selected. 
    (unsigned)(5000) maxFrames,  //< Maximum number of frames read in one cycle. 
    (bool)(true) onlySoundInSet, // If true, the module will not provide audio data in game states other than set 

  */
}

AudioRecorder::~AudioRecorder()
{ 
  exiting = true;

  if(audioRecorderThread.joinable()) {
    audioRecorderThread.join();
  }

  if(paSimple != NULL) {
    deinitAudio();
  }
}

void AudioRecorder::execute()
{
  // return;
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
      // deinit audio device ~8 seconds after switch off command was set
      // (recording has blocking behaviour 1024 samples equal 128 ms 63*128 equals ~8s
      if(deinitCyclesCounter >= 63)
      {
        std::cout << "[AudioRecorder] stop recording" << std::endl;
        deinitAudio();
        deinitCyclesCounter = 0;
      }
      deinitCyclesCounter++;
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
        recordingTimestamp = NaoTime::getNaoTimeInMilliSeconds();
        std::swap(writeIdx, readIdx);
      }
    } 
    else // !initialized
    { 
      usleep(128); // yield
    }

  } // end while
} // end execute


void AudioRecorder::set(const naoth::AudioControl& controlData)
{
  std::unique_lock<std::mutex> lock(dataMutex, std::try_to_lock);
  if ( lock.owns_lock() ) {
    // NOTE: changes in the recording parameters like sampleRate or numChannels 
    //       will only have an effect when capture is started. It means the parameters
    //       will not change during an ongoing recording.
    control = controlData;
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

  pa_sample_spec paSampleSpec;
  paSampleSpec.format   = PA_SAMPLE_S16LE;
  paSampleSpec.rate     = control.sampleRate;
  paSampleSpec.channels = (uint8_t)control.numChannels;

  // Create the recording stream
  int error = 0;
  if (!(paSimple = pa_simple_new(NULL, "AudioRecorder", PA_STREAM_RECORD, NULL, "AudioRecorder", &paSampleSpec, NULL, NULL, &error)))
  {
    std::cerr << "[PulseAudio] pa_simple_new() failed: " << pa_strerror(error) << "\n" << std::endl;
    // NOTE: paSimple is already NULL, this is why we are here :)
    //paSimple = NULL;
  }
  else
  {
    std::cout << "[PulseAudio] device opened" << std::endl;
    std::cout << "[PulseAudio] Rate: " << paSampleSpec.rate <<std::endl;
    std::cout << "[PulseAudio] Channels: " << (int) paSampleSpec.channels <<std::endl;
    std::cout << "[PulseAudio] Buffer Size: " << control.buffer_size <<std::endl;

    // resize the buffers if necessary
    audioBuffer[readIdx].resize(control.buffer_size*control.numChannels);
    audioBuffer[writeIdx].resize(control.buffer_size*control.numChannels);

    // TODO: do we need that?
    // give the device a bit time (moved here from the main while-loop
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