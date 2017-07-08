

#ifndef _LogFileScanner_h_
#define _LogFileScanner_h_

#include <string>
#include <cassert>
#include <iostream>
#include <fstream>
#include <list>
#include <set>
#include <map>
#include <vector>

class LogFileScanner
{
public:
  struct RepresentationData
  {
    RepresentationData() : valid(false) {}
    bool valid;
    std::vector<char> data;
  };

  //
  typedef std::map<std::string, RepresentationData > Frame;
  typedef std::list<unsigned int>::const_iterator FrameIterator;

public:
  LogFileScanner(const std::string& file);
  void readFrame(unsigned int currentFrame, Frame& frame);
  void open(const std::string& filePath);

  const std::set<std::string>& getIncludedRepresentations() {
    return includedRepresentations;
  }

  FrameIterator begin() {
    return frames.begin();
  }

  FrameIterator end() {
    return frames.end();
  }

  FrameIterator last() {
    return --frames.end();
  }

private:
  std::ifstream logFile;

  typedef std::map<unsigned int, std::streampos> FramesMap;
  FramesMap frameNumber2Pos;
  std::list<unsigned int> frames;

  // list of representation names included in the logfile
  std::set<std::string> includedRepresentations;

  void scanFile();
};

#endif //_LogFileScanner_h_