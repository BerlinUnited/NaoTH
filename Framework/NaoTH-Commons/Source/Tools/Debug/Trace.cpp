/**
* @file Tools/Debugging/Trace.cpp
*
* A class representing a trace.
* 
* @author Thomas Roefer
*/ 

#include "Trace.h"
#include "NaoTHAssert.h"
#include <sstream>
#include "Tools/SynchronizedFileWriter.h"
#include "Tools/NaoTime.h"
#include <string.h>

using namespace std;

//static Trace* trace = 0;

Trace::Line::Line(const char* f, int l, const std::string& msg)
  : 
  line(l),
  msg(msg)
{
  ASSERT(strlen(f) < MAX_FILE_NAME_LENGHT);
#ifdef WIN32
  strcpy_s(file, f);
#else
  strcpy(file, f);
#endif
}

void Trace::setCurrentLine(const char* file, int line, const std::string& msg) 
{
  buffer.add(Line(file, line, msg));
}//end setCurrentLine

void Trace::dump()
{
  std::stringstream sstream;
  sstream << *this;

  // write to file
  std::stringstream dumpName;
  dumpName << "trace.dump." << naoth::NaoTime::getSystemTimeInMicroSeconds();
  SynchronizedFileWriter::saveStreamToFile(sstream, dumpName.str());
}//end dump

 
std::ostream& operator<<(ostream& stream, const Trace& trace) 
{
  stream << trace.buffer.getNumberOfEntries() << endl;
  for(int i = trace.buffer.getNumberOfEntries() - 1; i >= 0; --i)
    stream << trace.buffer[i].file << ":" << trace.buffer[i].line << " " << trace.buffer[i].msg << endl;
  return stream;
}
