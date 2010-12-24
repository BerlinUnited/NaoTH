/* 
 * File:   Configuration.cpp
 * Author: thomas
 * 
 * Created on 8. November 2010, 12:47
 */

#include "Configuration.h"

#include <iostream>
#include <fstream>
#include <string.h>
#include <list>

using namespace naoth;

Configuration::Configuration()
{
  keyFile = g_key_file_new();
}

Configuration::Configuration(const Configuration& orig)
{
  keyFile = g_key_file_new();

  gsize bufferLength;
  gchar* buffer = g_key_file_to_data(orig.keyFile, &bufferLength, NULL);

  g_key_file_load_from_data(keyFile, buffer, bufferLength, G_KEY_FILE_NONE, NULL);

  g_free(buffer);

}

void Configuration::loadFromDir(std::string dirlocation, std::string platformName, std::string macAddress)
{
  if (!g_str_has_suffix(dirlocation.c_str(), "/"))
  {
    dirlocation = dirlocation + "/";
  }

  clear();

  if (g_file_test(dirlocation.c_str(), G_FILE_TEST_EXISTS) && g_file_test(dirlocation.c_str(), G_FILE_TEST_IS_DIR))
  {
    loadFromSingleDir(dirlocation + "general/");
    loadFromSingleDir(dirlocation + "platforms/" + platformName + "/");
    loadFromSingleDir(dirlocation + "robots/" + macAddress + "/");
    loadFromSingleDir(dirlocation + "private/");
  } else
  {
    g_warning("Could not load configuration from %s: directory does not exist", dirlocation.c_str());
  }
}

void Configuration::clear()
{
  if (keyFile != NULL)
  {
    g_key_file_free(keyFile);
  }
  keyFile = g_key_file_new();
}

void Configuration::loadFromSingleDir(std::string dirlocation)
{
  // iterate over all files in the folder

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
      if (g_str_has_suffix(name, ".cfg"))
      {
        gchar* group = g_strndup(name, strlen(name) - strlen(".cfg"));
        std::string completeFileName = dirlocation + name;
        if (g_file_test(completeFileName.c_str(), G_FILE_TEST_EXISTS)
          && g_file_test(completeFileName.c_str(), G_FILE_TEST_IS_REGULAR))
        {
          loadFile(completeFileName, std::string(group));
        }
        g_free(group);
      }


    }
    g_dir_close(dir);
  }
}

void Configuration::loadFile(std::string file, std::string groupName)
{
  GError* err = NULL;

  GKeyFile* tmpKeyFile = g_key_file_new();
  g_key_file_load_from_file(tmpKeyFile, file.c_str(), G_KEY_FILE_NONE, &err);
  if (err != NULL)
  {
    g_error("%s: %s", file.c_str(), err->message);
    g_error_free(err);
  } else
  {
    // syntactically correct, check if there is only one group with the same 
    // name as the file
    bool groupOK = true;
    gsize numOfGroups;
    gchar** groups = g_key_file_get_groups(tmpKeyFile, &numOfGroups);
    for (gsize i = 0; groupOK && i < numOfGroups; i++)
    {
      if (g_strcmp0(groups[i], groupName.c_str()) != 0)
      {
        groupOK = false;
        g_error("%s: config file contains illegal group \"%s\"", file.c_str(), groups[i]);
        break;
      }
    }

    if(groupOK && numOfGroups == 1)
    {
      // copy every single value to our configuration
      gsize numOfKeys = 0;
      gchar** keys = g_key_file_get_keys(tmpKeyFile, groups[0], &numOfKeys, NULL);
      for(gsize i=0; i < numOfKeys; i++)
      {
        gchar* buffer = g_key_file_get_value(tmpKeyFile, groups[0], keys[i], NULL);
        g_key_file_set_value(keyFile, groups[0], keys[i], buffer);
        g_free(buffer);
      }

      g_message("loaded %s", file.c_str());
    }

    g_strfreev(groups);

  }

  g_key_file_free(tmpKeyFile);
}

void Configuration::save(std::string dirlocation)
{
  if (!g_str_has_suffix(dirlocation.c_str(), "/"))
  {
    dirlocation = dirlocation + "/";
  }

  std::list<std::string> groups = getGroups();
  for(std::list<std::string>::const_iterator it=groups.begin(); it != groups.end(); it++)
  {
    saveFile(dirlocation + "private/" + *it + ".cfg", *it );
  }
}

