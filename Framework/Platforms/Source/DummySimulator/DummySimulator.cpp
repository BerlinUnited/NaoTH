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

DummySimulator::DummySimulator(bool backendMode, bool realTime, unsigned short port)
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
    unsigned int waitTime = Math::clamp(frameExecutionTime - calculationTime, 5u, 1000u);

    ThreadUtil::sleep(waitTime);
  }
}

void DummySimulator::executeFrame()
{
  runCognition();
  runMotion();
}
