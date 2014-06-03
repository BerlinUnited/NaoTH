
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

#include "Simulator.h"
#include "Tools/NaoTime.h"

#include <Messages/Framework-Representations.pb.h>

#include "Tools/Math/Common.h"
#include "PlatformInterface/Platform.h"

using namespace std;
using namespace naoth;

Simulator::Simulator(const std::string& filePath, bool backendMode, bool realTime)
: PlatformInterface("LogSimulator", CYCLE_TIME),
  backendMode(backendMode),
  realTime(realTime),
  logFileScanner(filePath),
  lastFrameTime(0),
  simulatedTime(0),
  simulatedFrameNumber(0)
{
  // TODO: we need a better solution for it, but now it's the 
  // fastest way to provide stuff for motion
  // register basic sensor input
  registerInput<AccelerometerData>(*this);
  registerInput<SensorJointData>(*this);
  registerInput<Image>(*this);
  registerInput<ImageTop>(*this);
  registerInput<FSRData>(*this);
  registerInput<GyrometerData>(*this);
  registerInput<InertialSensorData>(*this);
  registerInput<IRReceiveData>(*this);
  registerInput<CurrentCameraSettings>(*this);
  registerInput<ButtonData>(*this);
  registerInput<BatteryData>(*this);
  registerInput<UltraSoundReceiveData>(*this);

  registerInput<FrameInfo>(*this);
  registerInput<DebugMessageIn>(*this);
  registerOutput<DebugMessageOut>(*this);
}

void Simulator::open(const std::string& filePath) 
{
  logFileScanner.open(filePath);
  lastFrameTime = 0;
  //simulatedTime = 0;
  //simulatedFrameNumber = 0;

  jumpToBegin();
}

void Simulator::init()
{  
  lastFrameTime = 0;
  simulatedTime = 0;
  theDebugServer.start(5401, true);
}

void Simulator::printRepresentations()
{
  // list all representations included in the logfile
  std::cout << "-----------------------------------------" << std::endl;
  std::cout << "Representations contained in the logfile:" << std::endl;
  for(std::set<std::string>::iterator itIncluded = logFileScanner.getIncludedRepresentations().begin();
    itIncluded != logFileScanner.getIncludedRepresentations().end(); itIncluded++)
  {
    std::cout << *itIncluded << std::endl;
  }
  std::cout << "-----------------------------------------" << std::endl;
}

void Simulator::printHelp()
{
  cout << endl;
  cout << "Welcome to the NaoTH logfile based simulator" << endl;
  cout << "--------------------------------------------" << endl << endl;

  cout << "\"WASD\"-control" << endl;
  cout << "d - one step forward" << endl;
  cout << "a - one step back" << endl;
  cout << "w - to the beginning" << endl;
  cout << "s - to the end" << endl << endl;

  cout << "g - jump to specific frame" << endl;
  cout << "p - play to end (end by pressing p again)" << endl;
  cout << "l - play loop (end by pressing l again)" << endl;
  cout << "r - repeat a frame" << endl;
  cout << "q or x - quit/exit" << endl << endl;

  cout << "After a frame was executed you will always get a line showing you the current frame and the minimal and maximal frame number" << endl;
}//end printHelp

void Simulator::printCurrentLineInfo()
{
  if(logFileScanner.begin() == logFileScanner.end()) {
    cout << "The logfile seem to be empty.\t\r";
    return;
  }
  
  LogFileScanner::FrameIterator begin = logFileScanner.begin();
  LogFileScanner::FrameIterator end = logFileScanner.last();

  // output some informations about the current frame
  if(!backendMode) {
    cout << "[" << *currentFrame << "|" << *begin << "-" << *end << "]\t\r";
  }
}//end printCurrentLineInfo

char Simulator::getInput()
{
  if (backendMode) {
    return static_cast<char>(getchar());
  } else {
    return static_cast<char>(getch());
  }
}

void Simulator::main()
{
  init();

  printRepresentations();
  printHelp();

  jumpToBegin();

  char c;
  while((c = getInput()) && c != 'q' && c != 'x')
  {
    if(c == 'd') {
      stepForward();
    } else if(c == 'a') {
      stepBack();
    } else if(c == 'w') {
      jumpToBegin();
    } else if(c == 's') {
      jumpToEnd();
    } else if(c == 'g') {
      // read jump position
      unsigned int jpos;
      cout << " goto position: ";
      cin >> jpos;
      jumpTo(jpos);
    } else if(c == 'p') {
      play(false);
    } else if(c == 'l') {
      play(true);
    } else if(c == 'r') {
      executeCurrentFrame();
    } else if(c == 'h') {
      printHelp();
      printCurrentLineInfo();
    }
  }// while

  cout << endl << "bye bye!" << endl;
}//end main

