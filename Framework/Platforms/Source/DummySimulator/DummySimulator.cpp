/*
* File:   DummySimulator.cpp
* Author: schlottb[at]informatik.hu-berlin.de
*
* Created on 2017.05.21
*/
#include "myconio.h"

#include "DummySimulator.h"

#include "Tools/ThreadUtil.h"
#include "Tools/NaoTime.h"
#include "Tools/Math/Common.h"

#include "PlatformInterface/Platform.h"

using namespace std;
using namespace naoth;

#define TEAMCOMM_MAX_MSG_SIZE 4096

DummySimulator::DummySimulator(bool backendMode, unsigned short port)
  :
  backendMode(backendMode)
{
  registerInput<FrameInfo>(*this);

  registerInput<DebugMessageInCognition>(*this);
  registerInput<DebugMessageInMotion>(*this);
  registerOutput<DebugMessageOut>(*this);

  // dummy robot
  registerOutput<MotorJointData>(*this);
  registerInput<SensorJointData>(*this);
  registerInput<FSRData>(*this);
  registerInput<AccelerometerData>(*this);

  theDebugServer.start(port);
  theDebugServer.setTimeOut(0);
}

DummySimulator::~DummySimulator()
{
  if (theGameController != nullptr)
  {
    delete theGameController;
  }

  if (theTeamCommSender != nullptr)
  {
    delete theTeamCommSender;
  }

  if (theTeamCommListener != nullptr)
  {
    delete theTeamCommListener;
  }
}

void DummySimulator::printHelp()
{
  std::cout << endl;
  std::cout << "Welcome to the NaoTH DummySimulator" << endl;
  std::cout << "--------------------------------------------" << endl << endl;

  std::cout << "p - play " << endl;
  std::cout << "r - repeat a frame" << endl;
  std::cout << "q or x - quit/exit" << endl << endl;

  std::cout << "After a frame was executed you will always get a line showing you the current frame" << endl;
}//end printHelp

char DummySimulator::getInput()
{
  if (backendMode) {
    return static_cast<char>(getchar());
  } else {
    return static_cast<char>(mygetch());
  }
}

void DummySimulator::main()
{
  printHelp();
  executeFrame();

  std::thread playThread;
  doPlay = false;

  char c;
  while ((c = getInput()))
  {
    if(doPlay) {
      // stop playing
      if (c == 'p' || c == 'q' || c == 'x') {
        doPlay = false;
        if(playThread.joinable()) {
          playThread.join();
        }
      }
    } else if (c == 'r') {
      executeFrame();
    } else if (c == 'p') {
      doPlay = true;
      playThread = std::thread(&DummySimulator::play, this);
    } else if (c == 'q' || c == 'x') {
      break;
    }
  }

  std::cout << endl << "bye bye!" << endl;
}//end main


void DummySimulator::play()
{
  while (doPlay)
  {
    unsigned int startTime = NaoTime::getNaoTimeInMilliSeconds();
    
    executeFrame();
    
    unsigned int calculationTime = NaoTime::getNaoTimeInMilliSeconds() - startTime;
    // wait at least 5ms but max 1s
    unsigned int waitTime = Math::clamp((int)frameExecutionTime - (int)calculationTime, 5, 1000);

    ThreadUtil::sleep(waitTime);
  }
}

void DummySimulator::executeFrame()
{
  runCognition();
  runMotion();
}

void DummySimulator::enableGameController()
{
  // start gamecontroller listener thread
  theGameController = new SPLGameController();
  // register gamecontroller data
  registerInput<GameData>(*this);
  registerOutput<const GameReturnData>(*this);
}

void DummySimulator::enableTeamComm(string interface)
{
    string interfaceName = "";
    int teamcomm_port = 10700;

    // load from config
    naoth::Platform::getInstance().theConfiguration.get("teamcomm", "interface", interfaceName);
    naoth::Platform::getInstance().theConfiguration.get("teamcomm", "port", teamcomm_port);

    // parameter get precedence
    if (!interface.empty()) { interfaceName.assign(interface); }

    // start sender/receiver threads
    theTeamCommSender = new BroadCaster(interfaceName, teamcomm_port);
    theTeamCommListener = new UDPReceiver(teamcomm_port, TEAMCOMM_MAX_MSG_SIZE);

    // register teamcomm
    registerInput<TeamMessageDataIn>(*this);
    registerOutput<const TeamMessageDataOut>(*this);
}
