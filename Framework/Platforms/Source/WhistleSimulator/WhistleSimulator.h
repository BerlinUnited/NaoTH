/*
* File:   WhistleSimulator.h
* Author: schlottb[at]informatik.hu-berlin.de
*
* Created on 2017.05.21
*/

#ifndef _WHISTLE_SIMULATOR_H
#define _WHISTLE_SIMULATOR_H

#include <iostream>
#include <strstream>

#include <Representations/Infrastructure/FrameInfo.h>

#include "DebugCommunication/DebugServer.h"
#include "PlatformInterface/PlatformInterface.h"
#include <Representations/Infrastructure/AudioData.h>

#include "RawAudioFilePlayer.h"

class WhistleSimulator : public naoth::PlatformInterface {
    public:
        WhistleSimulator(const std::string& filePath, bool backendMode, bool realTime, unsigned short port);
        virtual ~WhistleSimulator() {}

        virtual std::string getBodyID() const {
            return "WhistleSimulator";
        }
        virtual std::string getBodyNickName() const {
            return "naoth";
        }
        virtual std::string getHeadNickName() const {
            return "naoth";
        }
        virtual std::string getRobotName() const {
            return "WhistleSimulator";
        }
        virtual std::string getPlatformName() const {
            return "WhistleSimulator";
        }
        virtual unsigned int getBasicTimeStep() const {
            return 20;
        }

        void main();


        static const unsigned int frameExecutionTime = 33;

    public: // platform getter and setter

        void get(naoth::FrameInfo& data) const {
            data.setTime(data.getTime() + frameExecutionTime);
            data.setFrameNumber(data.getFrameNumber() + 1);

            if (!backendMode) {
                std::cout << "Frame: " << data.getFrameNumber() << "\t\r";
            }
        }

        void get(naoth::DebugMessageInCognition& data) {
            theDebugServer.getDebugMessageInCognition(data);
        }
        void get(naoth::DebugMessageInMotion& data) {
            theDebugServer.getDebugMessageInMotion(data);
        }

        void get(naoth::AudioData& data) {
            theRawAudioFilePlayer.get(data);
        }

        void set(const naoth::DebugMessageOut& data) {
            if (data.answers.size() > 0) {
                theDebugServer.setDebugMessageOut(data);
            }
        }

    protected:
        virtual MessageQueue* createMessageQueue(const std::string& /*name*/) {
            // for single thread
            return new MessageQueue();
        }

    public:
        // the flag for backend mode, which is used by LogfilePlayer of RobotControl
        bool backendMode;
        std::string myfilePath;
        void printHelp();
        char getInput();
        void executeFrame();
        void play();
        void setParams(int numChannels, int sampleRate, int samples, int overlap);


    private:
        DebugServer theDebugServer;
        naoth::FrameInfo theFrameInfo;
        naoth::RawAudioFilePlayer theRawAudioFilePlayer;
};

#endif  /* _WHISTLE_SIMULATOR_H */
