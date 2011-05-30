// 
// File:   DebugDrawings.cpp
// Author: thomas
//
// Created on 19. März 2008, 21:52
//

#include "DebugDrawings.h"
#include "Cognition/CognitionDebugServer.h"


DebugDrawings::DebugDrawings() : debugDrawingsOut(&bufferOne)
{
  REGISTER_DEBUG_COMMAND("debug_drawings", 
    "return the debug drawings which where collected in the internal buffer", this);
}

DebugDrawings::~DebugDrawings(){}


void DebugDrawings::executeDebugCommand(
    const std::string& command, const std::map<std::string,std::string>& /*arguments*/,
    std::ostream &outstream)
{
  if ("debug_drawings" == command) {
    std::stringstream* tmpBuffer = &bufferOne;
    if (debugDrawingsOut == &bufferOne) {
      tmpBuffer = &bufferTwo;
    }

    const std::string& buf = (*tmpBuffer).str();
    if (buf != "") {
      outstream << buf;
      //(*tmpBuffer).str("");
      //(*tmpBuffer).clear();
    }
  }
}//end executeDebugCommand


void DebugDrawings::update()
{
  if(debugDrawingsOut == &bufferOne)
  {
    // clear bufferTwo
    bufferTwo.str("");
    bufferTwo.clear();
    debugDrawingsOut = &bufferTwo;
  }else
  {
    // clear bufferOne
    bufferOne.str("");
    bufferOne.clear();
    debugDrawingsOut = &bufferOne;
  }
}//end update


std::stringstream& DebugDrawings::out()
{
  return *debugDrawingsOut;
}

ostream& operator<<(ostream& stream, const DebugDrawings::Color& color)
{
  stream << color.toString();
  return stream;
}
