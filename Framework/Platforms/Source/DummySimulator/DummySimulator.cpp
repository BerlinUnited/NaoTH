/*
* File:   DummySimulator.cpp
* Author: schlottb[at]informatik.hu-berlin.de
*
* Created on 2017.05.21
*/
#ifdef  WIN32
#include <conio.h>
#include <windows.h>
#include <winbase.h>
#else
#include <unistd.h>
#include <fcntl.h>
#include "myconio.h"
#endif //WIN32

#include <sstream>

#include "DummySimulator.h"
#include "Tools/NaoTime.h"

#include <Messages/Framework-Representations.pb.h>

#include "Tools/Math/Common.h"
#include "PlatformInterface/Platform.h"

using namespace std;
using namespace naoth;

DummySimulator::DummySimulator(bool backendMode, bool realTime, unsigned short port)
  : PlatformInterface("DummySimulator", CYCLE_TIME),
  backendMode(backendMode),
  debugPort(port),
  dummyModule(),
  simulatedTime(0),
  lastFrameTime(0),
  simulatedFrameNumber(0)
{

}

void DummySimulator::init()
{
  lastFrameTime = 0;
  simulatedTime = 0;
  theDebugServer.start(debugPort);
  theDebugServer.setTimeOut(0);

  currentFrame = dummyModule.begin();
}

char DummySimulator::getInput()
{
  if (backendMode) {
    return static_cast<char>(getchar());
  }
  else {
    return static_cast<char>(getch());
  }
}

void DummySimulator::main()
{
  init();

  char c;
  while ((c = getInput()) && c != 'q' && c != 'x')
  {
    // execute    
    executeCurrentFrame();
  }

  cout << endl << "bye bye!" << endl;
}//end main

void DummySimulator::executeCurrentFrame()
{
  //TODO provide currentFrame correct
  dummyModule.readFrame(*currentFrame, representations);
  currentFrame++;
  adjust_frame_time();

  // execute
  runCognition();
  //runMotion();
  Sleep(500);

  //printCurrentLineInfo();

}//end executeCurrentFrame


void DummySimulator::adjust_frame_time()
{
  naothmessages::FrameInfo f;

  // default time since the last frame
  int time_delta = 33; // default time step simulating 30fps

  // if no FrameInfo was logged set it manually
  DummyModule::RepresentationData& frameData = representations["FrameInfo"];
  if (!frameData.valid)
  {
    f.set_framenumber(*currentFrame);
    lastFrameTime = 0;
  }
  else
  {
    // read the actual frame info
    f.ParseFromArray(frameData.data.data(), (int)frameData.data.size());
    unsigned int frameTime = f.time();

    // logged time since the last frame
    if (lastFrameTime != 0) {
      time_delta = (int)frameTime - (int)lastFrameTime;
    }

    // remember the current time for next cycle
    lastFrameTime = frameTime;
  }

  if (time_delta <= 0) {
    time_delta = 33; // default time step simulating 30fps
  }

  simulatedTime += time_delta;
  f.set_time(simulatedTime);

  f.set_framenumber(simulatedFrameNumber++);

  // write the result back
  string result = f.SerializeAsString();
  frameData.data.resize(result.size());
  std::copy(result.begin(), result.end(), frameData.data.begin());
}//end adjust_frame_time


MessageQueue* DummySimulator::createMessageQueue(const std::string& /*name*/)
{
  // for single thread
  return new MessageQueue();
}
