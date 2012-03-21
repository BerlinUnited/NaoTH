/**
* @file XabslFileInputSource.h
*
* Declaration of class XabslFileInputSource:
* reads a xabsl behavior from a file
*/

#ifndef _XabslFileInputSource_H
#define  _XabslFileInputSource_H

#include <XabslEngine/XabslTools.h>
#include <fstream>

class XabslFileInputSource : public xabsl::InputSource
{
public:
  XabslFileInputSource(std::string file);

  bool open();
  void close();
  double readValue();
  bool readString(char* destination, int maxLength);
  
  void skipComments();
  void skipWhiteSpace();

  virtual ~XabslFileInputSource();

private:
  std::string file;
  std::ifstream inFile;
};

#endif  /* _XabslFileInputSource_H */
