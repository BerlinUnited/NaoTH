/**
* @file Simulation.cpp
* @author <a href="mailto:schlottb@informatik.hu-berlin.de">Benjamin Schlotter</a>
* Implementation of class Simulation
*/

#include "Simulation.h"

using namespace naoth;
using namespace std;

  Simulation::Simulation()
  {
  DEBUG_REQUEST_REGISTER("Simulation:draw_one_action_point:global","draw_one_action_point:global", true);
  DEBUG_REQUEST_REGISTER("Simulation:draw_ball","draw_ball", true);
  DEBUG_REQUEST_REGISTER("Simulation:ActionTarget","ActionTarget", true);

  //actionRingBuffer.resize(ActionModel::numOfActions);
  //calculate the actions
  action_local.reserve(ActionModel::numOfActions);

  action_local.push_back(Action(ActionModel::none, Vector2d()));
  action_local.push_back(Action(ActionModel::kick_long, Vector2d(theParameters.action_long_kick_distance, 0))); // long
  action_local.push_back(Action(ActionModel::kick_short, Vector2d(theParameters.action_short_kick_distance, 0))); // short
  action_local.push_back(Action(ActionModel::sidekick_right, Vector2d(0, -theParameters.action_sidekick_distance))); // right
  action_local.push_back(Action(ActionModel::sidekick_left, Vector2d(0, theParameters.action_sidekick_distance))); // left
  }

  Simulation::~Simulation(){}

 void Simulation::execute()
 {
   if(!getBallModel().valid || getFrameInfo().getTimeInSeconds() >= getBallModel().frameInfoWhenBallWasSeen.getTimeInSeconds()+1){
     return;
   }
   else{
    Action& lonely_action = action_local[1];

    actionGlobal = calculateOneAction(lonely_action);

    DEBUG_REQUEST("Simulation:draw_one_action_point:global",
    FIELD_DRAWING_CONTEXT;
    PEN("000000", 1);

      CIRCLE( actionGlobal.x, actionGlobal.y, 50);

    );
   }
}//end execute


Vector2d Simulation::calculateOneAction(Action& lonely_action) const
{
  GoalModel::Goal oppGoalModel = getSelfLocGoalModel().getOppGoal(getCompassDirection(), getFieldInfo());
  Vector2d oppGoalPostLeftPreview = getMotionStatus().plannedMotion.hip / oppGoalModel.leftPost;
  Vector2d oppGoalPostRightPreview = getMotionStatus().plannedMotion.hip / oppGoalModel.rightPost;

  //the endpoints of our line are a shortened version of the goal line
  Vector2d leftEndpoint = oppGoalPostLeftPreview + Vector2d(getFieldInfo().goalpostRadius + getFieldInfo().ballRadius,0);
  Vector2d rightEndpoint = oppGoalPostRightPreview - Vector2d(getFieldInfo().goalpostRadius + getFieldInfo().ballRadius,0);

  // this is the goalline we are shooting for
  Math::LineSegment goalLinePreview(leftEndpoint, rightEndpoint);

  Vector2d ballRelativePreview = getBallModel().positionPreview;

  DEBUG_REQUEST("Simulation:draw_ball",
    FIELD_DRAWING_CONTEXT;
    PEN("FF0000", 1);
      Vector2d Ball = getRobotPose() *ballRelativePreview;
    CIRCLE( Ball.x, Ball.y, 50);

  );

  lonely_action.target = lonely_action.predict(ballRelativePreview, 0.1, Math::fromDegrees(5));

  DEBUG_REQUEST("Simulation:ActionTarget",
    FIELD_DRAWING_CONTEXT;
    PEN("0000FF", 1);

    Vector2d Ball = getRobotPose() *lonely_action.target;
    CIRCLE( Ball.x, Ball.y, 50);

  );
  //Math::LineSegment shootLine(ballRelativePreview, outsideField(lonely_action.target));//hmm
  Math::LineSegment shootLine(ballRelativePreview, lonely_action.target);

  Vector2d actionPoint;

  if(shootLine.intersect(goalLinePreview) && goalLinePreview.intersect(shootLine))
  {
    //actionGlobal = getRobotPose() * lonely_action.target;
    
    actionPoint = Vector2d(getFieldInfo().xPosOpponentGoal+200, 0.0);

  }
  else
  {
    actionPoint = getRobotPose() * lonely_action.target;
    actionPoint = outsideField(actionPoint);
  }

  return actionPoint;
}
  
  //correction of distance in percentage, angle in degrees
  Vector2d Simulation::Action::predict(const Vector2d& ball, double distance_variance, double angle_variance) const
  {
  double random_length = 2.0*Math::random()-1.0;
  double random_angle = 2.0*Math::random()-1.0;

  Vector2d noisyAction = actionVector + actionVector*(distance_variance*random_length);
  noisyAction.rotate(angle_variance*random_angle);

  return ball + noisyAction;
  }

  //calcualte according to the rules, without the roboter position, the ball position
  //if it goes outside the field
  Vector2d Simulation::outsideField(const Vector2d& globalPoint) const{ 
  Vector2d point = globalPoint;
   if(getFieldInfo().fieldRect.inside(point)){
     return point;
   }
   else{
     //an der linken Seite raus -> ein meter hinter roboter oder wo ins ausgeht ein meter hinter
     if(point.y  > getFieldInfo().yPosLeftSideline ){       
       if(point.x-1000 < getFieldInfo().xThrowInLineOwn) point.x = getFieldInfo().xThrowInLineOwn;
       else point.x -= 1000;
       return Vector2d(point.x, getFieldInfo().yThrowInLineLeft);//range check

     }
     //an der rechten Seite raus -> ein meter hinter roboter oder wo ins ausgeht ein meter hinter
     else if(point.y  < getFieldInfo().yPosRightSideline){
       if(point.x-1000 < getFieldInfo().xThrowInLineOwn) point.x = getFieldInfo().xThrowInLineOwn;
       else point.x -= 1000;
       return Vector2d(point.x, getFieldInfo().yThrowInLineRight);//range check
     }
     //hinten raus -> an der seite wo raus geht
     else if(point.x < getFieldInfo().xPosOwnGroundline){
       if(point.y < 0)return Vector2d(getFieldInfo().xThrowInLineOwn, getFieldInfo().yThrowInLineRight);//range check
       else return Vector2d(getFieldInfo().xThrowInLineOwn, getFieldInfo().yThrowInLineLeft); 
     }
     else{ //if(point.x > getFieldInfo().xPosOpponentGroundline){
       //vorne raus -> Ball einen Meter hinter Roboter mind anstoß höhe. jeweils seite wo ins ausgeht
       if(point.y < 0)return Vector2d(0, getFieldInfo().yThrowInLineRight);//range check
       else return Vector2d(0, getFieldInfo().yThrowInLineLeft); 
     }
   }
}

