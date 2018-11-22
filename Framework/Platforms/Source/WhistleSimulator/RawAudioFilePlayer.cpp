#include "RawAudioFilePlayer.h"

#include <Tools/ThreadUtil.h>
#include "Tools/NaoTime.h"

#include <chrono>

using namespace std;

namespace naoth
{
  RawAudioFilePlayer::RawAudioFilePlayer()
  :
    recordingTimestamp(0),
    readingTimestamp(0),
    numChannels_(0),
    sampleRate_(0),
    samples_(1024),
    overlap_(0),
    ready(false)
  {
	}

	RawAudioFilePlayer::~RawAudioFilePlayer()
  { 
    if (audioFile.is_open())
    {
      audioFile.close();
    }
	}

  void RawAudioFilePlayer::setParams(int numChannels, int sampleRate, int samples, int overlap)
  {
    if (numChannels > 0) numChannels_ = numChannels;
    if (sampleRate > 0) sampleRate_ = sampleRate;
    if (samples > 0) samples_ = samples;
    if (overlap > 0) overlap_ = overlap;
    audioReadBuffer.resize(samples_ - overlap_, 0);
  }

  void RawAudioFilePlayer::open(const std::string& filePath)
  {
    bool audioFileValid = false;
    ready = false;

    audioFile.open(filePath.c_str(), std::ios::in | std::ios::binary);

    if (audioFile.fail())
    {
      std::cerr << "[RawAudioFilePlayer] Could not open the file \"" << filePath << "\"!" << std::endl;
      assert(false);
    }

    if (size_t pos = filePath.rfind(".raw") != string::npos)
    {
      std::ifstream aupFile;
      std::string aupFilePath = filePath;
      aupFilePath.replace(filePath.length() - 4, 4, ".aup");
      //std::cout << "pos " << pos << " " << filePath.length() - 4 << " file" << aupFilePath << std::endl;
      aupFile.open(aupFilePath.c_str(), std::ios::in);
      if (aupFile.fail())
      {
        if (sampleRate_ != 0 && numChannels_ != 0)
        {
          std::cerr << "[RawAudioFilePlayer] Audacity Description file is missing! \"" << filePath << "\"!" << std::endl;
          std::cout << "[RawAudioFilePlayer] Using command line parameters for rate and channel!" << std::endl;
        }
        else
        {
          std::cerr << "[RawAudioFilePlayer] Could not open the raw audio file \"" << filePath << "\"!" << std::endl;
          std::cerr << "[RawAudioFilePlayer] Could not determine audio file properties !" << std::endl;
          std::cerr << "[RawAudioFilePlayer] Audacity Description file is missing !" << std::endl;
          std::cerr << "[RawAudioFilePlayer] Provide parameters in command line (--rate and --channels) !" << std::endl;
          assert(false);
        }
      }
      readAupFile(aupFile);
      aupFile.close();
    }

    if (filePath.rfind(".wav") != string::npos)
    {
      std::cout << "[RawAudioFilePlayer] wav file support not yet available!" << std::endl;
      exit(0);
    }

    audioFileValid = numChannels_ != 0 && sampleRate_ != 0;

    if (!audioFileValid)
    {
      std::cerr << "[RawAudioFilePlayer] Could not open the raw audio file \"" << filePath << "\"!" << std::endl;
      std::cerr << "[RawAudioFilePlayer] Could not determine audio file properties !" << std::endl;
      std::cerr << "[RawAudioFilePlayer] Provide parameters in command line (--rate and --channels) !" << std::endl;
      assert(false);
    }

    ready = true;
  }

  void RawAudioFilePlayer::readAupFile(std::ifstream& aupFile)
  {
    string line;
    bool paramsFound = false;

    std::vector<std::string> tracks;
    while (getline(aupFile, line))
    {
      if (line.find("<wavetrack") != string::npos)
      {
        paramsFound = true;
        tracks.emplace_back(line);
      }
    }

    if (tracks.size() > 0)
    {
      numChannels_ = tracks.size();
      int lastSampleRate;
      for (size_t i = 0; i < tracks.size(); i++)
      {
        std::stringstream stLine(tracks[i]);
        std::string field;
        while (!stLine.eof())
        {
          if (stLine >> field)
          {
            if (field.find("rate") != string::npos)
            {
              std::string r = field.substr(6, field.length() - 3);
              sampleRate_ = (unsigned short)strtol(r.c_str(), 0, 10);
              if (i > 0 && lastSampleRate != sampleRate_)
              {
                std::cerr << "[RawAudioFilePlayer] Tracks habe different rate settings !" << std::endl;
                assert(false);
              }
              lastSampleRate = sampleRate_;
            }
          }
        }
      }
    }
    std::cout << "[RawAudioFilePlayer] audio channels " << numChannels_ << " sample rate " << sampleRate_ << std::endl;
  }

