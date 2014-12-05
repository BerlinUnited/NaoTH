/**
 * @file ParameterList.cpp
 *
 * @author <a href="mailto:aschulz@informatik.hu-berlin.de">Alexander Schulz</a>
 * @author <a href="mailto:krause@informatik.hu-berlin.de">Thomas Krause</a>
 *
 */

#include <string>

#include <PlatformInterface/Platform.h>
#include <Tools/Debug/NaoTHAssert.h>
#include "ParameterList.h"

unsigned int& ParameterList::registerParameter(const std::string& name, unsigned int& parameter)
{
  ASSERT(unsignedIntParameters.find(name) == unsignedIntParameters.end());
  unsignedIntParameters[name] = &parameter;
  return parameter;
}

int& ParameterList::registerParameter(const std::string& name, int& parameter)
{
  ASSERT(intParameters.find(name) == intParameters.end());
  intParameters[name] = &parameter;
  return parameter;
}

double& ParameterList::registerParameter(const std::string& name, double& parameter)
{
  ASSERT(doubleParameters.find(name) == doubleParameters.end());
  doubleParameters[name] = &parameter;

  return parameter;
}

std::string& ParameterList::registerParameter(const std::string& name, std::string& parameter)
{
  ASSERT(stringParameters.find(name) == stringParameters.end());
  stringParameters[name] = &parameter;
  return parameter;
}

bool& ParameterList::registerParameter(const std::string& name, bool& parameter)
{
  ASSERT(boolParameters.find(name) == boolParameters.end());
  boolParameters[name] = &parameter;
  return parameter;
}

void ParameterList::syncWithConfig()
{
  naoth::Configuration& config =  naoth::Platform::getInstance().theConfiguration;

  // unsigned int
  for (std::map<std::string, unsigned int*>::const_iterator iter = unsignedIntParameters.begin(); iter != unsignedIntParameters.end(); ++iter)
  {
    if (config.hasKey(name, iter->first)) {
      *(iter->second) = config.getInt(name, iter->first);
    } else {
      config.setDefault(name, iter->first, *(iter->second));
    }
  }//end for

  // int
  for (std::map<std::string, int*>::const_iterator iter = intParameters.begin(); iter != intParameters.end(); ++iter)
  {
    if (config.hasKey(name, iter->first)) {
      *(iter->second) = config.getInt(name, iter->first);
    } else {
      config.setDefault(name, iter->first, *(iter->second));
    }
  }//end for

  // double
  for (std::map<std::string, double*>::const_iterator iter = doubleParameters.begin(); iter != doubleParameters.end(); ++iter)
  {
    if (config.hasKey(name, iter->first)) {
      *(iter->second) = config.getDouble(name, iter->first);
    } else {
      config.setDefault(name, iter->first, *(iter->second));
    }
  }//end for

  // string
  for (std::map<std::string, std::string*>::const_iterator iter = stringParameters.begin(); iter != stringParameters.end(); ++iter)
  {
    if (config.hasKey(name, iter->first)) {
      *(iter->second) = config.getString(name, iter->first);
    } else {
      config.setDefault(name, iter->first, *(iter->second));
    }
  }//end for

  // bool
  for (std::map<std::string, bool*>::const_iterator iter = boolParameters.begin(); iter != boolParameters.end(); ++iter)
  {
    if (config.hasKey(name, iter->first)) {
      *(iter->second) = config.getBool(name, iter->first);
    } else {
      config.setDefault(name, iter->first, *(iter->second));
    }
  }//end for

}//end syncWithConfig

void ParameterList::saveToConfig()
{
  naoth::Configuration& config =  naoth::Platform::getInstance().theConfiguration;

  for(std::map<std::string, unsigned int*>::const_iterator iter = unsignedIntParameters.begin(); iter != unsignedIntParameters.end(); ++iter)
  {
    config.setInt(name, iter->first, *(iter->second));
  }

  for(std::map<std::string, int*>::const_iterator iter = intParameters.begin(); iter != intParameters.end(); ++iter)
  {
    config.setInt(name, iter->first, *(iter->second));
  }

  for(std::map<std::string, double*>::const_iterator iter = doubleParameters.begin(); iter != doubleParameters.end(); ++iter)
  {
    config.setDouble(name, iter->first, *(iter->second));
  }

  for(std::map<std::string, std::string*>::const_iterator iter = stringParameters.begin(); iter != stringParameters.end(); ++iter)
  {
    config.setString(name, iter->first, *(iter->second));
  }

  for(std::map<std::string, bool*>::const_iterator iter = boolParameters.begin(); iter != boolParameters.end(); ++iter)
  {
    config.setBool(name, iter->first, *(iter->second));
  }

  config.save();
}

std::string ParameterList::convertName(std::string name)
{
  for( std::string::iterator i=name.begin(); i!= name.end();  )
  {
    if ( *i == '[' || *i == ']' )
    {
      i = name.erase(i);
    }
    else
    {
      ++i;
    }
  }
  return name;
}//end convertName

