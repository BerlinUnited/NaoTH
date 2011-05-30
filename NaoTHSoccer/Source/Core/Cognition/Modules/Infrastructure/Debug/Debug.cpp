/*
 * File:   Debug.cpp
 * Author: thomas
 *
 * Created on 11. November 2010, 18:32
 */

#include "Debug.h"

#include "Tools/Debug/DebugRequest.h"
#include "Cognition/CognitionDebugServer.h"
#include <Tools/Debug/DebugImageDrawings.h>
#include <Tools/Debug/Stopwatch.h>

Debug::Debug() : cognitionLogger("log")
{
  // TODO: use the player and team number for defining the port
  CognitionDebugServer::getInstance().start(5401);
    
  DEBUG_REQUEST_REGISTER("debug:request:test", "testing the debug requests", false);
  
  
  REGISTER_DEBUG_COMMAND("image", "get the image of the robot", this);
  REGISTER_DEBUG_COMMAND("ping",  "gives you a pong", this);
  
  cognitionLogger.addRepresentation(&(getImage()), "Image");
  cognitionLogger.addRepresentation(&(getSensorJointData()), "SensorJointData");
  cognitionLogger.addRepresentation(&(getInertialSensorData()), "InertialSensorData");
  cognitionLogger.addRepresentation(&(getAccelerometerData()), "AccelerometerData");
  cognitionLogger.addRepresentation(&getGyrometerData(), "GyrometerData");
  cognitionLogger.addRepresentation(&(getFSRData()), "FSRData");
  cognitionLogger.addRepresentation(&(getFrameInfo()), "FrameInfo");
}

void Debug::execute()
{
  CognitionDebugServer::getInstance().execute(); 
  DEBUG_REQUEST("debug:request:test", std::cout << "hello debug request" << std::endl;);
}

void Debug::executeDebugCommand(const std::string& command, const std::map<std::string,std::string>& arguments, std::ostream& outstream)
{
  if (command == "image")
  {
    //g_debug("sending image timestamp=%d, frame=%d", getImage().timestamp, getFrameInfo().frameNumber);
    // add the drawings to the image
    DebugImageDrawings::getInstance().drawToImage((Image&) getImage());
    
    STOPWATCH_START("sendImage");
    Serializer<Image>::serialize(getImage(), outstream);
    STOPWATCH_STOP("sendImage");
  }
  else if(command == "ping")
  {
    outstream << "pong";
  }
}

Debug::~Debug()
{
}

