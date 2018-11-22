#include "RawAudioFilePlayer.h"

#include <Tools/ThreadUtil.h>
#include "Tools/NaoTime.h"

#include <chrono>
#include "myconio.h"

using namespace std;

namespace naoth
{
  RawAudioFilePlayer::RawAudioFilePlayer()
  :
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

	void RawAudioFilePlayer::get(AudioData& data)
	{
    if (ready)
    {
      readingTimestamp = NaoTime::getNaoTimeInMilliSeconds();
      if (readingTimestamp - data.timestamp >= static_cast<double>(samples_ - overlap_) / static_cast<double>(sampleRate_)* 1000.0)
      {
        if (audioFile.is_open())
        {
          data.sampleRate = sampleRate_;
          data.numChannels = numChannels_;
          data.samples.resize(samples_ - overlap_, 0);
          char* audioReadBufferPtr = reinterpret_cast<char*>(data.samples.data());
          audioFile.read(audioReadBufferPtr, data.samples.size() * sizeof(short));
          if (audioFile.eof())
          {
            std::cout << "[RawAudioFilePlayer] end of file reached! Press any key to exit!" << std::endl;
            char c = static_cast<char>(getch());
            exit(0);
          }
          if (!audioFile.good())
          {
            std::cerr << "[RawAudioFilePlayer] error reading file" << std::endl;
            exit(0);
          }
          data.timestamp = readingTimestamp;
        }
      }
	  }
  } // end RawAudioFilePlayer::get


}// end namespace