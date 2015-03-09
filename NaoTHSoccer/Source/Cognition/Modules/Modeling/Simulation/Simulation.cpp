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
  DEBUG_REQUEST_REGISTER("Simulation:draw_one_action_point:global","draw_one_action_point:global", false);

 
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
  Action& lonely_action = action_local[1];

  actionGlobal = calculateOneAction(lonely_action);

    DEBUG_REQUEST("Simulation:draw_one_action_point:global",
    FIELD_DRAWING_CONTEXT;
    PEN("000000", 1);

        CIRCLE( actionGlobal.x, actionGlobal.y, 50);

  );
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

  lonely_action.target = lonely_action.predict(ballRelativePreview, 0.1, Math::fromDegrees(5));

  Math::LineSegment shootLine(ballRelativePreview, outsideField(lonely_action.target));
  
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

     if(point.y  > getFieldInfo().yPosLeftSideline ){
       //an der linken Seite raus -> ein meter hinter roboter oder wo ins ausgeht ein meter hinter
       //TODO symbols, nur für 7,9 Feld
       //TODO Throw-In line symbol
       if( -3500 > point.x-1000) point.x = -3500;
       else if(  3500 < point.x-1000) point.x =  3500;
       else point.x -= 1000;
       return Vector2d(point.x, 2600);//range check

     }
     else if(point.y  < getFieldInfo().yPosRightSideline){
       //an der rechten Seite raus -> ein meter hinter roboter oder wo ins ausgeht ein meter hinter
       if( -3500 > point.x-1000) point.x = -3500;
       else if(  3500 < point.x-1000) point.x =  3500;
       else point.x -= 1000;
       return Vector2d(point.x, -2600);//range check
     }
     else if(point.x < getFieldInfo().xPosOwnGroundline){
       //hinten raus -> an der seite wo raus geht
       if(point.y < 0)return Vector2d(-3500, -2600);//range check
       else return Vector2d(-3500, 2600); 
     }
     else{ //if(point.x > getFieldInfo().xPosOpponentGroundline){
       //vorne raus -> Ball einen Meter hinter Roboter mind anstoß höhe. jeweils seite wo ins ausgeht
       if(point.y < 0)return Vector2d(0, -2600);//range check
       else return Vector2d(0, 2600); 
     }
   }
}

