/* 
 * File:   LogfileEntry.h
 * Author: thomas
 *
 * Created on 18. November 2008, 11:29
 */

#ifndef _LOGFILEENTRY_H
#define _LOGFILEENTRY_H

#include <string>
#include <sstream>

class LogfileEntry
{
public:
  
  LogfileEntry()
    : 
    frameNumber(0), 
    name(""),
    data(std::ios::in | std::ios::out | std::ios::binary)
  {
  }
  

  unsigned int frameNumber;
  std::string name;
  std::stringstream data;
  
};

#endif  /* _LOGFILEENTRY_H */

