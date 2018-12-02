#include "AudioRecorder.h"

#include <Tools/ThreadUtil.h>
#include "Tools/NaoTime.h"

#include <chrono>
#include <pulse/error.h>

using namespace std;
using namespace naoth;


AudioRecorder::AudioRecorder()
  :
  running(false),

  numChannels(NUM_CHANNELS_RX),
  sampleRate(SAMPLE_RATE_RX),
  buffer_size(BUFFER_SIZE_RX),

  capture(false), 

  resetting(false),
  deinitCyclesCounter(0),

  initialized(false),
  paSimple(NULL),

  audioReadBuffer(BUFFER_SIZE_RX, 0),
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
  running = false;

  if(audioRecorderThread.joinable()) {
    audioRecorderThread.join();
  }

  if(initialized) {
    deinitAudio();
  }
}

void AudioRecorder::execute()
{
  running = true;

  // return;
  while(running)
  {

    // initialize the audio stream if necessary
    if(capture && !initialized)
    {
      std::cout << "[AudioRecorder] start recording" << std::endl;
      initAudio();
    }
    
    // stop capturing
    if(!capture && initialized) 
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
    if(initialized) 
    {
      //calculate amount of samples needed
      //int samplesToRead = SAMPLE_COUNT * numChannels;
      //int bytesToRead = samplesToRead * static_cast<int>(sizeof(short));
      int bytesToRead = audioReadBuffer.size() * static_cast<int>(sizeof(short));

      // Record some data
      if (!paSimple || paSimple == NULL)
      {
        std::cerr << "[PulseAudio] pa_simple == NULL" << std::endl;
        running = false;
      }
      //std::cout << "[AudioRecorder] bytesToRead: " << bytesToRead << std::endl;

      int error = 0;
      if (pa_simple_read(paSimple, &audioReadBuffer[0], bytesToRead, &error) < 0)
      {
        std::cerr << "[PulseAudio] pa_simple_read() failed: " << pa_strerror(error) << std::endl;
        running = false;
      } else {
        recordingTimestamp = NaoTime::getNaoTimeInMilliSeconds();
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
  std::unique_lock<std::mutex> lock(setMutex, std::try_to_lock);
  if ( lock.owns_lock() )
  {
    if(capture != controlData.capture) {
      capture = controlData.capture;
      std::cout << "Capture: " << capture << std::endl;
    }      
      
    if(activeChannels != controlData.activeChannels)
    {
      resetting =  true;
      if(initialized) {
        deinitAudio();
      }

      //clearBuffers(); //TODO are there any buffers to clear?
      activeChannels = controlData.activeChannels;
      numChannels = controlData.numChannels;
      sampleRate = controlData.sampleRate;
      buffer_size = controlData.buffer_size;
      resetting = false;
    }
  }
}

void AudioRecorder::get(AudioData& data)
{
  if (initialized) 
  {
    std::unique_lock<std::mutex> lock(getMutex, std::try_to_lock);
    if ( lock.owns_lock() && recordingTimestamp > data.timestamp) 
    {
      data.sampleRate = sampleRate;
      data.numChannels = numChannels;
      data.samples = audioReadBuffer;
      data.timestamp = recordingTimestamp;
    }
  }
} // end AudioRecorder::get


void AudioRecorder::initAudio()
{
  //clearBuffers(); probably not needed anymore

  pa_sample_spec paSampleSpec;
  paSampleSpec.format   = PA_SAMPLE_S16LE;
  paSampleSpec.rate     = sampleRate;
  paSampleSpec.channels = (uint8_t)numChannels;

  // Create the recording stream
  int error = 0;
  if (!(paSimple = pa_simple_new(NULL, "AudioRecorder", PA_STREAM_RECORD, NULL, "AudioRecorder", &paSampleSpec, NULL, NULL, &error)))
  {
    std::cerr << "[PulseAudio] pa_simple_new() failed: " << pa_strerror(error) << "\n" << std::endl;
    // NOTE: paSimple is already NULL, this is why we are here :)
    paSimple = NULL;
  }
  else
  {
    std::cout << "[PulseAudio] device opened" << std::endl;
    std::cout << "[PulseAudio] Rate: " << paSampleSpec.rate <<std::endl;
    std::cout << "[PulseAudio] Channels: " << (int) paSampleSpec.channels <<std::endl;
    std::cout << "[PulseAudio] Buffer Size: " << buffer_size <<std::endl;

    initialized = true;

    // TODO: do we need that?
    // give the device a bit time (moved here from the main while-loop
    usleep(128);
  }
} //end initAudio

void AudioRecorder::deinitAudio()
{
  if (paSimple != NULL)
  {
    initialized = false;
    std::cout << "[PulseAudio] device closed" << std::endl;
    pa_simple_free(paSimple);
    paSimple = NULL;
    
    // TODO: do we need that?
    // give the device a bit time (moved here from the main while-loop
    usleep(128);
  }
} //end deinitAudio