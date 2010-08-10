/* 
 * File:   ConfigPathInfo.h
 * Author: Heinrich Mellmann
 *
 * Created on 1. Februar 2009, 20:07
 */

#ifndef _ConfigPathInfo_h
#define	_ConfigPathInfo_h

// Tools
#include "Tools/Config/ConfigLoader.h"
#include "Tools/Config/Config.h"

#include <string>

using namespace std;

#define READ_PATH(path_name) \
{ \
  string path_name##_file; \
  if(!scheme_cfg.get(#path_name, path_name##_file)) \
    cout << "No path for " << #path_name << " file in " << scheme_file << " specified." << endl; \
  else \
    path_name = string("Config/").append(path_name##_file); \
} \

class ConfigPathInfo
{
public:

  ConfigPathInfo()
  {
  }

  ~ConfigPathInfo(){}

  string init;
  string scheme;

  string modules;
  string colortable;
  string behavior;
  string camera_parameter;
  string inertialsensor_parameter;
  string motionnet;
  string fsr_parameter;
  string ikmotion_parameter;
  string debug_commands;
  string field_info;
  string player;
  string joint_info;
  string mass_info;

  void loadPathInfo(const string& path)
  {
    init = path;
    
    try{
      cout << "Load Cognition configuration files..." << endl;
      cout << "-------------------------------------" << endl;

      // read init-file
      Config init_cfg = ConfigLoader::loadConfig(init.c_str());


      // read scheme-path
      string scheme_file;
      if(!init_cfg.get("scheme",scheme_file))
        cout << "No path for scheme in init.cfg specified." << endl;
      else
        scheme = string("Config/").append(scheme_file);

      // read scheme-file
      Config scheme_cfg = ConfigLoader::loadConfig(string("Config/").append(scheme_file).c_str());

      READ_PATH(modules)
      READ_PATH(colortable)
      READ_PATH(behavior)
      READ_PATH(motionnet)
      READ_PATH(camera_parameter)
      READ_PATH(inertialsensor_parameter)
      READ_PATH(fsr_parameter)
      READ_PATH(ikmotion_parameter)
      READ_PATH(debug_commands)
      READ_PATH(field_info)
      READ_PATH(player)
      READ_PATH(joint_info)
      READ_PATH(mass_info)
      
      cout << "-------------------------------------" << endl;
    }catch(string e)
    {
      cout << e << endl;
    }
  }//end loadPathInfo
};

#undef READ_PATH
#endif	/* _ConfigPathInfo_h */

