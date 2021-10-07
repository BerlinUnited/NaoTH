/**
* @file XabslFileInputSource.cpp
*
* Implementation of class XabslFileInputSource:
* reads a xabsl behavior from a file
*/

#include "XabslFileInputSource.h"

bool XabslFileInputSource::open()
{
  try {
    inFile.open(file.c_str(), std::ifstream::in);
    skipComments();
  } catch(...) {
    return false;
  }
  return inFile.is_open();
}

void XabslFileInputSource::close()
{
  inFile.close();
}

double XabslFileInputSource::readValue()
{
  double d;
  inFile >> d;
  return d;
}

bool XabslFileInputSource::readString(char* destination, int maxLength)
{

  if(inFile.eof()) {
    return false;
  }

  try {
    skipWhiteSpace();
    inFile.width(maxLength + 1);
    inFile >> destination;
  } catch(...) {
    return false;
  }

  return true;
}//end readString

void XabslFileInputSource::skipComments()
{
  char c = static_cast<char>(inFile.peek());
  while (c == '/' || c == '\n' || c == '#')
  {
    inFile.ignore(256, '\n');
    c = static_cast<char>(inFile.peek());
  }
}//end skipComments

void XabslFileInputSource::skipWhiteSpace()
{
  char c = static_cast<char>(inFile.peek());
  while (c == ' ' || c == '\t' || c == '\n' || c == '\r')
  {
    inFile.ignore(1);
    c = static_cast<char>(inFile.peek());
  }
}//end skipWhiteSpace
