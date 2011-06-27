/**
* @file Tools/Debugging/Trace.h
*
* A class representing a trace.
* (copied from GT07 Trace.h)
* 
* @author Thomas Roefer (GT07)
* @author Heinrich Mellmann
*/ 
#ifndef __Trace_h_
#define __Trace_h_

#include "Tools/DataStructures/RingBuffer.h"
#include "Tools/DataStructures/Singleton.h"

#include <ostream>
#include <sstream>

using namespace std;

class Trace : public naoth::Singleton<Trace>
{
protected:
  friend class naoth::Singleton<Trace>;
  Trace(){}
  ~Trace(){}

private:
  /**
  * The class represents single trace points.
  */
  class Line
  {
  public:
    static const unsigned int MAX_FILE_NAME_LENGHT = 256;
    char file[MAX_FILE_NAME_LENGHT]; /**< The name of the source file a passed trace point is located in. */
    int line; /**< The line a passed trace point is located in. */
    
    /** custom message */
    std::string msg;

    /**
    * Constructor.
    * @param f The name of the source file a passed trace point is located in.
    * @param l The line a passed trace point is located in.
    */
    Line(const char* f = "", int l = 0, const std::string& msg = "");
  };

  RingBuffer<Line, 256> buffer; /**< A buffer for the last 256 passed trace points. */

public:
  /**
  * The function adds a new entry to the list of passed trace points.
  * @param file The name of the source file a passed trace point is located in.
  * @param line The line a passed trace point is located in.
  */
  void setCurrentLine(const char* file, int line, const std::string& msg);

  friend ostream& operator<<(ostream& stream, const Trace& trace);

  void dump();
};//end class Trace

// another instance for motion
class MotionTrace : public Trace{};

/**
 * Streaming operator that writes the trace to a stream.
 * @param stream The stream to write on.
 * @param trace The trace to write.
 * @return The stream.
 */ 
ostream& operator<<(ostream& stream, const Trace& trace);



#if defined(DEBUG)
#define GT_TRACE(msg) {std::stringstream s; s << msg; Trace::getInstance().setCurrentLine(__FILE__, __LINE__, s.str());}
#define MOTION_TRACE(msg) {std::stringstream s; s << msg; Trace::getInstance().setCurrentLine(__FILE__, __LINE__, s.str());}
#else
#define GT_TRACE(...)
#define MOTION_TRACE(...)
#endif

#endif // __Trace_h_
