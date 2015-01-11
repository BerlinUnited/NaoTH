/**
 * @file DummyActiveGoalLocator.h
 *
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
 * Implementation of class DummyActiveGoalLocator
 */

#include "DummyActiveGoalLocator.h"

//for MODIFY
#include "Tools/Debug/DebugRequest.h"


DummyActiveGoalLocator::DummyActiveGoalLocator() 
{
  DEBUG_REQUEST_REGISTER("DummyActiveGoalLocator:draw_goal_model", "", false);
} //Constructor

void DummyActiveGoalLocator::execute() 
{
  // reset
  getLocalGoalModel().opponentGoalIsValid = false;
  getLocalGoalModel().ownGoalIsValid = false;

  getLocalGoalModel().someGoalWasSeen = 
    getGoalPercept().getNumberOfSeenPosts() > 0 ||
    getGoalPerceptTop().getNumberOfSeenPosts() > 0; //getSensingGoalModel().someGoalWasSeen;


  if(getLocalGoalModel().someGoalWasSeen) {
    if(getGoalPercept().getNumberOfSeenPosts() > 0) {
      Vector2d v(1.0,0);
      v.rotate(getGoalPercept().getPost(0).position.angle() + getCompassDirection().angle); // vector towards the goal
      getLocalGoalModel().ownGoalIsValid = v.x < 0;
    } else if(getGoalPerceptTop().getNumberOfSeenPosts() > 0) {
      Vector2d v(1.0,0);
      v.rotate(getGoalPerceptTop().getPost(0).position.angle() + getCompassDirection().angle); // vector towards the goal
      getLocalGoalModel().ownGoalIsValid = v.x < 0;
    }

    getLocalGoalModel().opponentGoalIsValid = !getLocalGoalModel().ownGoalIsValid;
  }


  // copy the self loc goal
  if(getLocalGoalModel().opponentGoalIsValid) {
    const GoalModel::Goal& oppGoal = getSelfLocGoalModel().getOppGoal(getCompassDirection(), getFieldInfo());
    getLocalGoalModel().goal = oppGoal;
  } else {
    const GoalModel::Goal& ownGoal = getSelfLocGoalModel().getOwnGoal(getCompassDirection(), getFieldInfo());
    getLocalGoalModel().goal = ownGoal;
  }

  //frame Info when goal was seen not useful! New: some_goal_was seen
  if(getGoalPercept().getNumberOfSeenPosts() > 0 || getGoalPerceptTop().getNumberOfSeenPosts())
  {
    getLocalGoalModel().goal.frameInfoWhenGoalLastSeen = getFrameInfo();

    if(getLocalGoalModel().opponentGoalIsValid) {
      getLocalGoalModel().frameWhenOpponentGoalWasSeen = getFrameInfo();
    } else {
      getLocalGoalModel().frameWhenOwnGoalWasSeen = getFrameInfo();
    }
  }

  DEBUG_REQUEST("DummyActiveGoalLocator:draw_goal_model",
    FIELD_DRAWING_CONTEXT;
    if(getLocalGoalModel().opponentGoalIsValid) {
      PEN("000000", 50);
    } else {
      PEN("FFFFFF", 50);
    }

    CIRCLE(getLocalGoalModel().goal.leftPost.x, getLocalGoalModel().goal.leftPost.y, 50);
    CIRCLE(getLocalGoalModel().goal.rightPost.x, getLocalGoalModel().goal.rightPost.y, 50);
    LINE(getLocalGoalModel().goal.rightPost.x, getLocalGoalModel().goal.rightPost.y, getLocalGoalModel().goal.leftPost.x, getLocalGoalModel().goal.leftPost.y);
  );
}//end execute
