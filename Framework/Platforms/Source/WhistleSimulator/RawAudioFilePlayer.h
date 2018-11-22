#ifndef _RawAudioFilePlayer_H
#define _RawAudioFilePlayer_H
#include <thread>
#include <mutex>
#include <iostream>
#include <fstream>
#include "Representations/Infrastructure/AudioData.h"
#include "Representations/Infrastructure/AudioControl.h"

namespace naoth
{
	class RawAudioFilePlayer
	{
		public:
	  	RawAudioFilePlayer();
	  	virtual ~RawAudioFilePlayer();

      void setParams(int numChannels, int sampleRate, int samples, int overlap);
      void open(const std::string& filePath);
      void readAupFile(std::ifstream& aupFile);

	  	void get(naoth::AudioData& data);
      void set(const naoth::AudioControl& controlData);

	  	std::vector<short> audioReadBuffer;
      unsigned long recordingTimestamp;
      unsigned long readingTimestamp;

	  protected:
  		//std::thread RawAudioFilePlayerThread;
      //std::mutex getMutex;
      //std::mutex setMutex;
    //  int command;
      std::ifstream audioFile;

      //Audio Control Parameter:
  		//std::string activeChannels;
      int numChannels_;
      int sampleRate_;
      int samples_;
      int overlap_;
      int buffer_size;

      //volatile bool running;
      bool ready;
      //bool recording;
      //bool resetting;
      //int deinitCyclesCounter;

  		void execute();  		
  		void initAudio();
  		void deinitAudio();

	};
}

#endif // _RawAudioFilePlayer_H