/**
* @file ConfigFileParser.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Declaration of class ConfigFileParser
*/

#ifndef __ConfigFileParser_h_
#define __ConfigFileParser_h_

#include <stdio.h>
#include <stdlib.h>

#include <string>
#include <cstring>
#include <iostream>
#include <sstream>

#include "Scanner.h"
#include "Config.h"


class ConfigFileParser
{
public:

  ConfigFileParser(Scanner& scanner) 
    : scanner(scanner)
  {}

  ~ConfigFileParser(){}

  Config parse();

private:

  enum KeyWordID
  {
    endOfLine,
    equal,
    separator,
    comment,
    numberOfKeyWordID
  };

  Scanner& scanner;

  void parseKey(Config& configuration);

  std::string getKeyWord(KeyWordID keyWordID);
  void eat();
  bool isToken(KeyWordID tokenId);
  void isTokenAndEat(KeyWordID tokenId);

};

#endif //__ConfigFileParser_h_
