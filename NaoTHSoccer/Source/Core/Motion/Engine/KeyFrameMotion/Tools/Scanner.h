/**
* @file Scanner.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Declaration of class Scanner
*/

#ifndef __Scanner_h_
#define __Scanner_h_

#include <stdio.h>
#include <stdlib.h>

#include <string>
#include <iostream>
#include <cctype>

using namespace std;

// defines the maximal length of a token
#define MAX_TOKEN_LENGTH 1024

class Scanner
{
public:

  Scanner(istream& inputStream)
    : inputStream(inputStream),
      nextChar(' '),
      bufferLength(0),
      lineNumber(1)
  {
    buffer[0] = '\0';
  }

  virtual ~Scanner(){}

  bool eof();
  bool isToken(std::string token);
  void getNextToken();
  bool endOfFile();

  char buffer[MAX_TOKEN_LENGTH];
  int getLineNumber();

private:
  istream& inputStream;
  
  char nextChar;
  int bufferLength;
  virtual bool isTokenChar(char c);
  int lineNumber;

  void getNextChar();
  void skipLine();
  void skipSpace();
};

#endif //__Scanner_h_
