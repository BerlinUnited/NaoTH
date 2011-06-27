/**
* @file BodyState.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Definition of class BodyState
*/

#ifndef __BodyState_h_
#define __BodyState_h_

#include "Representations/Infrastructure/FrameInfo.h"
#include "Tools/DataStructures/Printable.h"

class BodyState : public naoth::Printable
{
public:
  BodyState()
    : 
  fall_down_state(undefined),
  standByLeftFoot(true),
  standByRightFoot(true)
  {}
  ~BodyState(){}


  /** Current state of the robot's body*/
  enum State
  { 
    undefined, 
    upright, 
    lying_on_front, 
    lying_on_back,
    lying_on_left_side,
    lying_on_right_side,
    numOfStates
  };

  static State getId(const std::string& name)
  {
    for(int i = 0; i < numOfStates; i++)
    {
      if(name == getName((State)i)) return (State)i;
    }//end for
    return numOfStates;
  }//end getId

  static std::string getName(State id)
  {
    switch(id)
    {
      case undefined: return "empty";
      case upright: return "upright";
      case lying_on_front: return "lying_on_front";
      case lying_on_back: return "lying_on_back";
      case lying_on_left_side: return "lying_on_left_side";
      case lying_on_right_side: return "lying_on_right_side";
      case numOfStates: return "numOfStates";
    }//end switch
    return "unknown";
  }//end getName

  State fall_down_state;
  unsigned int fall_down_state_time;

  bool standByLeftFoot;
  bool standByRightFoot;
  unsigned int foot_state_time;


  virtual void print(ostream& stream) const
  {
    stream << "fall_down_state = " << getName(fall_down_state) << endl;
    stream << "standByLeftFoot = " << standByLeftFoot << endl;
    stream << "standByRightFoot = " << standByRightFoot << endl;
  }//end print
};

#endif// __BodyState_h_
