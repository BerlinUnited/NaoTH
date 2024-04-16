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
    kick_executed(false),
    turn_around_ball_2_target_reached(false)
  {}
  ~PathStatus() {}

  bool kick_executed;

  // TODO: this is a specific flag now. Make it more general for all paths'.
  bool turn_around_ball_2_target_reached;


  virtual void print(std::ostream& stream) const
  {  
    stream << "kick_executed = " << kick_executed << std::endl;
    stream << "turn_around_ball_2_target_reached = " << turn_around_ball_2_target_reached << std::endl;
  }
};


#endif /* PATH_STATUS_H */
