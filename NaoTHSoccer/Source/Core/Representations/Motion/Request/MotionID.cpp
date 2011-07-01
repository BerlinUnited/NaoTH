/**
* @file MotionID.cpp
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
*/

#include "MotionID.h"

namespace motion
{

MotionID getId(const std::string& name)
{
  for(int i = 0; i < num_of_motions; i++)
  {
    if(name == getName((MotionID)i)) return (MotionID)i;
  }//end for

  return num_of_motions;
}//end motionIDFromName

std::string getName(State state)
{
  switch(state)
  {
  case running: return "running";
  case stopped: return "stopped";
  default: return "unknown";
  }

  return "unknown";
}

} // namespace motion
