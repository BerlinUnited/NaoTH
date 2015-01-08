/**
* @file Scanner.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Implementation of class Scanner
*/

#include "Scanner.h"

bool Scanner::endOfFile()
{
  return nextChar == EOF;
}//end eof


void Scanner::getNextToken()
{
  skipSpace();
  bufferLength = 0;

  if(nextChar == EOF) 
  {
    buffer[bufferLength++] = nextChar;
    buffer[bufferLength] = '\0';
    return;
  }//end if

  if(!isTokenChar(nextChar))
  {
    buffer[bufferLength++] = nextChar;
    buffer[bufferLength] = '\0';
    getNextChar();
    return;
  }//end if

  while(isTokenChar(nextChar))
  {
    buffer[bufferLength++] = nextChar;
    buffer[bufferLength] = '\0';
    getNextChar();
  }//end while
}//end getNextToken

/*
 * Return true if c is a token character, 
 * i.e., 0-1, a-z, A-Z, or one of the character '.','-' or '_'
 */
bool Scanner::isTokenChar(char c)
{
  return isalnum(c) || c == '.' || c == '-' || c == '_';
}//end isTolenChar


bool Scanner::isToken(std::string token)
{
  return token == buffer;
}//end isToken

bool Scanner::eof()
{
  return nextChar == EOF;
}//end eof

void Scanner::skipSpace()
{
  while(isspace(nextChar) || nextChar == '#')
  {
    if(nextChar == '\n')
      lineNumber++;

    if(nextChar == '#') 
      skipLine();
    else
      getNextChar();
  }
}//end skipSpace


void Scanner::skipLine()
{
  while(nextChar != EOF && nextChar != '\n')
  {
    getNextChar();    
  }//end while
  
  getNextChar(); 
  lineNumber++;
}//end skipLine


void Scanner::getNextChar()
{
  if(inputStream.good())
    nextChar = (char) inputStream.get();
  else
    nextChar = EOF;
}//end getNextChar


int Scanner::getLineNumber()
{
  return lineNumber;
}//end getLineNumber


