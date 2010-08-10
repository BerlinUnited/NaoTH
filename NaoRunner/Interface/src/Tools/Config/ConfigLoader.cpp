/**
* @file ConfigLoader.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Implementation of class ConfigLoader
*/

#include "Tools/Config/ConfigLoader.h"

bool ConfigLoader::loadConfigFile(const char* file, Config& config)
{
  cout << "Load " << file << "\t";

  ifstream inputFileStream ( file , ifstream::in );
  bool ok = loadConfig(inputFileStream, config);
  inputFileStream.close();
  
  cout << (ok?"ok":"fail") << endl;
  return ok;
}//end loadConfigFile

bool ConfigLoader::loadConfig(std::istream& is, Config& config)
{
  if (!is) {
    return false;
  }//end if

  LineScanner scanner(is);
  ConfigFileParser parser(scanner);

  try {
    config = parser.parse();
  } catch (string e) {
    std::cout << "ConfigLoader Error parsing " << "\t" << e << "\n";
    return false;
  }
  return true;
}//end loadConfig

Config ConfigLoader::loadConfig(const char* fileName)
{
  Config cfg;
  ConfigLoader::loadConfigFile(fileName, cfg);
  return cfg;
}//end loadConfig
