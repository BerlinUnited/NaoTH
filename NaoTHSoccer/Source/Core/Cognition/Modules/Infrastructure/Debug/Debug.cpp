/*
 * File:   Debug.cpp
 * Author: thomas
 *
 * Created on 11. November 2010, 18:32
 */

#include "Debug.h"

#include "Tools/Debug/DebugRequest.h"
#include <DebugCommunication/DebugCommandManager.h>

#include <Tools/Debug/DebugImageDrawings.h>
#include <Tools/Debug/DebugDrawings3D.h>
#include <Tools/Debug/Stopwatch.h>
#include "Tools/Debug/DebugParameterList.h"
#include <Tools/Debug/Trace.h>
#include <PlatformInterface/Platform.h>

#include <Tools/SynchronizedFileWriter.h>

Debug::Debug() : cognitionLogger("CognitionLog")
{   
  DEBUG_REQUEST_REGISTER("debug:request:test", "testing the debug requests", false);
  
  
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

  registerLogableRepresentationList();

  // 3d drawings
  DEBUG_REQUEST_REGISTER("3DViewer:Robot:Body", "Show the robot body in the 3D viewer.", true);
  DEBUG_REQUEST_REGISTER("3DViewer:Robot:CoM", "Show the robot's center of mass in the 3D viewer.", true);
  DEBUG_REQUEST_REGISTER("3DViewer:Ball", "Show the ball in the 3D viewer.", true);
  DEBUG_REQUEST_REGISTER("3DViewer:Global", "Draw objects in global coordinate, i.e. the selflocator is used.", false);

  REGISTER_DEBUG_COMMAND(cognitionLogger.getCommand(), cognitionLogger.getDescription(), &cognitionLogger);

  // parameter list
  DebugParameterList::getInstance().add(&(getCameraSettingsRequest()));
}

void Debug::execute()
{

  cognitionLogger.log(getFrameInfo().getFrameNumber());

  // draw 3d only when 3d viewer is active
  if (DebugDrawings3D::getInstance().isActive())
  {
    draw3D();
  }

  DEBUG_REQUEST("debug:request:test", std::cout << "hello debug request" << std::endl;);
}

void Debug::executeDebugCommand(const std::string& command, const std::map<std::string,std::string>& arguments, std::ostream& outstream)
{
  if (command == "image")
  {
    GT_TRACE("Debug::executeDebugCommand() handling image");
    //g_debug("sending image timestamp=%d, frame=%d", getImage().timestamp, getFrameInfo().frameNumber);
    // add the drawings to the image
    GT_TRACE("Debug::executeDebugCommand() before drawToImage(...)");
    DebugImageDrawings::getInstance().drawToImage(getImage());
    
    GT_TRACE("Debug::executeDebugCommand() before serialize");
    STOPWATCH_START("sendImage");
    Serializer<Image>::serialize(getImage(), outstream);
    STOPWATCH_STOP("sendImage");
    GT_TRACE("Debug::executeDebugCommand() after serialize");
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
  else if (command == "colortable:load")
  {
    const string file = naoth::Platform::getInstance().theConfigDirectory + "/colortable.c64";

    if (g_file_test(file.c_str(), G_FILE_TEST_EXISTS))
    {
      if (getColorTable64().loadFromFile(file))
      {
        getColorClassificationModel().setColorTable(getColorTable64());
        outstream << "colortable loaded";
      }
      else
      {
        outstream << "colortable cold not be loaded";
      }
    }
    else
    {
      outstream << "colortable doesn't exist at location " << file;
    }
  }
  else if (command == "colortable:file_path")
  {
    const string file = naoth::Platform::getInstance().theConfigDirectory + "colortable.c64";
    
    if (file.length() == 0)
      outstream << "colortable file path is empty";
    else
      outstream << file;
  }
  else if (command == "motion_request:set")
  {
    if (!arguments.empty())
    {
      getMotionRequest().id = motion::getId(arguments.begin()->first);
      outstream << "Set MotionRequest to " << motion::getName(getMotionRequest().id) << endl;
    }
  }
  else if(command == "walk")
  {
    if(arguments.find("x") != arguments.end() && arguments.find("y") != arguments.end() && arguments.find("rot") != arguments.end())
    {
      getMotionRequest().id = motion::walk;
      double x;
      double y;
      double rot;
      DataConversion::strTo(arguments.find("x")->second, x);
      DataConversion::strTo(arguments.find("y")->second, y);
      DataConversion::strTo(arguments.find("rot")->second, rot);
      getMotionRequest().walkRequest.target.translation.x = x;
      getMotionRequest().walkRequest.target.translation.y = y;
      getMotionRequest().walkRequest.target.rotation = rot;
      outstream << "walk requested" << std::endl;

    }
    else
    {
      outstream << "missing param" << std::endl;
    }
  }
  else if(command == "kick")
  {
    // TODO
  }
}

Debug::~Debug()
{
  // parameter list
  DebugParameterList::getInstance().remove(&(getCameraSettingsRequest()));
}

void Debug::draw3D()
{
  Pose3D robotPose;
  DEBUG_REQUEST("3DViewer:Global",
    robotPose.translation.x = getRobotPose().translation.x;
    robotPose.translation.y = getRobotPose().translation.y;
    robotPose.rotation = RotationMatrix::getRotationZ(getRobotPose().rotation);
    );

  DEBUG_REQUEST("3DViewer:Robot:Body", drawRobot3D(robotPose););

  DEBUG_REQUEST("3DViewer:Robot:CoM",
    Vector3d p0 = robotPose * getKinematicChain().CoM;
    Vector3d p1 = p0;
    p1.z = 0;
    LINE_3D(ColorClasses::red, p0, p1);
    );

  // draw ball in 3D viewer
  Vector3d ballPos = robotPose * Vector3d(getBallModel().position.x, getBallModel().position.y, getFieldInfo().ballRadius);
  DEBUG_REQUEST("3DViewer:Ball", SPHERE(getFieldInfo().ballColor, getFieldInfo().ballRadius, ballPos););

}//end draw3D

void Debug::drawRobot3D(const Pose3D& robotPose)
{
  const Kinematics::Link* theLink = getKinematicChain().theLinks;

  for (int i = 0; i < KinematicChain::numOfLinks; i++)
  {
    if (i != KinematicChain::Neck
      && i != KinematicChain::LShoulder && i != KinematicChain::LElbow
      && i != KinematicChain::RShoulder && i != KinematicChain::RElbow
      && i != KinematicChain::Hip)
    {
      Pose3D p = robotPose * theLink[i].M;
      ENTITY(KinematicChain::getLinkName((KinematicChain::LinkID)i), p);
    }
  }//end for
}//end drawRobot3D
