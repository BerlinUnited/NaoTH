/*
 * File:   Debug.cpp
 * Author: thomas
 *
 * Created on 11. November 2010, 18:32
 */


#include "Debug.h"

#include <PlatformInterface/Platform.h>
#include <DebugCommunication/DebugCommandManager.h>

// debug
#include <Tools/Debug/DebugRequest.h>
#include <Tools/Debug/DebugDrawings3D.h>

#include <Tools/SynchronizedFileWriter.h>


Debug::Debug() : cognitionLogger("CognitionLog")
{
  REGISTER_DEBUG_COMMAND("image", "get the image of the robot", this);
  REGISTER_DEBUG_COMMAND("ping",  "gives you a pong", this);

  REGISTER_DEBUG_COMMAND("file::read", "read a file, usage: file::read path=<file path>", this);
  REGISTER_DEBUG_COMMAND("file::write", "read a file, usage: file::write path=<file path> [+]content=<content>", this);
  
  REGISTER_DEBUG_COMMAND("colortable:load", "set the current color table", this);
  REGISTER_DEBUG_COMMAND("colortable:file_path",
    "return the path of the currentelly loaded colortable (needed by ColorTableTool", this);


  REGISTER_DEBUG_COMMAND("motion_request:set",
    "force a motion request (usage MotionRequest:set <name> )", this);
  REGISTER_DEBUG_COMMAND("walk", "let the robot walk", this);
  REGISTER_DEBUG_COMMAND("kick", "let the robot kick", this);

  REGISTER_DEBUG_COMMAND("kill_cognition", "kill cognition", this);

  registerLogableRepresentationList();

  // 3d drawings
  DEBUG_REQUEST_REGISTER("3DViewer:Robot:Body", "Show the robot body in the 3D viewer.", true);
  DEBUG_REQUEST_REGISTER("3DViewer:Robot:Camera", "Show the robot body in the 3D viewer.", true);
  DEBUG_REQUEST_REGISTER("3DViewer:Robot:CoM", "Show the robot's center of mass in the 3D viewer.", true);
  DEBUG_REQUEST_REGISTER("3DViewer:Ball", "Show the ball in the 3D viewer.", true);
  DEBUG_REQUEST_REGISTER("3DViewer:Global", "Draw objects in global coordinate, i.e. the selflocator is used.", false);

  DEBUG_REQUEST_REGISTER("3DViewer:kinematic_chain:links", "Visualize the kinematic chain in the 3D viewer", false);
  DEBUG_REQUEST_REGISTER("3DViewer:kinematic_chain:mass", "Visualize the kinematic chain in the 3D viewer", false);
  DEBUG_REQUEST_REGISTER("3DViewer:kinematic_chain:com", "Visualize the kinematic chain in the 3D viewer", false);

  REGISTER_DEBUG_COMMAND(cognitionLogger.getCommand(), cognitionLogger.getDescription(), &cognitionLogger);

  // HACK: initialize the both canvases
  getDebugImageDrawings().init(getImage().width(), getImage().height());
  getDebugImageDrawingsTop().init(getImageTop().width(), getImageTop().height());
}

Debug::~Debug()
{
}

void Debug::execute()
{

  cognitionLogger.log(getFrameInfo().getFrameNumber());

  // draw 3d only when 3d viewer is active
  if (DebugDrawings3D::getInstance().isActive())
  {
    //draw3D();
  }

  FIELD_DRAWING_CONTEXT;
  PEN("FF0000", 50);
  CIRCLE(1000,500,300);

  getDebugImageDrawings().drawCircleToImage(ColorClasses::red, 320, 240, 30);
  getDebugImageDrawingsTop().drawCircleToImage(ColorClasses::blue, 320, 240, 30);
}

void Debug::executeDebugCommand(const std::string& command, const std::map<std::string,std::string>& arguments, std::ostream& outstream)
{
  if (command == "image")
  {
    GT_TRACE("Debug::executeDebugCommand() handling image");
    // add the drawings to the image
    GT_TRACE("Debug::executeDebugCommand() before drawToImage(...)");
    
    if(arguments.find("top") != arguments.end())
    {
      ImageDrawingCanvas canvas(getImageTop());
      getDebugImageDrawingsTop().drawToImage(canvas);
      GT_TRACE("Debug::executeDebugCommand() before serialize");
      STOPWATCH_START("sendImageTop");
      Serializer<Image>::serialize(getImageTop(), outstream);
      STOPWATCH_STOP("sendImageTop");
      GT_TRACE("Debug::executeDebugCommand() after serialize");
    }
    else
    {
      ImageDrawingCanvas canvas(getImage());
      getDebugImageDrawings().drawToImage(canvas);
      GT_TRACE("Debug::executeDebugCommand() before serialize");
      STOPWATCH_START("sendImage");
      Serializer<Image>::serialize(getImage(), outstream);
      STOPWATCH_STOP("sendImage");
      GT_TRACE("Debug::executeDebugCommand() after serialize");
    }

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
        }
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
  else if(command == "kill_cognition")
  {
    outstream << "will go into endless loop" << std::endl;
    while(true)
    {
      std::cout << "cognition in endless loop due to \"kill_cognition\" debug command"
                 << std::endl;
      g_usleep(500000);

    }
  }
}

