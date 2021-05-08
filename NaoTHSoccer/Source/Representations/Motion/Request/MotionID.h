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
#include <Tools/StringTools.h>

namespace motion
{

  /** ids for all motion types */
  enum MotionID
  {
    /* analytical */
    empty,
    init,
    dead,
    falling,
    sit,

    /* inverse kinematic */
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
    stand_up_from_back_arms_back,
    stand_up_from_side,
    
    block_middle,
    goalie_get_down,
    goalie_sit,
    goalie_sit_small,

    goalie_get_fast_down,

    fall_left,
    fall_right,

    kick_right,

    goalie_sit_block,
    goalie_sit_n_dive_left,
    goalie_sit_n_dive_right,

    jump_indicator_left,
    jump_indicator_right,

    // instantly load and run the play_editor_motionnet.mef
    // used by the motion editor
    play_editor_motionnet,

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
      case falling: return "falling";
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
      case stand_up_from_back_arms_back: return "stand_up_from_back_arms_back";
      case stand_up_from_side: return "stand_up_from_side";

      case block_middle: return "block_middle";
      case goalie_get_down: return "goalie_get_down";
      case goalie_sit: return "goalie_sit";
      case goalie_sit_small: return "goalie_sit_small";

      case goalie_get_fast_down: return "goalie_get_fast_down";

      case fall_left: return "fall_left";
      case fall_right: return "fall_right";

      case kick_right: return "kick_right";

      case goalie_sit_block: return "goalie_sit_block";
      case goalie_sit_n_dive_left: return "goalie_sit_n_dive_left";
      case goalie_sit_n_dive_right: return "goalie_sit_n_dive_right";

      case jump_indicator_left: return "jump_indicator_left";
      case jump_indicator_right: return "jump_indicator_right";

      case play_editor_motionnet: return "play_editor_motionnet";

      case num_of_motions: return "num_of_motion";
    }///end switch

    return "unknown " + naoth::StringTools::toStr(id);
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


