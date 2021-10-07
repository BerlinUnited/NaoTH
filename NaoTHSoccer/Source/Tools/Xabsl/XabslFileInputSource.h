/**
* @file XabslFileInputSource.h
*
* Declaration of class XabslFileInputSource:
* reads and tokenazes a xabsl behavior from an intermediate code file, e.h., behavior-ic.dat
*/

#ifndef XABSLFILEINPUTSOURCE_H
#define XABSLFILEINPUTSOURCE_H

#include <XabslEngine/XabslTools.h>
#include <fstream>

class XabslFileInputSource : public xabsl::InputSource
{
public:
  XabslFileInputSource(const std::string& file) : file(file) {}
  virtual ~XabslFileInputSource() {}

  // virtual methods from InputSource
  virtual bool open();
  virtual void close();
  virtual double readValue();
  virtual bool readString(char* destination, int maxLength);
  
private:
  void skipComments();
  void skipWhiteSpace();

private:
  std::string file;
  std::ifstream inFile;
};

#endif  // XABSLFILEINPUTSOURCE_H
