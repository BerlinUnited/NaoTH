//
// structure of the frame: 
// -----------------------
// | f | name | n | data |
// -----------------------
// f    - frame number (unsigned int, 4 byte)
// name - name of the representation (string terminated with '\0')
// n    - size of the representation data (unsigned int, 4 byte)
// data - serialzed representation (n bytes)
//


#include "LogFileScanner.h"
#include <sstream>

LogFileScanner::LogFileScanner(const std::string& filePath)
{
  logFile.open(filePath.c_str(), std::ios::in | std::ios::binary);

  if(logFile.fail())
  {
    std::cerr << "[LogFileScanner] Could not open the file \"" << filePath << "\"!" << std::endl;
    assert(false);
  }

  scanFile();
}

void LogFileScanner::open(const std::string& filePath)
{
  logFile.close();
  logFile.open(filePath.c_str(), std::ios::in | std::ios::binary);

  if(logFile.fail())
  {
    std::cerr << "[LogFileScanner] Could not open the file \"" << filePath << "\"!" << std::endl;
    assert(false);
  }

  scanFile();
}

void LogFileScanner::scanFile()
{
  frames.clear();
  includedRepresentations.clear();
  frameNumber2Pos.clear();

  // reset file cursor
  logFile.seekg(0, std::ios_base::beg);

  int lastFrameNumber = -1;

  while(!logFile.fail() && !logFile.eof())
  {
    std::streampos currentPos = logFile.tellg();
    
    // read the frame number
    unsigned int currentFrameNumber = 0;
    logFile.read((char*) &currentFrameNumber, 4);

    // NOTE: this is the regular end of the loop, because the eof bit in logFile is set after
    //       we tried to read after the end of file
    //       currentPos contains the last valid position
    if(logFile.eof()) {
      // add a virtual frame number to mark the end position of the last frame
      frameNumber2Pos[lastFrameNumber+1] = currentPos;
    }

    if((int)currentFrameNumber < lastFrameNumber && lastFrameNumber != -1) {
      std::cerr << "[LogFileScanner] Frame " << currentFrameNumber << ": "
                << "The frame number is smaler that the previous " << lastFrameNumber << std::endl;
      break;
    }

    // read the representation name
    std::string currentName;
    char c = '\0';
    logFile.read(&c, 1);
    while(c != '\0') {
      currentName += c;
      logFile.read(&c, 1);
    }

    if(currentName.size() == 0) {
      std::cerr << "[LogFileScanner] Frame " << currentFrameNumber << ": "
                << "Empty representation name." << std::endl;
      break;
    }

    includedRepresentations.insert(currentName);

    // read the size of the data
    size_t currentSize = 0;
    logFile.read((char*) &currentSize, 4);

    if(currentName.size() == 0) {
      std::cerr << "[LogFileScanner] Frame " << currentFrameNumber << ": "
                << "Empty data for representation " << currentName << std::endl;
      break;
    }
    
    // skip data
    logFile.seekg(currentSize, std::ios_base::cur);

    // save the frame markers
    if((unsigned int) lastFrameNumber != currentFrameNumber)
    {
      frameNumber2Pos[currentFrameNumber] = currentPos;
      frames.push_back(currentFrameNumber);

      lastFrameNumber = currentFrameNumber;
    }
  }//end while

}//end scanFile


void LogFileScanner::readFrame(unsigned int currentFrame, Frame& frame)
{
  FramesMap::const_iterator framePos = frameNumber2Pos.find(currentFrame);
  // no such frame
  if(framePos == frameNumber2Pos.end()) {
    return;
  }

  std::streampos start = framePos->second;
  std::streampos end = (++framePos)->second;

  logFile.clear();
  logFile.seekg(start);
  
  for(LogFileScanner::Frame::iterator i = frame.begin(); i != frame.end(); i++) {
    (*i).second.valid = false;
  }

  while(!logFile.fail() && !logFile.eof() && logFile.tellg() < end)
  {
    unsigned int tmpFrameNumber = 0;
    logFile.read((char*) &tmpFrameNumber, 4);

    // read the name of the representation
    std::string name;
    char c = '\0';
    logFile.read(&c, 1);
    while(c != '\0') {
      name += c;
      logFile.read(&c, 1);
    }

    // read the size of the data
    unsigned int dataSize = 0;
    logFile.read((char*) &dataSize, 4);

    // read the representation data to the current frame buffer
    RepresentationData& data = frame[name];
    data.data.resize(dataSize);
    logFile.read(data.data.data(), dataSize);
    data.valid = true;
  }
}//end executeCurrentFrame
