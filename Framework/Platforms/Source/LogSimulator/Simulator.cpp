
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
#include <limits.h>
#include "Simulator.h"
#include "LegacyConverter.h"
#include "Tools/NaoTime.h"
#include <Messages/Representations.pb.h>

#include "Tools/Math/Common.h"
#include "PlatformInterface/Platform.h"

using namespace std;
using namespace naoth;

Simulator::Simulator(const char* filePath, bool compatibleMode, bool backendMode)
: PlatformInterface<Simulator>("LogSimulator", CYCLE_TIME),
compatibleMode(compatibleMode),
  backendMode(backendMode)
{
  registerInput<AccelerometerData>(*this);
  registerInput<FrameInfo>(*this);
  registerInput<SensorJointData>(*this);
  registerInput<Image>(*this);
  registerInput<FSRData>(*this);
  registerInput<GyrometerData>(*this);
  registerInput<InertialSensorData>(*this);
  registerInput<BumperData>(*this);
  registerInput<IRReceiveData>(*this);
  registerInput<CurrentCameraSettings>(*this);
  registerInput<ButtonData>(*this);
  registerInput<BatteryData>(*this);
  registerInput<UltraSoundReceiveData>(*this); 
  registerInput<GameData>(*this);

  // percepts
  registerInput<CameraMatrix>(*this); 
  
  logFile.open(filePath, ios::in | ios::binary);

  if(logFile.fail())
  {
    cerr << endl << "Could not load file \"" << filePath << "\"!" << endl << endl;
    exit(-1);
  }

  parseFile();
}

void Simulator::init()
{  
  
  // print all representations included in the logfile
  std::cout << "-----------------------------------------" << std::endl;
  std::cout << "Representations contained in the logfile:" << std::endl;
  for(std::set<std::string>::iterator itIncluded = includedRepresentations.begin();
    itIncluded != includedRepresentations.end(); itIncluded++)
  {
    std::cout << *itIncluded << std::endl;
  }//end for
  std::cout << "-----------------------------------------" << std::endl;

  //noFrameInfo = includedRepresentations.find("FrameInfo") == includedRepresentations.end();
  lastFrameTime = 0;
  startFrameTime = CYCLE_TIME;

  theGameData.loadFromCfg(Platform::getInstance().theConfiguration);
  theDebugServer.start(5401, true);
}//end init

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
  cout << "q or x - quit/exit" << endl << endl;

  cout << "After a frame was executed you will always get a line showing you the current frame and the minimal and maximal frame number" << endl;
}//end printHelp

char Simulator::getInput()
{
  if (backendMode)
    return getchar();
  else
    return getch();
}

void Simulator::main()
{
  init();

  printHelp();

  jumpToBegin();

  char c;
  while((c = getInput()) && c != 'q' && c != 'x')
  {
    if(c == 'd')
    {
      stepForward();
    }
    else if(c == 'a')
    {
      stepBack();
    }
    else if(c == 'w')
    {
      jumpToBegin();
    }
    else if(c == 's')
    {
      jumpToEnd();
    }
    else if(c == 'g')
    {
      // read jump position
      unsigned int jpos;
      cout << " goto position: ";
      cin >> jpos;
      jumpTo(jpos);
    }
    else if(c == 'p')
    {
      play();
    }
    else if(c == 'l')
    {
      loop();
    }
    else if(c == 'h')
    {
      printHelp();
      printCurrentLineInfo();
    }
  }// while
  cout << endl << "bye bye!" << endl;
}//end main

