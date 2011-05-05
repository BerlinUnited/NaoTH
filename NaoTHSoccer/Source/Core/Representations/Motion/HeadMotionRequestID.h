/**
* @file HeadMotionRequestID.h
*
* @author <a href="mailto:welter@informatik.hu-berlin.de">Oliver Welter</a>
* Definition of HeadMotionReqeuest
*/

#ifndef __HeadMotionRequestID_h_
#define __HeadMotionRequestID_h_

#include <string>

/**
* This describes the HeadMotionRequestID
*/
class HeadMotionRequestID
{
public:
  /** constructor */
  HeadMotionRequestID(){}
  ~HeadMotionRequestID(){}

  /** ids for all motion types */
  enum HeadMotionID {
    hold,
    search,
    stabilize,
    look_straight_ahead,
    look_at_world_point,
    look_at_point_on_the_ground,

    goto_angle,
    look_at_point,
    numOfMotion
  };

  static string getName(HeadMotionID id)
  {
    switch(id)
    {
      case search: return "search";
      case goto_angle: return "goto_angle";
      case look_at_point: return "look_at_point";
      case hold: return "hold";
      case stabilize: return "stabilize";
      case look_straight_ahead: return "look_straight_ahead";
      case look_at_world_point: return "look_at_world_point";
      case look_at_point_on_the_ground: return "look_at_point_on_the_ground";
      default: return "unknown";
    }///end switch
  }///end getName
  

  /** return the head motion id reprented by the name */
  static HeadMotionID getId(const std::string& name)
  {
    for(int i = 0; i < numOfMotion; i++)
    {
      if(name == getName((HeadMotionID)i)) return (HeadMotionID)i;
    }//end for
    
    return numOfMotion;
  }//end getId
};

#endif // __HeadMotionRequestID_h_
