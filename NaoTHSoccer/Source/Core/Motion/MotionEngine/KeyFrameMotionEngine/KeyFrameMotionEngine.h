/**
* @file KeyFrameMotionEngine.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Declaration of class KeyFrameMotionEngine
*/

#ifndef __KeyFrameMotionEngine_h_
#define __KeyFrameMotionEngine_h_

#include <Tools/Math/Common.h>
#include <PlatformInterface/Platform.h>

#include "Tools/MotionNetParser.h"
#include "Tools/KeyFrameMotion.h"


#include "Core/Representations/Motion/Request/MotionRequest.h"
#include "Core/Motion/MotionEngine/MotionFactory.h"

#include <stdio.h>
#include <string.h>
#include <fstream>
#include <dirent.h>
#include <iostream>

using namespace std;


class KeyFrameMotionEngine : public MotionFactory//TODO: , public DebugCommandExecutor
{
public:

  KeyFrameMotionEngine();
  ~KeyFrameMotionEngine();

  virtual AbstractMotion* createMotion(const MotionRequest& motionRequest);

  /*virtual void executeDebugCommand(
    const std::string& command, const std::map<std::string,std::string>& arguments,
    std::stringstream &outstream);*/

private:
  KeyFrameMotion* currentMotion;
  std::map<string, MotionNet> motionNets;

  // loading motion-nets
  void loadMotionNetFromFile(const std::string& fileName, MotionNet& motionNet);
  bool loadMotionNet(const std::string& fileName, const std::string& motionNetName);
  void loadAvailableMotionNets(const std::string& directoryName);
};

#endif //__KeyFrameMotionEngine_h_
