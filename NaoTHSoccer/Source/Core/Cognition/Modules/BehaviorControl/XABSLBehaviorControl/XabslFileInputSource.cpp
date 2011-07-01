/**
* @file XabslFileInputSource.cpp
*
* Implementation of class XabslFileInputSource:
* reads a xabsl behavior from a file
*/

#include "XabslFileInputSource.h"

XabslFileInputSource::XabslFileInputSource(std::string file)
{
  this->file = file;
}

XabslFileInputSource::~XabslFileInputSource()
{
}

bool XabslFileInputSource::open()
{
  try
  {
    inFile.open(file.c_str(), std::ifstream::in);
    skipComments();
  }
  catch(...)
  {
    return false;
  }
  return inFile.is_open();
}//end open

void XabslFileInputSource::close()
{
  inFile.close();
}//end close

double XabslFileInputSource::readValue()
{
  double d;
  inFile >> d;
  return d;
}//end readValue

bool XabslFileInputSource::readString(char* destination, int maxLength)
{

  if(inFile.eof())
  {
    return false;
  }

  try
  {
    skipWhiteSpace();
    inFile.width(maxLength+1);
    inFile >> destination;
  }
  catch(...)
  {
    return false;
  }

  return true;
}//end readString

void XabslFileInputSource::skipComments()
{
  char c = inFile.peek();
	while (c == '/' || c == '\n' || c == '#')
  {
    inFile.ignore(256, '\n');
    c = inFile.peek();
  }
}//end skipComments

void XabslFileInputSource::skipWhiteSpace()
{
  char c = inFile.peek();
	while (c == ' ' || c == '\t' || c == '\n' || c == '\r')
  {
    inFile.ignore(1);
    c = inFile.peek();
  }
}//end skipWhiteSpace
