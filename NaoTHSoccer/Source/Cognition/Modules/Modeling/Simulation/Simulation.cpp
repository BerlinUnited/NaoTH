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
  action_local.reserve(ActionNew::numOfActions);

  action_local.push_back(Action(ActionNew::none, Vector2d()));
  action_local.push_back(Action(ActionNew::kick_long, Vector2d(theParameters.action_long_kick_distance, 0))); // long
  action_local.push_back(Action(ActionNew::kick_short, Vector2d(theParameters.action_short_kick_distance, 0))); // short
  action_local.push_back(Action(ActionNew::sidekick_right, Vector2d(0, -theParameters.action_sidekick_distance))); // right
  action_local.push_back(Action(ActionNew::sidekick_left, Vector2d(0, theParameters.action_sidekick_distance))); // left

  actionRingBuffer.resize(ActionNew::numOfActions);
}

Simulation::~Simulation(){}

void Simulation::execute()
{
  if(!getBallModel().valid || getFrameInfo().getTimeInSeconds() >= getBallModel().frameInfoWhenBallWasSeen.getTimeInSeconds()+1)
  {
    return;
  }
  else
  {
    int best_action = 0;

    for(size_t i=0; i<actionRingBuffer.size(); i++)
    {
      simulate(action_local[i], actionRingBuffer[i]);

      if(action_local[i].potential < action_local[best_action].potential)
      {	
        best_action = i;
      }
    }

    getActionNew().myAction = action_local[best_action].id();
  }
}//end execute

void Simulation::simulate(Simulation::Action& action, RingBufferWithSum<double, 30>& actionRingBuffer)const
{

  Vector2d ballPositionResult = calculateOneAction(action);

  DEBUG_REQUEST("Simulation:draw_one_action_point:global",
    FIELD_DRAWING_CONTEXT;
    PEN("000000", 1);
    CIRCLE( ballPositionResult.x, ballPositionResult.y, 50);
  );

  double v = evaluateAction(ballPositionResult);

  actionRingBuffer.add(v);

  action.potential = actionRingBuffer.getAverage();

  //if there is big gap between our values(average and median), we know it is not good
  //action.goodness = actionRingBuffer.getAverage()/actionRingBuffer.getMedian();
}

