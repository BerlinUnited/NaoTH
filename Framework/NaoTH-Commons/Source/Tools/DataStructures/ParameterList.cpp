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

ParameterList::ParameterList(const std::string& parentClassName)
{
  this->parentClassName = parentClassName;
}//end constructor ParameterList

unsigned int& ParameterList::registerParameter(const std::string& name, unsigned int& parameter)
{
  ASSERT(unsignedIntParameterReferences.find(name) == unsignedIntParameterReferences.end());
  unsignedIntParameterReferences[name] = &parameter;
  return parameter;
}//end registerParameter

int& ParameterList::registerParameter(const std::string& name, int& parameter)
{
  ASSERT(intParameterReferences.find(name) == intParameterReferences.end());
  intParameterReferences[name] = &parameter;
  return parameter;
}//end registerParameter

double& ParameterList::registerParameter(const std::string& name, double& parameter)
{
  ASSERT(doubleParameterReferences.find(name) == doubleParameterReferences.end());
  doubleParameterReferences[name] = &parameter;

  return parameter;
}//end registerParameter

std::string& ParameterList::registerParameter(const std::string& name, std::string& parameter)
{
  ASSERT(stringParameterReferences.find(name) == stringParameterReferences.end());
  stringParameterReferences[name] = &parameter;
  return parameter;
}//end registerParameter

bool& ParameterList::registerParameter(const std::string& name, bool& parameter)
{
  ASSERT(boolParameterReferences.find(name) == boolParameterReferences.end());
  boolParameterReferences[name] = &parameter;
  return parameter;
}

void ParameterList::syncWithConfig()
{
  naoth::Configuration& config =  naoth::Platform::getInstance().theConfiguration;

  // unsigned int
  for (std::map<std::string, unsigned int*>::const_iterator iter = unsignedIntParameterReferences.begin(); iter != unsignedIntParameterReferences.end(); ++iter)
  {
    if (config.hasKey(parentClassName, iter->first))
    {
      *(iter->second) = config.getInt(parentClassName, iter->first);
    }
    else
    {
      config.setDefault(parentClassName, iter->first, iter->second);
    }
  }//end for

  // int
  for (std::map<std::string, int*>::const_iterator iter = intParameterReferences.begin(); iter != intParameterReferences.end(); ++iter)
  {
    if (config.hasKey(parentClassName, iter->first))
    {
      *(iter->second) = config.getInt(parentClassName, iter->first);
    }
    else
    {
      config.setDefault(parentClassName, iter->first, iter->second);
    }
  }//end for

  // double
  for (std::map<std::string, double*>::const_iterator iter = doubleParameterReferences.begin(); iter != doubleParameterReferences.end(); ++iter)
  {
    if (config.hasKey(parentClassName, iter->first))
    {
      *(iter->second) = config.getDouble(parentClassName, iter->first);
    }
    else
    {
      config.setDefault(parentClassName, iter->first, iter->second);
    }
  }//end for

  // string
  for (std::map<std::string, std::string*>::const_iterator iter = stringParameterReferences.begin(); iter != stringParameterReferences.end(); ++iter)
  {
    if (config.hasKey(parentClassName, iter->first))
    {
      *(iter->second) = config.getString(parentClassName, iter->first);
    }
    else
    {
      config.setDefault(parentClassName, iter->first, iter->second);
    }
  }//end for

  // bool
  for (std::map<std::string, bool*>::const_iterator iter = boolParameterReferences.begin(); iter != boolParameterReferences.end(); ++iter)
  {
    if (config.hasKey(parentClassName, iter->first))
    {
      *(iter->second) = config.getBool(parentClassName, iter->first);
    }
    else
    {
      config.setDefault(parentClassName, iter->first, iter->second);
    }
  }//end for

}

void ParameterList::saveToConfig()
{
  naoth::Configuration& config =  naoth::Platform::getInstance().theConfiguration;

  for(std::map<std::string, unsigned int*>::iterator iter = unsignedIntParameterReferences.begin(); iter != unsignedIntParameterReferences.end(); ++iter)
  {
    config.setInt(parentClassName, iter->first, *(iter->second));
  }//end for

  for(std::map<std::string, int*>::iterator iter = intParameterReferences.begin(); iter != intParameterReferences.end(); ++iter)
  {
    config.setInt(parentClassName, iter->first, *(iter->second));
  }//end for

  for(std::map<std::string, double*>::iterator iter = doubleParameterReferences.begin(); iter != doubleParameterReferences.end(); ++iter)
  {
    config.setDouble(parentClassName, iter->first, *(iter->second));
  }//end for

  for(std::map<std::string, std::string*>::iterator iter = stringParameterReferences.begin(); iter != stringParameterReferences.end(); ++iter)
  {
    config.setString(parentClassName, iter->first, *(iter->second));
  }//end for

  for(std::map<std::string, bool*>::iterator iter = boolParameterReferences.begin(); iter != boolParameterReferences.end(); ++iter)
  {
    config.setBool(parentClassName, iter->first, *(iter->second));
  }//end for

  config.save();
}

std::string ParameterList::covertName(std::string name)
{
  for( string::iterator i=name.begin(); i!= name.end();  )
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
}

