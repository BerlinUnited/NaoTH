/**
* @file ConfigFileParser.cpp
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Implementation of class ConfigFileParser
*/

#include "Tools/Config/ConfigFileParser.h"

Config ConfigFileParser::parse()
{
  Config configuration;

  eat();
  while( !scanner.eof() )
  {
    parseKey(configuration);
  }//end while

 return configuration;
}//end parse

void ConfigFileParser::parseKey(Config& configuration)
{
  string key(scanner.buffer);
  eat();

  if(isToken(separator))
  {
    eat();
    configuration.set(key, string("1"));
    return;
  }//end if

  isTokenAndEat(equal); // "="

  // read parameter
  string value(scanner.buffer);
  configuration.set(key, value);
  eat();
  
  isTokenAndEat(separator); // ";"
}//end parseKey


std::string ConfigFileParser::getKeyWord(KeyWordID keyWordID)
{
  switch(keyWordID)
  {
    case endOfLine: return std::string("\n");
    case equal: return std::string("=");
    case separator: return std::string(";");
    case comment: return std::string("#");
    default: return std::string("unknown");
  }//end switch
}//end getKeyWord




void ConfigFileParser::eat()
{
  scanner.getNextToken();
}//end eat

bool ConfigFileParser::isToken(KeyWordID tokenId)
{
  return scanner.isToken(getKeyWord(tokenId));
}//end isToken

void ConfigFileParser::isTokenAndEat(KeyWordID tokenId)
{
  if(isToken(tokenId))
    eat();
  else
  {
    stringstream str;
    str << "Syntax error in line " << scanner.getLineNumber() << ". Expected token '" << getKeyWord(tokenId) << "'";
    throw str.str();
  }
}//end isTokenAndEat
