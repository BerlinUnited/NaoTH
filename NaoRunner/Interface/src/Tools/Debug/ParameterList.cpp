/**
* @file ParameterList.cpp
*
* @author <a href="mailto:aschulz@informatik.hu-berlin.de">Alexander Schulz</a>
* 
*/

#include "Tools/Debug/ParameterList.h"
//#include "Debug/DebugServer.h"
#include "Tools/Config/ConfigLoader.h"
#include "Tools/Debug/NaoTHAssert.h"

ParameterList::ParameterList(const std::string& parentClassName, const std::string& configureFileName)
{
  this->parentClassName = parentClassName;
  this->configureFileName = configureFileName;


//  REGISTER_DEBUG_COMMAND(string(parentClassName).append(":set"),
//    string("set the parameters for ").append(parentClassName), this);
//
//  REGISTER_DEBUG_COMMAND(string(parentClassName).append(":list"),
//    "get the list of available parameters", this);
//
//  REGISTER_DEBUG_COMMAND(string(parentClassName).append(":store"),
//    "store the configure file according to the path", this);

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

void ParameterList::saveToConfig(Config& config)
{
  for(std::map<std::string, unsigned int*>::iterator iter = unsignedIntParameterReferences.begin(); iter != unsignedIntParameterReferences.end(); iter++)
  {
    config.set(iter->first, *(iter->second));
  }//end for

  for(std::map<std::string, int*>::iterator iter = intParameterReferences.begin(); iter != intParameterReferences.end(); iter++)
  {
    config.set(iter->first, *(iter->second));
  }//end for

  for(std::map<std::string, double*>::iterator iter = doubleParameterReferences.begin(); iter != doubleParameterReferences.end(); iter++)
  {
    config.set(iter->first, *(iter->second));
  }//end for

  for(std::map<std::string, std::string*>::iterator iter = stringParameterReferences.begin(); iter != stringParameterReferences.end(); iter++)
  {
    config.set(iter->first, *(iter->second));
  }//end for

  for(std::map<std::string, bool*>::iterator iter = boolParameterReferences.begin(); iter != boolParameterReferences.end(); iter++)
  {
    config.set<bool>(iter->first, *(iter->second));
  }//end for
}//end saveToConfig


void ParameterList::loadFromConfig(const Config& config)
{
  for (std::map<std::string, unsigned int*>::const_iterator iter = unsignedIntParameterReferences.begin(); iter != unsignedIntParameterReferences.end(); iter++) {
    unsigned int tmp;
    if (config.get(iter->first, tmp))
      *(iter->second) = tmp;
  }//end for

  for(std::map<std::string, int*>::const_iterator iter = intParameterReferences.begin(); iter != intParameterReferences.end(); iter++)
  {
    int tmp;
    if(config.get(iter->first, tmp))
      *(iter->second) = tmp;
  }//end for

  for(std::map<std::string, double*>::const_iterator iter = doubleParameterReferences.begin(); iter != doubleParameterReferences.end(); iter++)
  {
    double tmp;
    if(config.get(iter->first, tmp))
      *(iter->second) = tmp;
  }//end for

  for(std::map<std::string, std::string*>::const_iterator iter = stringParameterReferences.begin(); iter != stringParameterReferences.end(); iter++)
  {
    std::string tmp;
    if(config.get(iter->first, tmp))
      *(iter->second) = tmp;
  }//end for

  for (std::map < std::string, bool*>::const_iterator iter = boolParameterReferences.begin(); iter != boolParameterReferences.end(); iter++)
  {
    bool tmp;
    if (config.get(iter->first, tmp))
      *(iter->second) = tmp;
  }//end for
}//end loadFromConfig


bool ParameterList::loadFromConfigFile()
{
  // if it connects to a configure file, load it from file
  if (!configureFileName.empty()) {
    return loadFromConfigFile(configureFileName);
  }
  return false;
}//end loadFromConfigFile

bool ParameterList::loadFromConfigFile(const std::string& filename)
{
  Config cfg;
  if(ConfigLoader::loadConfigFile(filename.c_str(), cfg))
  {
    loadFromConfig(cfg);
    return true;
  }
  return false;
}//end loadFromConfigFile

void ParameterList::saveToConfigFile()
{
  // if it connects to a configure file, save it
  if (!configureFileName.empty())
  {
    saveToConfigFile(configureFileName);
  }
  else
  {
    cerr<<"["<<parentClassName<<"] configure file name is empty!"<<endl;
  }
}//end saveToConfigFile

void ParameterList::saveToConfigFile(const std::string& filename)
{
    Config cfg;
    saveToConfig(cfg);
    ofstream of(filename.c_str());
    if (of) {
        of << cfg;
    } else {
        cerr << parentClassName <<" : save to configure file "<< filename << " failed" << endl;
    }
}//end saveToConfigFile

void ParameterList::setConfigFile(const std::string& filename)
{
  configureFileName = filename;
}//end setConfigFile

void ParameterList::executeDebugCommand(
    const std::string& command, const std::map<std::string,std::string>& arguments, 
    std::stringstream &outstream)
{  
  if(command == string(parentClassName).append(":set")) 
  {
    stringstream ss;
    for (std::map<std::string, std::string>::const_iterator iter = arguments.begin(); iter != arguments.end(); iter++) {
      ss << iter->first << " = " << iter->second << ";\n";
    }
    Config cfg;
    if ( ConfigLoader::loadConfig(ss, cfg) ){
      loadFromConfig(cfg);
      outstream<<"set " << parentClassName << "successfully"<<endl;
    }
    else{
      outstream<<"set " << parentClassName << "failed"<<endl;
    }
    
    saveToConfigFile();
  }
  else if(command == string(parentClassName).append(":list"))
  {
    Config cfg;
    saveToConfig(cfg);
    cfg.toStream(outstream, ";\n");
  }
  else if ( command == string(parentClassName).append(":store"))
  {
    Config cfg;
    saveToConfig(cfg);
    outstream<<configureFileName<<"\n"<<cfg;
  }
}//end executeDebugCommand
