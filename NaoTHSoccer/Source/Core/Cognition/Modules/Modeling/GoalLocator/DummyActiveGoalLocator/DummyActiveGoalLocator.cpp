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

  getLocalGoalModel().someGoalWasSeen = getGoalPercept().getNumberOfSeenPosts() > 0; //getSensingGoalModel().someGoalWasSeen;
  
  // opp goal is in front of me
  const GoalModel::Goal& oppGoal = getSelfLocGoalModel().getOppGoal(getCompassDirection(), getFieldInfo());
  if(((oppGoal.leftPost+oppGoal.leftPost)*0.5).x > 0)
    getLocalGoalModel().opponentGoalIsValid = true;
  else
    getLocalGoalModel().ownGoalIsValid = true;


  // copy the self loc goal
  getLocalGoalModel().goal = getSelfLocGoalModel().goal;
  
  //frame Info when goal was seen not useful! New: some_goal_was seen
  if(getGoalPercept().getNumberOfSeenPosts() > 0)
  {
    getLocalGoalModel().goal.frameInfoWhenGoalLastSeen = getFrameInfo();

    if(getLocalGoalModel().opponentGoalIsValid)
    {
      getLocalGoalModel().frameWhenOpponentGoalWasSeen = getFrameInfo();
    }
    else
    {
      getLocalGoalModel().frameWhenOwnGoalWasSeen = getFrameInfo();
    }
  }

  DEBUG_REQUEST("DummyActiveGoalLocator:draw_goal_model",
    FIELD_DRAWING_CONTEXT;
    if(getLocalGoalModel().opponentGoalIsValid)
      PEN("000000", 50);
    else
      PEN("FFFFFF", 50);

    CIRCLE(getLocalGoalModel().goal.leftPost.x, getLocalGoalModel().goal.leftPost.y, 50);
    CIRCLE(getLocalGoalModel().goal.rightPost.x, getLocalGoalModel().goal.rightPost.y, 50);
    LINE(getLocalGoalModel().goal.rightPost.x, getLocalGoalModel().goal.rightPost.y, getLocalGoalModel().goal.leftPost.x, getLocalGoalModel().goal.leftPost.y);
  );
}//end execute
