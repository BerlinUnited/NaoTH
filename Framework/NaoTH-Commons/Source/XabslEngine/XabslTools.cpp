/**
* @file XabslTools.cpp
*
* Implementation of several helper classes for the Engine.
*
* @author <a href="http://www.martin-loetzsch.de">Martin Loetzsch</a>
*/

#include "XabslTools.h"
#include <stdarg.h>
#include <string.h>
#include <stdio.h>

namespace xabsl 
{

void ErrorHandler::error(const char* format, ...)
{
  // get the argument list
  va_list args;
  va_start(args,format);
  
  // print the expression to the buffer
  vsprintf(messageBuffer,format,args); 
  
  printError(messageBuffer);
  
  // delete the argument list
  va_end(args); 
  errorsOccurred = true;
}

void ErrorHandler::message(const char* format, ...)
{
  // get the argument list
  va_list args;
  va_start(args,format);
  
  // print the expression to the buffer
  vsprintf(messageBuffer,format,args); 
  
  printMessage(messageBuffer);
  
  // delete the argument list
  va_end(args); 
}

} // namespace

