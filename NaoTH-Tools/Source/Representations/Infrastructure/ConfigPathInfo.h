/* 
 * File:   ConfigPathInfo.h
 * Author: Heinrich Mellmann
 *
 * Created on 1. Februar 2009, 20:07
 */

#ifndef _ConfigPathInfo_h
#define	_ConfigPathInfo_h

// Tools
#include <glib.h>

#include <string>

using namespace std;

#define READ_PATH(path_name) \
{ \
  gchar* buf = g_key_file_get_string(scheme_cfg, "scheme", #path_name, NULL); \
  if(buf == NULL) \
  { \
   std::cerr << "No path for " << #path_name << " file in " << scheme << " specified." << endl; \
  } \
  else \
  { \
    path_name = std::string(buf); \
    g_free(buf); \
  } \
} \

class ConfigPathInfo
{
public:

  ConfigPathInfo()
  {
  }

  ~ConfigPathInfo(){}
  
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
    scheme = path;
    
    try{
      cout << "Load Cognition configuration files..." << endl;
      cout << "-------------------------------------" << endl;

      // read scheme-file
      GKeyFile* scheme_cfg = g_key_file_new();
      g_key_file_load_from_file(scheme_cfg, scheme.c_str(), G_KEY_FILE_NONE, NULL);
      
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

      g_key_file_free(scheme_cfg);
      
      cout << "-------------------------------------" << endl;
    }catch(string e)
    {
      cout << e << endl;
    }
  }//end loadPathInfo

};

#undef READ_PATH
#endif	/* _ConfigPathInfo_h */

