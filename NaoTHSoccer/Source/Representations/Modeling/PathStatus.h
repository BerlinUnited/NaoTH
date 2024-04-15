/**
 * @file PathStatus.h
 *
 * @author <a href="mailto:mellmann@hu-berlin.de">Heinrich Mellmann</a>
 */

#ifndef PATH_STATUS_H
#define PATH_STATUS_H

#include <Tools/DataStructures/Printable.h>
#include <Tools/Debug/NaoTHAssert.h>

class PathStatus : public naoth::Printable
{
public:
  PathStatus() :
     kick_executed(false)
  {}
  ~PathStatus() {}

  bool kick_executed;

  virtual void print(std::ostream& stream) const
  {  
    stream << "kick_executed = " << kick_executed << std::endl;
  }
};


#endif /* PATH_STATUS_H */
