/**
* @file XabslTools.h
*
* Definition of several helper classes for the Engine.
*
* @author <a href="http://www.martin-loetzsch.de">Martin Loetzsch</a>
* @author <a href="http://www.sim.informatik.tu-darmstadt.de/pers/card/risler.html">Max Risler</a>
* @author <a href="http://www.informatik.hu-berlin.de/~juengel">Matthias Jüngel</a>
*/

#ifndef __XabslTools_h_
#define __XabslTools_h_

#include "XabslArray.h"

namespace xabsl 
{

/**
* A Engine helper class for reading input data from files or from memory.
*/
class InputSource
{
public:
  /** Virtual destructor */
  virtual ~InputSource() {}
  
  /** opens the source that contains the intermediate code */
  virtual bool open() = 0;
  
  /** closes the source */
  virtual void close() = 0;

  /** reads a numeric value from the input source */
  virtual double readValue() = 0;

  /** 
  * reads a string from the input source
  * @param destination The position where to write the string
  * @param maxLength the maximum length of the string
  * @return if the read succeded
  */
  virtual bool readString(char* destination, int maxLength) = 0;
};

/**
* A Engine helper class for handling errors and debug messages
*/
class ErrorHandler
{
public:
  /** constructor */
  ErrorHandler() 
  :
  errorsOccurred(false)
  {
    for(int i = 0; i < 300; i++)
    {
      messageBuffer[i] = '\0';
    }
  };
  
  /** virtual destructor */
  virtual ~ErrorHandler() {}

  /** 
  * Prints out an error
  * @param text The text to display
  */
  virtual void printError(const char* text) = 0;

  /**
  * Prints out a message
  * @param text The text to display
  */
  virtual void printMessage(const char* text) = 0;

  /** 
  * Formats a error message and calls the printError() function.
  * @param format Format string as used by printf defined in stdio.h.
  * @param ... See printf in stdio.h.
  */
  void error(const char* format, ...);

  /** 
  * Formats a message and calls the printMessage() function 
  * @param format Format string as used by printf defined in stdio.h.
  * @param ... See printf in stdio.h.
  */
  void message(const char* format, ...);

  /** if errors occurred */
  bool errorsOccurred;

private:
  /** a buffer for errors and debug messages */
  char messageBuffer[300];
};

/**
* @typedef TimeFunction
* A pointer to a function that returns the current system time.
*/
typedef unsigned (*TimeFunction)();

// If that macro is defined, the engine prints a lot of debug messages during initialization 
//#define _DO_DEBUG_INIT

/** Expressions inside that macro are only executed if XABSL_DEBUG_INIT is defined */
#ifdef _DO_DEBUG_INIT
#define XABSL_DEBUG_INIT(expression) expression
#else
#define XABSL_DEBUG_INIT(expression) /**/
#endif

} // namespace

#endif //__XabslTools_h_
