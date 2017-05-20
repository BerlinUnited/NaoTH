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
  : PlatformInterface("DummySimulator", CYCLE_TIME)
{

}

void DummySimulator::init()
{
  theDebugServer.start(debugPort);
  theDebugServer.setTimeOut(0);
}

char DummySimulator::getInput()
{
  if (false) {
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

  }// while

  cout << endl << "bye bye!" << endl;
}//end main

///// Getter/Setter /////

template<class T>
void DummySimulator::generalGet(T& data, std::string name) const
{
  /*
  LogFileScanner::Frame::const_iterator iter = representations.find(name);
  if (iter != representations.end() && iter->second.valid)
  {
    std::istrstream stream(iter->second.data.data(), iter->second.data.size());
    Serializer<T>::deserialize(stream, data);
  }
  */
}//end generalGet


///// end Getter/Setter /////

MessageQueue* DummySimulator::createMessageQueue(const std::string& /*name*/)
{
  // for single thread
  return new MessageQueue();
}
