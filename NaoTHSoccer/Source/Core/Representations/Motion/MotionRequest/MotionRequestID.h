/**
* @file MotionRequestID.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Definition of the class MotionRequestID
*/

#ifndef __MotionRequestID_h_
#define __MotionRequestID_h_

#include <string>

/**
* This describes the MotionRequestID
*/
class MotionRequestID
{
public:

   /** constructor */
  MotionRequestID(){}
  ~MotionRequestID(){}

  /** ids for all motion types */
  enum MotionID {
    
    empty,
    init,
    dead,
    
    /* inverse kinematic */
    stand,
    sit_stable,
    walk,
    kick,
    grasp,
    check_reactivity,
    dance,

    /* motion net */
    stand_up_from_front,
    stand_up_from_back,
    block_left,
    block_right,
    block_middle,
    evolution_motion, //motionrequest for evolution (Martin)
    //pre_grasping,

    /* debug */
    goto_pose_directly,
    force_joints,
    test,
    play_editor_motionnet,
    knead_mode,
    reachability_grid_experiment,

    goalie_get_down,
    goalie_sit,
    goalie_get_up,

    goalie_get_fast_down,
    goalie_sit_small,
    goalie_get_fast_up,

    goalie_dive_left,
    goalie_dive_right,

    goalie_dive_left_test,
    goalie_dive_right_test,

    fall_left,
    fall_right,

    // test of the parallel_kinematic :)
    parallel_dance,
    parallel_stepper,
    sensor_stepper,

    // neural motions
    neural_walk,

    kick_right,

    numOfMotion //error value
  };
  
  /** return the motion id reprented by the name */
  static MotionID getId(const std::string& name)
  {
    for(int i = 0; i < numOfMotion; i++)
    {
      if(name == getName((MotionID)i)) return (MotionID)i;
    }//end for
    
    return numOfMotion;
  }//end motionIDFromName


  /** return a string reprenting the id */
  static std::string getName(MotionID id)
  {
    switch(id)
    {
      case empty: return "empty";
      case init: return "init";
      case dead: return "dead";
      case stand: return "stand";

      case walk: return "walk";
      case kick: return "kick";
      case dance: return "dance";
      case check_reactivity: return "check_reactivity";

//      case pre_grasping:    return "pre_grasping";
      case grasp: return "grasping";

      case stand_up_from_front: return "stand_up_from_front";
      case stand_up_from_back: return "stand_up_from_back";
      case block_left: return "block_left";
      case block_right: return "block_right";
      case block_middle: return "block_middle";
      case evolution_motion: return "evolution_motion"; //(Martin)

      case goto_pose_directly: return "goto_pose_directly";
      case force_joints: return "force_joints";
      case test: return "test";
      case play_editor_motionnet: return "play_editor_motionnet";
      case knead_mode: return "knead_mode";
      case reachability_grid_experiment: return "reachability_grid_experiment";

      case goalie_get_down: return "goalie_get_down";
      case goalie_sit: return "goalie_sit";
      case goalie_get_up: return "goalie_get_up";

      case goalie_get_fast_down: return "goalie_get_fast_down";
      case goalie_sit_small: return "goalie_sit_small";
      case goalie_get_fast_up: return "goalie_get_fast_up";

      case goalie_dive_left: return "goalie_dive_left";
      case goalie_dive_right: return "goalie_dive_right";

      case goalie_dive_left_test: return "goalie_dive_left_test";
      case goalie_dive_right_test: return "goalie_dive_right_test";

      case fall_left: return "fall_left";
      case fall_right: return "fall_right";

      case sit_stable: return "sit_stable";

      case parallel_dance: return "parallel_dance";
      case parallel_stepper: return "parallel_stepper";
      case sensor_stepper: return "sensor_stepper";

      case neural_walk: return "neural_walk";

      case kick_right: return "kick_right";

      case numOfMotion: return "numOfMotion";
    }///end switch

    return "unkown";
  }///end getName
};

#endif // __MotionRequestID_h_


