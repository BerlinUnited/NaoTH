/* 
 * File:   Configuration.cpp
 * Author: thomas
 * 
 * Created on 8. November 2010, 12:47
 */

#include "Configuration.h"

Configuration::Configuration()
{
}

Configuration::Configuration(const Configuration& orig)
{
  if(keyFile != NULL)
  {
    g_key_file_free(keyFile);
  }
  
  gsize bufferLength;
  gchar* buffer = g_key_file_to_data(orig.keyFile, &bufferLength, NULL);

  keyFile = g_key_file_new();
  g_key_file_load_from_data(keyFile, buffer, bufferLength, G_KEY_FILE_NONE, NULL);

  g_free(buffer);

}

void Configuration::loadFromDir(std::string dirlocation, std::string platformName, std::string macAddress)
{
  
}

Configuration::~Configuration()
{
  if(keyFile != NULL)
  {
    g_key_file_free(keyFile);
  }
}