	void RawAudioFilePlayer::execute()
	{
	  //running = true;

	  //// return;
	  //while(running)
	  //{
	  //  int error = 0;

	  //  if(!recording)
	  //  {
	  //    if(!resetting && command == 1)
	  //    {
	  //      std::cout << "[RawAudioFilePlayer] start recording" << std::endl;
	  //      initAudio();
	  //    }
	  //    else if(resetting)
	  //    {
	  //      usleep(128);
	  //    }
	  //  }
	  //  else
	  //  {
	  //    if(command == 0)
	  //    {
	  //      // deinit audio device ~8 seconds after switch off command was set
	  //      // (recording has blocking behaviour 1024 samples equal 128 ms 63*128 equals ~8s
	  //      if(deinitCyclesCounter >= 63)
	  //      {
	  //        std::cout << "stop recording" << std::endl;
	  //        deinitAudio();
	  //        deinitCyclesCounter = 0;
	  //        usleep(128);
	  //        continue;
	  //      }
	  //      deinitCyclesCounter++;
	  //    }
	  //  }

	  //  if(!resetting && recording)
	  //  {
	  //    //calculate amount of samples needed
	  //    int samplesToRead = SAMPLE_NEW_COUNT * numChannels;
	  //    int bytesToRead = samplesToRead * static_cast<int>(sizeof(short));

	  //    // Record some data
	  //    if (!paSimple || paSimple == NULL)
	  //    {
	  //      std::cerr << "[PulseAudio] pa_simple == NULL" << std::endl;
	  //      running = false;
	  //    }
	  //    //std::cout << "[RawAudioFilePlayer] bytesToRead: " << bytesToRead << std::endl;
	  //    if (pa_simple_read(paSimple, &audioReadBuffer[0], bytesToRead, &error) < 0)
	  //    {
	  //      std::cerr << "[PulseAudio] pa_simple_read() failed: " << pa_strerror(error) << std::endl;
	  //      running = false;
	  //    } else {
   //       recordingTimestamp = NaoTime::getNaoTimeInMilliSeconds();
   //     }
	  //  }
	  //  else
	  //  {
	  //    usleep(128);
	  //    continue;
	  //  }
	  //} // end while
	} // end execute


	void RawAudioFilePlayer::set(const naoth::AudioControl& controlData)
	{
	  
	  //std::unique_lock<std::mutex> lock(setMutex, std::try_to_lock);
	  //if ( lock.owns_lock() )
	  {
	  //	if(command != controlData.onOffSwitch){
	  //		command = controlData.onOffSwitch;
	  //		std::cout << "Command: " << command << std::endl;
	  //	}	    
	  //  
	  //  if(activeChannels != controlData.activeChannels)
	  //  {
	  //    resetting =  true;
	  //    if(recording)
	  //    {
	  //      deinitAudio();
	  //    }
	  //    //clearBuffers(); //TODO are there any buffers to clear?
	  //    activeChannels = controlData.activeChannels;
	  //    numChannels = controlData.numChannels;
	  //    sampleRate = controlData.sampleRate;
	  //    buffer_size = controlData.buffer_size;
	  //    resetting = false;
	  //  }
	  }
	}

	void RawAudioFilePlayer::get(AudioData& data)
	{
    if (ready)
    {
      readingTimestamp = NaoTime::getNaoTimeInMilliSeconds();
      if (readingTimestamp - recordingTimestamp >= static_cast<double>(audioReadBuffer.size()) / static_cast<double>(sampleRate_) * 1000.0)
      {
        if (audioFile.is_open())
        {
          audioReadBuffer.resize(audioReadBuffer.size(), 0);
          char* audioReadBufferPtr = reinterpret_cast<char*>(audioReadBuffer.data());
          //std::cout << "reading file " << (readingTimestamp - recordingTimestamp) << " >= " << (static_cast<double>(audioReadBuffer.size()) / static_cast<double>(sampleRate_)* 1000.0) << std::endl;
          audioFile.read(audioReadBufferPtr, audioReadBuffer.size() * sizeof(short));
          if (audioFile.eof())
          {
            std::cout << "[RawAudioFilePlayer] end of file reached" << std::endl;
            exit(0);
          }
          if (!audioFile.good())
          {
            std::cerr << "[RawAudioFilePlayer] error reading file" << std::endl;
            exit(0);
          }
          recordingTimestamp = readingTimestamp;
          data.sampleRate = sampleRate_;
          data.numChannels = numChannels_;
          data.samples = audioReadBuffer;
          data.timestamp = recordingTimestamp;
        }
      }
	  }
  } // end RawAudioFilePlayer::get


	void RawAudioFilePlayer::initAudio()
	{
	//  //clearBuffers(); probably not needed anymore

	//  int error;

	//  pa_sample_spec paSampleSpec;
	//  paSampleSpec.format = PA_SAMPLE_S16LE;
	//  paSampleSpec.rate = sampleRate;
	//  paSampleSpec.channels = (uint8_t)numChannels;

	//  // Create the recording stream
	//  std::string appName = "RawAudioFilePlayer";
	//  if (!(paSimple = pa_simple_new(NULL, appName.c_str(), PA_STREAM_RECORD, NULL, "RawAudioFilePlayer", &paSampleSpec, NULL, NULL, &error)))
	//  {
	//    std::cerr << "[PulseAudio] pa_simple_new() failed: " << pa_strerror(error) << "\n" << std::endl;
	//    paSimple = NULL;
	//  }
	//  else
	//  {
	//    std::cout << "[PulseAudio] device opened" << std::endl;
	//    std::cout << "[PulseAudio] Rate: " << paSampleSpec.rate <<std::endl;
	//    std::cout << "[PulseAudio] Channels: " << (int) paSampleSpec.channels <<std::endl;
	//    std::cout << "[PulseAudio] Buffer Size: " << buffer_size <<std::endl;

	//    recording = true;
	//  }
	//} //end initAudio

	//void RawAudioFilePlayer::deinitAudio()
	//{
 //   if (paSimple != NULL)
 //   {
 //     recording = false;
 //     std::cout << "[PulseAudio] device closed" << std::endl;
 //     pa_simple_free(paSimple);
 //     paSimple = NULL;
 //   }
	} //end deinitAudio

}// end namespace