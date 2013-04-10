/*
 * File:   Debug.cpp
 * Author: thomas
 *
 * Created on 11. November 2010, 18:32
 */

#include "Debug.h"

#include "Tools/Debug/DebugRequest.h"
#include <DebugCommunication/DebugCommandManager.h>

#include <Tools/Debug/Stopwatch.h>

#include <PlatformInterface/Platform.h>

#include <Tools/SynchronizedFileWriter.h>

using namespace std;

Debug::Debug()
{   
  DEBUG_REQUEST_REGISTER("debug:request:test", "testing the debug requests", false);
  
  
  REGISTER_DEBUG_COMMAND("image", "get the image of the robot", this);
  REGISTER_DEBUG_COMMAND("ping",  "gives you a pong", this);

  REGISTER_DEBUG_COMMAND("file::read", "read a file, usage: file::read path=<file path>", this);
  REGISTER_DEBUG_COMMAND("file::write", "read a file, usage: file::write path=<file path> [+]content=<content>", this);
  

  /*
  cognitionLogger.addRepresentation(&(getImage()), "Image");
  cognitionLogger.addRepresentation(&(getSensorJointData()), "SensorJointData");
  cognitionLogger.addRepresentation(&(getInertialSensorData()), "InertialSensorData");
  cognitionLogger.addRepresentation(&(getAccelerometerData()), "AccelerometerData");
  cognitionLogger.addRepresentation(&getGyrometerData(), "GyrometerData");
  cognitionLogger.addRepresentation(&(getFSRData()), "FSRData");
  cognitionLogger.addRepresentation(&(getFrameInfo()), "FrameInfo");
*/


/*
  REGISTER_DEBUG_COMMAND(cognitionLogger.getCommand(), cognitionLogger.getDescription(), &cognitionLogger);
*/
}

void Debug::execute()
{
/*
  cognitionLogger.log(getFrameInfo().getFrameNumber());
*/

  DEBUG_REQUEST("debug:request:test", std::cout << "hello debug request" << std::endl;);
}

void Debug::executeDebugCommand(const std::string& command, const std::map<std::string,std::string>& arguments, std::ostream& outstream)
{
  if (command == "image")
  {
    STOPWATCH_START("sendImage");
    Serializer<Image>::serialize(getImage(), outstream);
    STOPWATCH_STOP("sendImage");
  }
  else if(command == "ping")
  {
    outstream << "pong";
  }
  else if (command == "file::write")
  {
    map<string, string>::const_iterator pIter = arguments.find("path");
    if( pIter != arguments.end() )
    {
      const string& fileName = pIter->second;

      map<string, string>::const_iterator cIter = arguments.find("content");
      if (cIter != arguments.end() )
      {
        const string& str = cIter->second;
        //int tmp = str.size();
        if(SynchronizedFileWriter::saveStringToFile(str, fileName))
          outstream << fileName << " successfull written.";
        else
          outstream << fileName << " couldn't write file " << fileName;
      }
      else
      {
        outstream << fileName << " could not be written.";
      }
    }
    else
    {
      outstream << "No path given";
    }
    return;
  }
  else if (command == "file::read")
  {
    map<string, string>::const_iterator pIter = arguments.find("path");
    if( pIter != arguments.end() )
    {
      const string& fileName = pIter->second;
      FILE* file = fopen(fileName.c_str(), "rb");
      if (file) {
        char c;
        while (fread(&c, 1, 1, file)) {
          outstream << c;
        }//end while
        fclose(file);
      }
      else
      {
        outstream << fileName << " could not be read.";
      }
    }
    else
    {
      outstream << "No path given";
    }
    return;
  }
}

Debug::~Debug()
{
}

