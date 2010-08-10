/**
* @file ConfigLoader.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Declaration of class ConfigLoader
*/

#ifndef __ConfigLoader_h_
#define __ConfigLoader_h_

#include <stdio.h>
#include <stdlib.h>

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>

#include "Config.h"
#include "Scanner.h"
#include "ConfigFileParser.h"

using namespace std;

class ConfigLoader
{
public:

  /* 
   * override the definition of the scanner:
   * almost every character can be used for a token
   */
  class LineScanner: public Scanner
  {
  public:
    LineScanner(istream& inputStream):Scanner(inputStream){}

    virtual ~LineScanner(){}

    virtual bool isTokenChar(char c)
    {
      return c > 0 && c != ';' && c != '=' && !isspace(c);
    }
  };

  static bool loadConfig(std::istream& is, Config& config);
  static bool loadConfigFile(const char* file, Config& config);
  static Config loadConfig(const char* fileName);

private:
  ConfigLoader(){}
  ~ConfigLoader(){}
};

#endif //__ConfigLoader_h_