void Simulator::play(bool loop)
{
  #ifdef WIN32
  //cerr << "Play-Support now yet enabled under Windows" << endl;
  #else
  // set terminal to non-blocking...
  const int fd = fileno(stdin);
  const int fcflags = fcntl(fd,F_GETFL);
  if (fcntl(fd,F_SETFL,fcflags | O_NONBLOCK) <0)
  {
    cerr << "Could not set terminal to non-blocking mode" << endl;
    cerr << "\"Play\" capatibility not available on this terminal" << endl;
    return;
  }
  #endif //WIN32

  int c = -1;
  while(c != 'l' && c != 'p' && c != '\n' && c != 's' && c != 'q' && c !='x')
  {
    // execute the frame and calculate the time to wait
    unsigned int simulatedTimeBefore = simulatedTime;
    unsigned int startTime = NaoTime::getNaoTimeInMilliSeconds();
    stepForward();
    unsigned int calculationTime = NaoTime::getNaoTimeInMilliSeconds() - startTime;
    unsigned int maxTimeToWait = realTime?simulatedTime - simulatedTimeBefore:33;

    // wait at leas 5ms but max 1s
    unsigned int waitTime = Math::clamp((int)maxTimeToWait - (int)calculationTime, 5, 1000);


    #ifdef WIN32
    Sleep(waitTime);
    if(_kbhit())
    #else
    // wait some time
    usleep(waitTime * 1000);
    #endif
    c = getInput();

    if(!loop && currentFrame == logFileScanner.last()) {
      break;
    }
  }//while

  #ifdef WIN32
  //cerr << "Play-Support now yet enabled under Windows" << endl;
  #else
  // set back to blocking
  if (fcntl(fd,F_SETFL,fcflags) <0)
  {
    cerr << "Could not set terminal to blocking mode" << endl;
    cerr << "terminating since this is a serious error" << endl;
    exit(EXIT_FAILURE);
  }
  #endif //WIN32
}//end play

void Simulator::stepForward()
{
  currentFrame++;
  if(currentFrame == logFileScanner.end()) {
    jumpToBegin();
  } else {
    executeCurrentFrame();
  }
}

void Simulator::stepBack()
{
  if(currentFrame == logFileScanner.begin()) {
    jumpToEnd();
  } else {
    currentFrame--;
    executeCurrentFrame();
  }
}

void Simulator::jumpToBegin()
{
  currentFrame = logFileScanner.begin();
  executeCurrentFrame();
}

void Simulator::jumpToEnd()
{
  currentFrame = logFileScanner.end();
  currentFrame--;
  executeCurrentFrame();
}

void Simulator::jumpTo(unsigned int position)
{
  // do it the stupid way...
  bool wasFound = false;
  LogFileScanner::FrameIterator oldPos = currentFrame;
  currentFrame = logFileScanner.begin();
  while(!wasFound && currentFrame != logFileScanner.end())
  {
    if(*currentFrame == position) {
      wasFound = true;
      break;
    } else {
      currentFrame++;
    }
  }//end while

  if(wasFound) {
    executeCurrentFrame();
  } else {
    cout << "frame not found!" << endl;
    currentFrame = oldPos;
    if(!backendMode) { printCurrentLineInfo(); }
  }
}//end jumpTo



void Simulator::executeCurrentFrame()
{
  logFileScanner.readFrame(*currentFrame, representations);
  
  //
  adjust_frame_time();
    
  // execute
  runCognition();
  runMotion();
  
  //std::cout << "end executeCurrentFrame" << std::endl;

  printCurrentLineInfo();

}//end executeCurrentFrame


void Simulator::adjust_frame_time()
{
  naothmessages::FrameInfo f;

  // default time since the last frame
  int time_delta = 33; // default time step simulating 30fps
  
  // if no FrameInfo was logged set it manually
  LogFileScanner::RepresentationData& frameData = representations["FrameInfo"];
  if(!frameData.valid)
  {
    f.set_framenumber(*currentFrame);
    lastFrameTime = 0;
  }
  else
  {
    // read the actual frame info
    f.ParseFromArray(frameData.data.data(), (int) frameData.data.size());
    unsigned int frameTime = f.time();

    // logged time since the last frame
    if(lastFrameTime != 0) {
      time_delta = (int)frameTime - (int)lastFrameTime;
    }

    // remember the current time for next cycle
    lastFrameTime = frameTime;
  }

  if(time_delta <= 0) {
    time_delta = 33; // default time step simulating 30fps
  }

  simulatedTime += time_delta;
  f.set_time(simulatedTime);

  f.set_framenumber(simulatedFrameNumber++);
  
  // write the result back
  string result = f.SerializeAsString();
  frameData.data.resize(result.size());
  std::copy ( result.begin(), result.end(), frameData.data.begin() );
}//end adjust_frame_time


///// Getter/Setter /////

template<class T>
void Simulator::generalGet(T& data, std::string name) const
{
  LogFileScanner::Frame::const_iterator iter = representations.find(name); 
  if(iter != representations.end() && iter->second.valid)
  {
    std::istrstream stream(iter->second.data.data(), iter->second.data.size());
    Serializer<T>::deserialize(stream, data);
  }
}//end generalGet


///// end Getter/Setter /////

MessageQueue* Simulator::createMessageQueue(const std::string& /*name*/)
{
  // for single thread
  return new MessageQueue();
}
