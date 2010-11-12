/**
* @file ParameterList.cpp
*
* @author <a href="mailto:aschulz@informatik.hu-berlin.de">Alexander Schulz</a>
* 
*/

#include <string>

#include "Core/Cognition/CognitionDebugServer.h"
#include <Tools/Debug/NaoTHAssert.h>
#include "ParameterList.h"

ParameterList::ParameterList(const std::string& parentClassName)
{
  this->parentClassName = parentClassName;
  
  REGISTER_DEBUG_COMMAND(std::string(parentClassName).append(":set"),
    std::string("set the parameters for ").append(parentClassName), this);

  REGISTER_DEBUG_COMMAND(std::string(parentClassName).append(":list"),
    "get the list of available parameters", this);

  REGISTER_DEBUG_COMMAND(std::string(parentClassName).append(":store"),
    "store the configure file according to the path", this);
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

void ParameterList::executeDebugCommand(
    const std::string& command, const std::map<std::string,std::string>& arguments, 
    std::stringstream &outstream)
{  
  if(command == std::string(parentClassName).append(":set"))
  {
    std::stringstream ss;
    for (std::map<std::string, std::string>::const_iterator iter = arguments.begin(); iter != arguments.end(); iter++) {
      ss << iter->first << " = " << iter->second << ";\n";
    }
    // TODO
  }
  else if(command == std::string(parentClassName).append(":list"))
  {
    // TODO
  }
  else if ( command == std::string(parentClassName).append(":store"))
  {
    // TODO
  }
}//end executeDebugCommand
