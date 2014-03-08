/**
* @file KeyFrameMotionFactory.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Implementation of class KeyFrameMotionFactory
*/

#include "KeyFrameMotionEngine.h"

#include <Tools/Math/Common.h>
#include <PlatformInterface/Platform.h>

#include "Tools/MotionNetParser.h"

#include <cstdio>
#include <fstream>
//#include <dirent.h>
#include <iostream>


using namespace naoth;

KeyFrameMotionFactory::KeyFrameMotionFactory()
{
  /*REGISTER_DEBUG_COMMAND("reload:motion_net", 
    "reloads a particular motion net. Usage: reload:motion_net file=turn_right", this);
  REGISTER_DEBUG_COMMAND("motion:load_editor_motionnet", 
      "load the temporary editor's motion net.", this);*/
  
  // load motion nets in different folders
  const std::string& dirlocation = Platform::getInstance().theConfigDirectory;
  //const std::string& scheme = Platform::getInstance().theScheme;
  const std::string& id = Platform::getInstance().theHardwareIdentity;
  const std::string& platform = Platform::getInstance().thePlatform;
  const std::string motionnet = "motionnet/";
  loadAvailableMotionNets(dirlocation + "general/" + motionnet);
  loadAvailableMotionNets(dirlocation + "platform/" + platform + "/" + motionnet);
  loadAvailableMotionNets(dirlocation + "robots/" + id + "/" + motionnet);
  loadAvailableMotionNets(dirlocation + "private/" + motionnet);

  //
  keyFrameMotionCreator = registerModule<KeyFrameMotion>("KeyFrameMotion");
}

KeyFrameMotionFactory::~KeyFrameMotionFactory()
{
}

void KeyFrameMotionFactory::loadAvailableMotionNets(std::string dirlocation)
{
  if (!g_str_has_suffix(dirlocation.c_str(), "/"))
  {
    dirlocation = dirlocation + "/";
  }

  GDir* dir = g_dir_open(dirlocation.c_str(), 0, NULL);
  if (dir != NULL)
  {
    const gchar* name;
    while ((name = g_dir_read_name(dir)) != NULL)
    {
      if (g_str_has_suffix(name, ".mef"))
      {
        gchar* group = g_strndup(name, strlen(name) - strlen(".cfg"));
        
        std::string completeFileName = dirlocation + name;
        loadMotionNet(completeFileName, std::string(group));

        g_free(group);
      }
    }//end while

    g_dir_close(dir);
  }//end if
}//end loadAvailableMotionNets


void KeyFrameMotionFactory::loadMotionNetFromFile(const std::string& fileName, MotionNet& motionNet) const
{
  ifstream inputFileStream ( fileName.c_str() , ifstream::in );

  if(inputFileStream.fail())
  {
    throw std::string("Could not open MotionNet file.");
  }//end if

  Scanner scanner(inputFileStream);
  MotionNetParser parser(scanner);

  motionNet = parser.parse();

  inputFileStream.close();

  if(motionNet.isEmpty())
  {
    throw std::string("MotionNet file is empty.");
  }//end if
}//end loadMotionNetFromFile


bool KeyFrameMotionFactory::loadMotionNet(const std::string& fileName, const std::string& motionNetName)
{
  try
  {
    MotionNet motionNet;

    cout << "KeyFrameMotionFactory: load " << fileName;
    loadMotionNetFromFile(fileName, motionNet);
    cout << "\tok" << endl;

    // loading successfull: copy to the map
    getMotionNets()[motionNetName] = motionNet;
    return true;
  }
  catch(std::string e)
  {
    // loading failed
    cout << "\tfail" << endl; 
    cout << e << endl;
    return false;
  }
}//end loadMotionNet


Module* KeyFrameMotionFactory::createMotion(const MotionRequest& motionRequest)
{
  keyFrameMotionCreator->setEnabled(false);

  std::string motionName = motion::getName(motionRequest.id);
  const MotionNet* net = getMotionNets().get(motionName);

  if(net != NULL)
  {
    keyFrameMotionCreator->setEnabled(true);
    KeyFrameMotion* motion = keyFrameMotionCreator->getModuleT();
    motion->set(*net, motionRequest.id);

    return keyFrameMotionCreator->getModule();
  }//end if

  return NULL;
}//end createMotion

/*
 * TODO
void KeyFrameMotionFactory::executeDebugCommand(
    const std::string& command, const std::map<std::string,std::string>& arguments,
    std::stringstream &outstream)
{  
  if(command == "reload:motion_net")
  { 
    if(arguments.find("file") != arguments.end())
    {
      string fileName = arguments.find("file")->second;

      outstream << "load " << fileName;

      string filePath = Platform::getInstance().theConfigPathInfo.motionnet + fileName + ".mef";
      if(loadMotionNet(filePath, fileName))
        outstream << " ok\n" << endl;
      else
        outstream << " fail\n" << endl;
    }
    else
      outstream << "Command fail: Usage: reload:motion_net file=turn_right" << endl;
  }
  else if(command == "motion:load_editor_motionnet")
  {
    if(arguments.find("condition") != arguments.end())
    {
      cout << command << endl;
    }
    string filePath = Platform::getInstance().theConfigPathInfo.motionnet + "../" + "play_editor_motionnet.mef";
    outstream << "load motionnet editor file: " << filePath;
    if(loadMotionNet(filePath, string("play_editor_motionnet")))
    {
        outstream << " ok\n" << endl;
    }
    else
        outstream << " fail\n" << endl;
  }//end if
}//end executeDebugCommand
*/