void Simulator::play()
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
  bool endReached = false;
  while(!endReached && c != 'l' && c != 'p' && c != '\n' && c != 's' && c != 'q' && c !='x')
  {
    unsigned int startTime = NaoTime::getNaoTimeInMilliSeconds();
    stepForward();
    unsigned int waitTime = Math::clamp(33 - (NaoTime::getNaoTimeInMilliSeconds() - startTime),(unsigned int) 5, (unsigned int) 33);

    #ifdef WIN32
    Sleep(waitTime);
    if(_kbhit())
    #else
    // wait some time
    usleep(waitTime * 1000);
    #endif
    c = getInput();

    if(*currentFrame >= maxFrame)
    {
      endReached = true;
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

void Simulator::loop()
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
    unsigned int startTime = NaoTime::getNaoTimeInMilliSeconds();
    stepForward();
    unsigned int waitTime = Math::clamp(33 - (NaoTime::getNaoTimeInMilliSeconds() - startTime),(unsigned int) 5, (unsigned int) 33);

    #ifdef WIN32
    Sleep(waitTime);
    if(_kbhit())
    #else
    // wait some time
    usleep(waitTime * 1000);
    #endif
    c = getInput();
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
}//end loop

void Simulator::stepForward()
{
  currentFrame++;
  if(currentFrame == frames.end())
    jumpToBegin();
  else
    executeCurrentFrame();
}//end stepForward

void Simulator::stepBack()
{
  if(currentFrame == frames.begin())
    jumpToEnd();
  else
  {
    currentFrame--;
    executeCurrentFrame();
  }
}//end stepBack

void Simulator::jumpToBegin()
{
  currentFrame = frames.begin();
  executeCurrentFrame();
}//end jumpToBegin

void Simulator::jumpToEnd()
{
  currentFrame = frames.end();
  currentFrame--;
  executeCurrentFrame();
}//end jumpToEnd

void Simulator::jumpTo(unsigned int position)
{
  // do it the stupid way...
  bool wasFound = false;
  list<unsigned int>::iterator oldPos = currentFrame;
  currentFrame = frames.begin();
  while(!wasFound && currentFrame != frames.end())
  {
    if(*currentFrame == position)
    {
      wasFound = true;
      break;
    }
    else
    {
      currentFrame++;
    }
  }//end while

  if(wasFound)
  {
    executeCurrentFrame();
  }
  else
  {
    cout << "frame not found!" << endl;
    currentFrame = oldPos;
    printCurrentLineInfo();
  }
}//end jumpTo

void Simulator::executeCurrentFrame()
{  
  //std::cout << "begin executeCurrentFrame" << std::endl;
  //unsigned int i = (*currentFrame);
  std::streampos start = frameNumber2PosStart[(*currentFrame)];
  std::streampos end = frameNumber2PosEnd[*currentFrame];

  logFile.clear();
  logFile.seekg(start);
  
  representations.clear();

  while(!logFile.fail() && logFile.tellg() < end)
  {
    unsigned int tmpFrameNumber = 0;
    logFile.read((char*) &tmpFrameNumber, 4);

    string name = "";
    char c = '\0';
    logFile.read(&c, 1);
    while(c != '\0')
    {
      name += c;
      logFile.read(&c, 1);
    }

    size_t dataSize = 0;
    logFile.read((char*) &dataSize, 4);

    unsigned int posBeforeRead = logFile.tellg();

    bool found = false;

    if(compatibleMode)
    {
      found = compatibleExecute(name, dataSize);
    }
    else
    {
      // read to buffer
      stringstream s;
      char* c = new char[dataSize];
      logFile.read(c, dataSize);
      s.write(c,dataSize);
      
      representations[name] = s.str();      
      found = true; // we always read in/buffer the string data of all logged representations
      
      delete[] c;
    }//end if

    // check if something was read
    if(found)
    {
      // check how much data was read
      unsigned int posAfterRead = logFile.tellg();
      unsigned int diffAfterBefore = posAfterRead - posBeforeRead;
      if(diffAfterBefore < dataSize)
      {
        std::cerr << "ERROR: the handler for \"" << name <<
          "\" did not read enough characters, I will fix that for now but please review the code"
            << std::endl << "offset: " << (dataSize-diffAfterBefore) << std::endl;
        logFile.seekg(dataSize - diffAfterBefore, ios_base::cur);
      }
      else if(diffAfterBefore > dataSize)
      {
        std::cerr << "ERROR: the handler for \"" << name <<
          "\" read TOO MUCH BYTES: please review the code!!!"
          << std::endl << "offset: " << (diffAfterBefore - dataSize) << std::endl;

        logFile.seekg(dataSize - diffAfterBefore, ios_base::cur);

      }
    }
    else
    {
      std::cerr << "ERROR: no handler found for \"" << name <<
          "\"!" << std::endl;
        logFile.seekg(dataSize, ios_base::cur);
    }
  }
  
  adjust_frame_time();
  
  // HACK: set the frame number...
  // in fact we don't have to update if every time
  theGameData.frameNumber = *currentFrame;
    
  // execute
  callCognition();
  callMotion();
  
  //std::cout << "end executeCurrentFrame" << std::endl;

  printCurrentLineInfo();

}


void Simulator::adjust_frame_time()
{
  // HACK: adjust the timestamp: 
  // the time should contineously increase even if the logfile is played backwards (!)
  static unsigned int current_time = 0;

  naothmessages::FrameInfo f;
  unsigned int time_delta = CYCLE_TIME;
  
  // if no FrameInfo was logged set it manually
  if(noFrameInfo)
  {
    f.set_framenumber(*currentFrame);
    lastFrameTime = 0;
  }
  else
  {
    // read the actual frame info
    f.ParseFromString(representations["FrameInfo"]);
    unsigned int frameTime = f.time();

    // adjust the frame time
    if(lastFrameTime != 0)
    {
      time_delta = abs((int)frameTime - (int)lastFrameTime);
    }//end if

    // remember the last logged time
    lastFrameTime = frameTime;
  }

  if(time_delta == 0)
  {
    time_delta = CYCLE_TIME;
  }
  current_time += time_delta;
  
  f.set_time(current_time);

  // write the result back
  string result = f.SerializeAsString();
  representations["FrameInfo"] = result;
}//end adjust_frame_time


bool Simulator::compatibleExecute(const string& name, size_t dataSize)
{
  // TODO
  
//  // patch name in order to be able to use older logfiles, too
//  if (name == "image" || name == "Image")
//  {
//    theCognition.theImage->cameraInfo = Platform::getInstance().theCameraInfo;
//    LegacyConverter::oldRawImage(theCognition.theImage, logFile);
//
//    return true;
//  }
//  else if (name == "sensorJointData" || name == "SensorJointData")
//  {
//    naothmessages::SensorJointData data =
//      LegacyConverter::sensorJointDatafromStream(logFile, dataSize);
//
//    for (int i = 0; i < JointData::numOfJoint && i < data.jointdata().position_size(); i++)
//    {
//      theCognition.theSensorJointData->position[i] = data.jointdata().position(i);
//      theMotion.theSensorJointData->position[i] = data.jointdata().position(i);
//    }
//
//    for (int i = 0; i < JointData::numOfJoint && i < data.jointdata().hardness_size(); i++)
//    {
//      theCognition.theSensorJointData->hardness[i] = data.jointdata().hardness(i);
//      theMotion.theSensorJointData->hardness[i] = data.jointdata().hardness(i);
//    }
//    return true;
//  }
//  else if (name == "cameraMatrix" || name == "CameraMatrix")
//  {
//    naothmessages::CameraMatrix data =
//      LegacyConverter::cameraMatrixfromStream(logFile, "cameraMatrix" == name);
//
//    for
//    (
//      std::map<std::string, Streamable*>::iterator iter = logableCognitionRepresentations.begin();
//      iter != logableCognitionRepresentations.end();
//      ++iter
//    )
//    {
//      CameraMatrix* theCameraMatrix = dynamic_cast<CameraMatrix*> (iter->second);
//      if (theCameraMatrix != NULL)
//      {
//        theCameraMatrix->translation.x = data.pose().translation().x();
//        theCameraMatrix->translation.y = data.pose().translation().y();
//        theCameraMatrix->translation.z = data.pose().translation().z();
//
//        for (int i = 0; i < 3 && i < data.pose().rotation_size(); i++) {
//          theCameraMatrix->rotation.c[i].x = data.pose().rotation(i).x();
//          theCameraMatrix->rotation.c[i].y = data.pose().rotation(i).y();
//          theCameraMatrix->rotation.c[i].z = data.pose().rotation(i).z();
//        }
//      }
//    }
//    return true;
//  }
  return false;
}

void Simulator::printCurrentLineInfo()
{
  // output some informations about the current frame
  cout << "[" << *currentFrame << "|" << minFrame << "-" << maxFrame << "]\t\r";
}//end printCurrentLineInfo

void Simulator::parseFile()
{
  noFrameInfo = false;

  frames.clear();
  includedRepresentations.clear();

  frameNumber2PosStart.clear();
  frameNumber2PosEnd.clear();

  minFrame = UINT_MAX;
  maxFrame = 0;

  // reset file cursor
  logFile.seekg(0, ios_base::beg);

  unsigned int lastFrameNumber = 0;
  bool firstFrame = true;

  std::set<std::string> perFrameIncluded;

  while(!logFile.fail() && !logFile.eof())
  {
    streampos currentPos = logFile.tellg();
    unsigned int currentFrameNumber = 0;
    logFile.read((char*) &currentFrameNumber, 4);

    string currentName = "";
    char c = '\0';
    logFile.read(&c, 1);
    while(c != '\0')
    {
      currentName += c;
      logFile.read(&c, 1);
    }

    includedRepresentations.insert(currentName);
    perFrameIncluded.insert(currentName);

    size_t currentSize = 0;
    logFile.read((char*) &currentSize, 4);

    if(currentSize == 0 && currentName.size() == 0)
    {
      cerr << endl << "Illegal end of file."<< endl << endl;
      break;
    }//end if

    // skip data
    logFile.seekg(currentSize, ios_base::cur);


    if(firstFrame || lastFrameNumber < currentFrameNumber)
    {
      if(firstFrame)
      {
        firstFrame = false;
      }
      else
      {
        // set end of last frame
        frameNumber2PosEnd[lastFrameNumber] = currentPos;
      }

      minFrame = min(minFrame, currentFrameNumber);
      maxFrame = max(maxFrame, currentFrameNumber);

      // add new frame
      frames.push_back(currentFrameNumber);
      frameNumber2PosStart[currentFrameNumber] = currentPos;

      // needed to generate the the frame info if not available in one single frame
      if(perFrameIncluded.find("FrameInfo") == perFrameIncluded.end())
      {
        noFrameInfo = true;
        std::cout << "automated FrameInfo generation" << std::endl;
      }

      perFrameIncluded.clear();

      lastFrameNumber = currentFrameNumber;
    }//end if
  }//end while
}//end parseFile


///// Getter/Setter /////

template<class T>
void Simulator::generalGet(T& data, std::string name) const
{
  std::map<std::string, std::string>::const_iterator iter = representations.find(name); 
  if(iter != representations.end())
  {
  //std::cout << "getting " << name << std::endl;
    std::stringstream stream(iter->second);
    Serializer<T>::deserialize(stream, data);
  }//end if
}//end generalGet


///// end Getter/Setter /////

Simulator::~Simulator()
{
}


MessageQueue* Simulator::createMessageQueue(const std::string& name)

{
  // for single thread
  return new MessageQueue();
}
