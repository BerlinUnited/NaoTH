#ifndef _RawAudioFilePlayer_H
#define _RawAudioFilePlayer_H
#include <thread>
#include <mutex>
#include <iostream>
#include <fstream>
#include "Representations/Infrastructure/AudioData.h"
#include "Representations/Infrastructure/AudioControl.h"

#include "myconio.h"

namespace naoth {
    class RawAudioFilePlayer {
        public:
            RawAudioFilePlayer();
            virtual ~RawAudioFilePlayer();

            void setParams(int numChannels, int sampleRate, int samples, int overlap);
            void open(const std::string& filePath);

            void get(naoth::AudioData& data);

            unsigned int readingTimestamp;

        protected:
            std::ifstream audioFile;

            int numChannels_;
            int sampleRate_;
            int samples_;
            int overlap_;

            bool ready;

            void readAupFile(std::ifstream& aupFile);
            size_t extractField(const std::string& line, const std::string name,
                                std::string& value, const size_t start);
    };
}

#endif // _RawAudioFilePlayer_H