Vector2d Simulation::calculateOneAction(const Action& action) const
{
  // STEP 1: predict the action outcome
  const Vector2d& ballRelativePreview = getBallModel().positionPreview;

  DEBUG_REQUEST("Simulation:draw_ball",
    FIELD_DRAWING_CONTEXT;
    PEN("FF0000", 1);
    Vector2d ball = getRobotPose() * getBallModel().positionPreview;
    CIRCLE( ball.x, ball.y, 50);
  );

  Vector2d result = action.predict(ballRelativePreview, 0.1, Math::fromDegrees(5));

  DEBUG_REQUEST("Simulation:ActionTarget",
    FIELD_DRAWING_CONTEXT;
    PEN("0000FF", 1);
    Vector2d ball = getRobotPose() * result;
    CIRCLE( ball.x, ball.y, 50);
  );


  // STEP 2: calculate the goal line
  GoalModel::Goal oppGoalModel = getSelfLocGoalModel().getOppGoal(getCompassDirection(), getFieldInfo());
  Vector2d oppGoalPostLeftPreview = getMotionStatus().plannedMotion.hip / oppGoalModel.leftPost;
  Vector2d oppGoalPostRightPreview = getMotionStatus().plannedMotion.hip / oppGoalModel.rightPost;

  //the endpoints of our line are a shortened version of the goal line
  Vector2d leftEndpoint = oppGoalPostLeftPreview + Vector2d(getFieldInfo().goalpostRadius + getFieldInfo().ballRadius,0);
  Vector2d rightEndpoint = oppGoalPostRightPreview - Vector2d(getFieldInfo().goalpostRadius + getFieldInfo().ballRadius,0);

  // this is the goalline we are shooting for
  Math::LineSegment goalLinePreview(leftEndpoint, rightEndpoint);


  // STEP 3: estimate external factors (shooting a goal, ball moved by referee)
  //Math::LineSegment shootLine(ballRelativePreview, outsideField(lonely_action.target));//hmm
  Math::LineSegment shootLine(ballRelativePreview, result);

  Vector2d resultingBallPosition;

  if(shootLine.intersect(goalLinePreview) && goalLinePreview.intersect(shootLine)) {
    resultingBallPosition = Vector2d(getFieldInfo().xPosOpponentGoal+200, 0.0);
  } else {
    resultingBallPosition = outsideField(getRobotPose() * result);
  }

  return resultingBallPosition;
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
Vector2d Simulation::outsideField(const Vector2d& globalPoint) const
{ 
  Vector2d point = globalPoint;

  //Schusslinie
  Math::LineSegment shootLine(getBallModel().positionPreview, globalPoint);

  if(getFieldInfo().fieldRect.inside(point)){
    return point;
  }
  else
      //Nach Prioritäten geordnet - zuerst die Regeln mit dem möglichst schlimmsten Resultat
  {   //Opponent Groundline Out - Ball einen Meter hinter Roboter mind anstoß höhe. jeweils seite wo ins ausgeht
      if(point.x > getFieldInfo().xPosOpponentGroundline)
      { 
        Vector2d OppOutPoint = getFieldInfo().oppLineSegment.point(getFieldInfo().oppLineSegment.intersection(shootLine));
        if(OppOutPoint.y < 0)
        {
          return Vector2d(0, getFieldInfo().yThrowInLineRight);//range check
        } else
        {
          return Vector2d(0, getFieldInfo().yThrowInLineLeft); 
        }
      }
      //Own Groundline out -  an der seite wo raus geht
      else if(point.x < getFieldInfo().xPosOwnGroundline)
      {
        Vector2d OwnOutPoint = getFieldInfo().ownLineSegment.point(getFieldInfo().ownLineSegment.intersection(shootLine));
        if(OwnOutPoint.y < 0)
        {
          return Vector2d(getFieldInfo().xThrowInLineOwn, getFieldInfo().yThrowInLineRight);//range check
        } else
        { 
          return Vector2d(getFieldInfo().xThrowInLineOwn, getFieldInfo().yThrowInLineLeft); 
        }
      }
      //an der linken Seite raus -> ein meter hinter roboter oder wo ins ausgeht ein meter hinter
      else if(point.y > getFieldInfo().yPosLeftSideline )
      {  
        Vector2d leftOutPoint = getFieldInfo().leftLineSegment.point(getFieldInfo().leftLineSegment.intersection(shootLine));
        point.x = min(leftOutPoint.x,getRobotPose().translation.x);

        if(point.x-1000 < getFieldInfo().xThrowInLineOwn)
        { 
          point.x = getFieldInfo().xThrowInLineOwn;
        } else
        { 
          point.x -= 1000;
        }
        return Vector2d(point.x, getFieldInfo().yThrowInLineLeft); //range check
      }
      //an der rechten Seite raus -> ein meter hinter roboter oder wo ins ausgeht ein meter hinter
      else //(point.y < getFieldInfo().yPosRightSideline)
      { 
        Vector2d rightOutPoint = getFieldInfo().rightLineSegment.point(getFieldInfo().rightLineSegment.intersection(shootLine));
        point.x = min(rightOutPoint.x,getRobotPose().translation.x);

        if(point.x-1000 < getFieldInfo().xThrowInLineOwn)
        {
        point.x = getFieldInfo().xThrowInLineOwn;
        } else
        { 
         point.x -= 1000;
        }
      return Vector2d(point.x, getFieldInfo().yThrowInLineRight);//range check
      }
  }
}

double Simulation::evaluateAction(const Vector2d& a) const{

  return (Vector2d(getFieldInfo().xPosOpponentGoal+200, 0.0)-a).abs();  
}
