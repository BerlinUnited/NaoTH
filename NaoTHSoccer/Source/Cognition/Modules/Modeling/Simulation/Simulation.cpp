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
  DEBUG_REQUEST_REGISTER("Simulation:goal_target", "draw goal target", false);
  DEBUG_REQUEST_REGISTER("Simulation:draw_action_points:best_action","best action",false);
  DEBUG_REQUEST_REGISTER("Simulation:draw_action_points:global","draw global action points", false);
  DEBUG_REQUEST_REGISTER("Simulation:draw_one_action_point:global","draw_one_action_point:global", false);
  DEBUG_REQUEST_REGISTER("Simulation:draw_potential_field:global","draw gobal potential field", false);
  DEBUG_REQUEST_REGISTER("Simulation:draw_lines:goal_line_global","goal line",false);
  DEBUG_REQUEST_REGISTER("Simulation:draw_lines:shootline_global","draw shootline if exist",false);

  actionRingBuffer.resize(ActionModel::numOfActions);
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
  // choose the goal model:
  // use the selflocator by default
  GoalModel::Goal oppGoalModel = getSelfLocGoalModel().getOppGoal(getCompassDirection(), getFieldInfo());

  //FIXME: frameWhenOpponentGoalWasSeen not supported by Model anymore!
  if ( getFrameInfo().getTimeSince(getLocalGoalModel().frameWhenOpponentGoalWasSeen.getTime()) < 10000
    //&& getLocalGoalModel().someGoalWasSeen
    && getLocalGoalModel().opponentGoalIsValid )
  {
    const GoalModel::Goal localOppGoalModel = getLocalGoalModel().getOppGoal(getCompassDirection(), getFieldInfo());
    if ( localOppGoalModel.calculateCenter().abs() < 4000 )
    {
      oppGoalModel = localOppGoalModel;
    }
  }

  //TODO Call calculateOneAction for all 5 actions
  Action& lonely_action = action_local[3];
  calculateOneAction(oppGoalModel, actionRingBuffer, lonely_action);

  DEBUG_REQUEST("Simulation:draw_potential_field:global",
    FIELD_DRAWING_CONTEXT;
    PEN("FF69B4", 5);
    const double stepX = getFieldInfo().xFieldLength/50.0;
    const double stepY = getFieldInfo().yFieldLength/50.0;

    Vector2d simulatedGlobalBall;
    for (simulatedGlobalBall.x = -getFieldInfo().xFieldLength/2.0;
         simulatedGlobalBall.x <= getFieldInfo().xFieldLength/2.0;
         simulatedGlobalBall.x += stepX)
    {
      for (simulatedGlobalBall.y = -getFieldInfo().yFieldLength/2.0;
           simulatedGlobalBall.y <= getFieldInfo().yFieldLength/2.0;
           simulatedGlobalBall.y += stepY)
      {
        // claculate the local attack direction for the current 
        // robots position and current obstacles
        Vector2d simulatedLocalBall = getRobotPose()/simulatedGlobalBall;
        Vector2d target = getGoalTarget(simulatedLocalBall, oppGoalModel.leftPost, oppGoalModel.rightPost);
        double f = calculatePotential(simulatedLocalBall, target);
        double rad = 5+f*50;

        CIRCLE(simulatedGlobalBall.x, simulatedGlobalBall.y, rad);
      }
    }
  );
}//end execute


  void Simulation::calculateOneAction(GoalModel::Goal &oppGoalModel, std::vector<RingBufferWithSum<double, 30> > &buffer, Action& lonely_action) const
{
  // reset our Model and get Ball/Goal
  //getActionModel().myAction = ActionModel::none;

  Vector2d ballRelativePreview = getBallModel().positionPreview;

  Vector2d oppGoalPostLeftPreview = getMotionStatus().plannedMotion.hip / oppGoalModel.leftPost;
  Vector2d oppGoalPostRightPreview = getMotionStatus().plannedMotion.hip / oppGoalModel.rightPost;

  // the endpoints of our line are a shortened version of the goal line
  Vector2d leftEndpoint = oppGoalPostLeftPreview + Vector2d(getFieldInfo().goalpostRadius + getFieldInfo().ballRadius,0);
  Vector2d rightEndpoint = oppGoalPostRightPreview - Vector2d(getFieldInfo().goalpostRadius + getFieldInfo().ballRadius,0);

  // this is the goalline we are shooting for
  Math::LineSegment goalLinePreview(leftEndpoint, rightEndpoint);

    Action& action = lonely_action;

    action.target = action.predict(ballRelativePreview, 0.1, Math::fromDegrees(5));

    // calculate the target point to play the ball to based on the 
    // goal model and the ball model 
	  Vector2d targetPointPreview = getGoalTarget(outsideField(action.target), oppGoalPostLeftPreview, oppGoalPostRightPreview);

    Math::LineSegment shootLine(ballRelativePreview, outsideField(action.target));
    // check if the action scores a goal
    if(shootLine.intersect(goalLinePreview) && goalLinePreview.intersect(shootLine)) {
      buffer[action.id()].add(1.0);
    }
	  else{
	    buffer[action.id()].add(calculatePotential(outsideField(action.target), targetPointPreview));
	  }

  DEBUG_REQUEST("Simulation:draw_one_action_point:global",
    FIELD_DRAWING_CONTEXT;
    PEN("000000", 1);
       const Action& action = lonely_action;
       Vector2d actionGlobal = getRobotPose() * lonely_action.target;
       CIRCLE( actionGlobal.x, actionGlobal.y, 50);
       //Keine Bewertungen an dieser Stelle
       //TEXT_DRAWING( actionGlobal.x+100,  actionGlobal.y+100, action.potential);
       //TEXT_DRAWING( actionGlobal.x+100,  actionGlobal.y+200, action.goodness);
    //}
  );
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

  Vector2d Simulation::getGoalTarget(const Vector2d& point, const Vector2d& leftPost, const Vector2d& rightPost) const
{

  // normalized vector from left post to the right
  const Vector2d leftToRight((rightPost - leftPost).normalize());

  // a normal vector ponting from the goal towards the field
  Vector2d goalNormal(leftToRight);
  goalNormal.rotateRight();


  // the endpoints of our line are a shortened version of the goal line
  Vector2d leftEndpoint = leftPost + leftToRight * theParameters.goal_post_offset;
  Vector2d rightEndpoint = rightPost - leftToRight * theParameters.goal_post_offset;

  // this is the goalline we are shooting for
  Math::LineSegment goalLine(leftEndpoint, rightEndpoint);

  // project the point on the goal line
  Vector2d target = goalLine.projection(point);

  // this is the cos of the angle between the vectors (leftEndpoint-point) and (rightEndpoint-point)
  // simple linear algebra: <l-p,r-p>/(||l-p||*||r-p||)
  double goalAngleCos = (leftPost-point).normalize()*(rightPost-point).normalize();

  // assymetric quadratic scale
  // goalAngleCos = -1 => t = -goalLineOffsetBack
  // goalAngleCos =  1 => t =  goalLineOffsetFront;
  double c = (theParameters.goal_line_offset_front + theParameters.goal_line_offset_back)*0.5;
  double v = (theParameters.goal_line_offset_front - theParameters.goal_line_offset_back)*0.5;
  double t = goalAngleCos*(goalAngleCos*c + v);

  // move the target depending on the goal opening angle
  target = target + goalNormal.normalize(t);


  DEBUG_REQUEST("Simulation:goal_target",
    FIELD_DRAWING_CONTEXT;
    PEN("0000FF", 10);
    CIRCLE((getRobotPose()*leftEndpoint).x, (getRobotPose()*leftEndpoint).y, 100);
    CIRCLE((getRobotPose()*rightEndpoint).x, (getRobotPose()*rightEndpoint).y, 100);
  );
 
  return target;
}//end getGoalTarget

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

  double Simulation::calculatePotential( const Vector2d& point, const Vector2d& targetPoint) const
{
  return globalAttractorPotential(targetPoint, point);
}

  double Simulation::globalAttractorPotential(const Vector2d& p, const Vector2d& x)const
{
  const double alpha = theParameters.goal_attractor_strength; //-0.001;
  return exp(alpha * (p - x).abs());
}









