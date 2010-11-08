/* 
 * File:   Configuration.cpp
 * Author: thomas
 * 
 * Created on 8. November 2010, 12:47
 */

#include "Configuration.h"

#include <iostream>

Configuration::Configuration()
{
}

Configuration::Configuration(const Configuration& orig)
{
  clear();
  
  gsize bufferLength;
  gchar* buffer = g_key_file_to_data(orig.keyFile, &bufferLength, NULL);

  g_key_file_load_from_data(keyFile, buffer, bufferLength, G_KEY_FILE_NONE, NULL);

  g_free(buffer);

}

void Configuration::loadFromDir(std::string dirlocation, std::string platformName, std::string macAddress)
{
  if(!g_str_has_suffix(dirlocation.c_str(), "/"))
  {
    dirlocation = dirlocation + "/";
  }

  clear();

  loadFromSingleDir(dirlocation + "general/");
  loadFromSingleDir(dirlocation + "platforms/" + platformName + "/");
  loadFromSingleDir(dirlocation + "robots/" + macAddress + "/");
  loadFromSingleDir(dirlocation + "private/");
}

void Configuration::clear()
{
  if(keyFile != NULL)
  {
    g_key_file_free(keyFile);
  }
  keyFile = g_key_file_new();
}

void Configuration::loadFromSingleDir(std::string dirlocation)
{
  // iterate over all files in the folder

  if(!g_str_has_suffix(dirlocation.c_str(), "/"))
  {
    dirlocation = dirlocation + "/";
  }

  GDir* dir = g_dir_open(dirlocation.c_str(), 0, NULL);
  if(dir != NULL)
  {
    const gchar* name; 
    while((name = g_dir_read_name(dir)) != NULL)
    {
      std::string completeFileName = dirlocation + name;
      if(g_file_test(completeFileName.c_str(), G_FILE_TEST_EXISTS)
        && g_file_test(completeFileName.c_str(), G_FILE_TEST_IS_REGULAR)
        && g_str_has_suffix(completeFileName.c_str(), ".cfg"))
      {
        loadFile(completeFileName);
      }
    }
    g_dir_close(dir);
  }
}

void Configuration::loadFile(std::string file)
{
  GError* err = NULL;
  g_key_file_load_from_file(keyFile, file.c_str(), G_KEY_FILE_KEEP_COMMENTS, &err);
  if(err != NULL)
  {
    std::cerr << "could not load configuration file " << file << ": "
      << err->message << std::endl;
    g_error_free(err);
  }
}

Configuration::~Configuration()
{
  if(keyFile != NULL)
  {
    g_key_file_free(keyFile);
  }
}

