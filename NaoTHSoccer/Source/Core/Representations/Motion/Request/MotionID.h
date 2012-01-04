/**
* @file MotionID.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* Definition of the class MotionID
*/

#ifndef __MotionID_h_
#define __MotionID_h_

#include <string>
#include <Tools/DataConversion.h>

namespace motion
{

  /** ids for all motion types */
  enum MotionID
  {
    empty,
    init,
    dead,
    
    /* inverse kinematic */
    sit,
    stand,
    
    walk,
    kick,
    dance,
    protect_falling,
    
    /* paralell kinematic */
    parallel_stepper,
    parallel_dance,
    

    /* motion net */
    stand_up_from_front,
    stand_up_from_back,
    
    block_middle,
    goalie_get_down,
    goalie_sit,
    goalie_sit_small,

    goalie_get_fast_down,

    fall_left,
    fall_right,

    kick_right,

    goalie_dive_right_test,
    goalie_dive_left_test,
    goalie_sit_block,
    goalie_sit_n_dive_left,
    goalie_sit_n_dive_right,
    goalie_sit_n_dive_left_test,
    goalie_sit_n_dive_right_test,

    num_of_motions //error value
  };

  /** return a string reprenting the id */
  static std::string getName(MotionID id)
  {
    switch(id)
    {
      case empty: return "empty";
      case init: return "init";
      case dead: return "dead";
      case sit: return "sit";

      case stand: return "stand";
      case walk: return "walk";
      case kick: return "kick";
      case dance: return "dance";
      case protect_falling: return "protect_falling";

      case parallel_stepper: return "parallel_stepper";
      case parallel_dance: return "parallel_dance";
      
      case stand_up_from_front: return "stand_up_from_front";
      case stand_up_from_back: return "stand_up_from_back";
      
      case block_middle: return "block_middle";
      case goalie_get_down: return "goalie_get_down";
      case goalie_sit: return "goalie_sit";
      case goalie_sit_small: return "goalie_sit_small";

      case goalie_get_fast_down: return "goalie_get_fast_down";

      case fall_left: return "fall_left";
      case fall_right: return "fall_right";

      case kick_right: return "kick_right";

      case goalie_dive_right_test: return "goalie_dive_right_test";
      case goalie_dive_left_test: return "goalie_dive_left_test";
      case goalie_sit_block: return "goalie_sit_block";
      case goalie_sit_n_dive_left: return "goalie_sit_n_dive_left";
      case goalie_sit_n_dive_right: return "goalie_sit_n_dive_right";
      case goalie_sit_n_dive_left_test: return "goalie_sit_n_dive_left_test";
      case goalie_sit_n_dive_right_test: return "goalie_sit_n_dive_right_test";

      case num_of_motions: return "num_of_motion";
    }///end switch

    return "unknown "+naoth::DataConversion::toStr(id);
  }///end getName

  /** return the motion id reprented by the name */
  MotionID getId(const std::string& name);


  enum State
  {
    running,
    stopped
  };

  std::string getName(State state);

} // namespace motion

#endif // __MotionRequestID_h_