void Configuration::saveFile(std::string file, std::string group)
{
  GKeyFile* tmpKeyFile = g_key_file_new();
  gsize numOfKeys = 0;
  gchar** keys = g_key_file_get_keys(keyFile, group.c_str(), &numOfKeys, NULL);
  for(gsize i=0; i < numOfKeys; i++)
  {
    GError* err = NULL;
    gchar* buffer = g_key_file_get_value(keyFile, group.c_str(), keys[i], &err);
    if(err != NULL)
    {
      g_warning("%s", err->message);
    }
    g_key_file_set_value(tmpKeyFile, group.c_str(), keys[i], buffer);
    g_free(buffer);
  }
  GError* err = NULL;
  gsize dataLength;
  gchar* data = g_key_file_to_data(tmpKeyFile, &dataLength, &err);
  if(err == NULL)
  {
    if(dataLength > 0)
    {
      std::ofstream outFile;
      outFile.open(file.c_str(), std::ios::out);
      outFile.write(data, dataLength);
      outFile.close();
      g_free(data);
    }
  }
  else
  {
    g_error("could not save configuration file %s: %s", file.c_str(), err->message);
    g_error_free(err);
  }

  g_key_file_free(tmpKeyFile);
}

std::list<std::string> Configuration::getGroups()
{

  std::list<std::string> result;
  gsize length = 0;
  gchar** groups = g_key_file_get_groups(keyFile, &length);
  for(gsize i=0; i < length; i++)
  {
    result.push_back(std::string(groups[i]));
  }
  g_strfreev(groups);
  return result;
}


std::list<std::string> Configuration::getKeys(std::string group)
{

  std::list<std::string> result;
  gsize length = 0;
  gchar** keys = g_key_file_get_keys(keyFile, group.c_str(), &length, NULL);
  for(gsize i=0; i < length; i++)
  {
    result.push_back(std::string(keys[i]));
  }
  g_strfreev(keys);
  return result;
}

bool Configuration::hasKey(std::string group, std::string key)
{
  return g_key_file_has_key(keyFile, group.c_str(), key.c_str(), NULL) > 0;
}

std::string Configuration::getString(std::string group, std::string key)
{
  gchar* buf = g_key_file_get_string(keyFile, group.c_str(), key.c_str(), NULL);
  if (buf != NULL)
  {
    std::string result(buf);
    g_free(buf);
    return result;
  }
  return "";
}

void Configuration::setString(std::string group, std::string key, std::string value)
{
  g_key_file_set_string(keyFile, group.c_str(), key.c_str(), value.c_str());
}

std::string Configuration::getRawValue(std::string group, std::string key)
{
  gchar* buf = g_key_file_get_value(keyFile, group.c_str(), key.c_str(), NULL);
  if (buf != NULL)
  {
    std::string result(buf);
    g_free(buf);
    return result;
  }
  return "";
}

void Configuration::setRawValue(std::string group, std::string key, std::string value)
{
  g_key_file_set_value(keyFile, group.c_str(), key.c_str(), value.c_str());
}

int Configuration::getInt(std::string group, std::string key)
{
  return g_key_file_get_integer(keyFile, group.c_str(), key.c_str(), NULL);
}

void Configuration::setInt(std::string group, std::string key, int value)
{
  g_key_file_set_integer(keyFile, group.c_str(), key.c_str(), value);
}

double Configuration::getDouble(std::string group, std::string key)
{
  return g_key_file_get_double(keyFile, group.c_str(), key.c_str(), NULL);
}

void Configuration::setDouble(std::string group, std::string key, double value)
{

  g_key_file_set_double(keyFile, group.c_str(), key.c_str(), value);
}

bool Configuration::getBool(std::string group, std::string key)
{
  return g_key_file_get_boolean(keyFile, group.c_str(), key.c_str(), NULL) > 0;
}

void Configuration::setBool(std::string group, std::string key, bool value)
{
  g_key_file_set_boolean(keyFile, group.c_str(), key.c_str(), value);
}

Configuration::~Configuration()
{
  if (keyFile != NULL)
  {
    g_key_file_free(keyFile);
  }
}

