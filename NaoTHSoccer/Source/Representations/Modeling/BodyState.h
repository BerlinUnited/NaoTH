/**
* @file BodyState.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Definition of class BodyState
*/

#ifndef _BodyState_h_
#define _BodyState_h_

#include "Tools/DataStructures/Printable.h"
#include "Tools/DataStructures/Serializer.h"

class BodyState : public naoth::Printable
{
public:
  BodyState()
    : 
    fall_down_state(undefined),
    fall_down_state_time(0),
    readyToWalk(false),
    standByLeftFoot(true),
    standByRightFoot(true),
    foot_state_time(0),
    temperatureLeftLeg(0),
    temperatureRightLeg(0),
    isLiftedUp(false),
    isDischarging(true),
    isCharging(false),
    batteryCharge(0.0)
  {}

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
    for(int i = 0; i < numOfStates; i++) {
      if(name == getName((State)i)) return (State)i;
    }
    return numOfStates;
  }

  static std::string getName(State id)
  {
    switch(id)
    {
      case undefined: return "undefined";
      case upright: return "upright";
      case lying_on_front: return "lying_on_front";
      case lying_on_back: return "lying_on_back";
      case lying_on_left_side: return "lying_on_left_side";
      case lying_on_right_side: return "lying_on_right_side";
      case numOfStates: return "numOfStates";
    }
    return "unknown";
  }//end getName

  // indicates whether the robot is upright etc.
  // TODO: this should be renamed to koerper_lage or so
  State fall_down_state;
  // timestamp when the fall down state changed last time
  unsigned int fall_down_state_time;
  /** indicates, whether the robot is ready to walk */
  bool readyToWalk;

  // indicates whether the particular foot is on the ground
  bool standByLeftFoot;
  bool standByRightFoot;
  // timestamp when any of the feet states changed last time
  unsigned int foot_state_time;

  // the maximal temperature for each leg
  double temperatureLeftLeg;
  double temperatureRightLeg;

  //Is the body lifted up
  bool isLiftedUp;

  // TODO: we need a more clear representation of the charging state.
  // IDEA (needs discussion) Perhaps, this might be better represented by an enum, e.g.,
  /* 
  enum ChargingState {
    CHARGED,    // we see low charging or discharging + battery charge is 100% => battery is fully charged + cable is plugged in
    ERROR,      // we see low charging or discharging => cable is not plugged in incorrectly
    CHARGING,   // we see expected charging rate => cable is plugged in
    DISCHARGING // we see expected discharging rate => cable not plugged in
  };
  */
  
  // modeled battery state
  bool isDischarging;
  bool isCharging;

  // filtered state of the battery
  double batteryCharge;

  virtual void print(std::ostream& stream) const
  {
      stream << "fall_down_state = " << getName(fall_down_state) << std::endl;
      stream << "fall_down_state_time = " << fall_down_state_time << std::endl;
      stream << "readyToWalk = " << readyToWalk << std::endl;
      stream << "standByLeftFoot = " << standByLeftFoot << std::endl;
      stream << "standByRightFoot = " << standByRightFoot << std::endl;
      stream << "foot_state_time = " << foot_state_time << std::endl;
      stream << "isLiftedUp = " << isLiftedUp << std::endl;
      stream << "isDischarging = " << isDischarging << std::endl;
      stream << "isCharging = " << isCharging << std::endl;
      stream << "batteryCharge (filtered) = " << batteryCharge << std::endl;
  }

};

namespace naoth
{
    template<>
    class Serializer<BodyState>
    {
    public:
        static void serialize(const BodyState& representation, std::ostream& stream);
        static void deserialize(std::istream& stream, BodyState& representation);
    };
}


#endif// __BodyState_h_
