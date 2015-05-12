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
  DEBUG_REQUEST_REGISTER("Simulation:draw_ball","draw_ball", false);
  DEBUG_REQUEST_REGISTER("Simulation:ActionTarget","ActionTarget", false);
  DEBUG_REQUEST_REGISTER("Simulation:draw_best_action","best action",false);
  //DEBUG_REQUEST_REGISTER("Simulation:draw_pessimistic_best_action","best pessimistic action",false);
  DEBUG_REQUEST_REGISTER("Simulation:GoalLinePreview","GoalLinePreview",false);
  DEBUG_REQUEST_REGISTER("Simulation:draw_potential_field","Draw Potential Field",false);
  DEBUG_REQUEST_REGISTER("Simulation:use_Parameters","use_Parameters",false);

  getDebugParameterList().add(&theParameters);

  //actionRingBuffer.resize(ActionModel::numOfActions);
  //calculate the actions  
  action_local.reserve(KickActionModel::numOfActions);

  action_local.push_back(Action(KickActionModel::none, Vector2d()));
  action_local.push_back(Action(KickActionModel::kick_long, Vector2d(theParameters.action_long_kick_distance, 0))); // long
  action_local.push_back(Action(KickActionModel::kick_short, Vector2d(theParameters.action_short_kick_distance, 0))); // short
  action_local.push_back(Action(KickActionModel::sidekick_right, Vector2d(0, -theParameters.action_sidekick_distance))); // right
  action_local.push_back(Action(KickActionModel::sidekick_left, Vector2d(0, theParameters.action_sidekick_distance))); // left
}

Simulation::~Simulation(){}

void Simulation::execute()
{
  DEBUG_REQUEST("Simulation:use_Parameters",
  action_local.clear();
  action_local.reserve(KickActionModel::numOfActions);

  action_local.push_back(Action(KickActionModel::none, Vector2d()));
  action_local.push_back(Action(KickActionModel::kick_long, Vector2d(theParameters.action_long_kick_distance, 0))); // long
  action_local.push_back(Action(KickActionModel::kick_short, Vector2d(theParameters.action_short_kick_distance, 0))); // short
  action_local.push_back(Action(KickActionModel::sidekick_right, Vector2d(0, -theParameters.action_sidekick_distance))); // right
  action_local.push_back(Action(KickActionModel::sidekick_left, Vector2d(0, theParameters.action_sidekick_distance))); // left
  );

  
  if(!getBallModel().valid || getFrameInfo().getTimeInSeconds() >= getBallModel().frameInfoWhenBallWasSeen.getTimeInSeconds()+1)
  {
    return;
  }
  else
  {
    int best_action = 0;
    
    for(size_t i=0; i<action_local.size(); i++)
    {
      // physics simulator
      std::vector<Vector2d> ballPositionResults;
      for(size_t j=0; j<30; j++)
      {
        const Vector2d& ballRelativePreview = getBallModel().positionPreview;
        Vector2d ballPositionResult = action_local[i].predict(ballRelativePreview, theParameters.distance_variance, theParameters.angle_variance);
        ballPositionResults.push_back(ballPositionResult);
      }
      
      // categorize positions
      std::vector<CategorizedBallPosition> categorizedBallPositionResults;
      for(std::vector<Vector2d>::iterator ballPosition = ballPositionResults.begin(); ballPosition != ballPositionResults.end(); ballPosition++)
      {
        BallPositionCategory category = this->categorizePosition(*ballPosition);
        CategorizedBallPosition categorizedBallPosition = CategorizedBallPosition(*ballPosition, category);
        categorizedBallPositionResults.push_back(categorizedBallPosition);
      }
    }

    getKickActionModel().myAction = action_local[best_action].id();

    DEBUG_REQUEST("Simulation:draw_best_action",
    {
      FIELD_DRAWING_CONTEXT;
      PEN("FF69B4", 7);
      Vector2d actionGlobal = action_local[best_action].target;
      FILLOVAL(actionGlobal.x, actionGlobal.y, 50,50);
    });

//  DEBUG_REQUEST("Simulation:draw_potential_field",
//     draw_potential_field();
//  );

 
  }
}//end execute

Simulation::BallPositionCategory Simulation::categorizePosition(const Vector2d& globalPoint) const
{ 
  Vector2d point = globalPoint;

  //Schusslinie
  Math::LineSegment shootLine(getBallModel().positionPreview, globalPoint);

  BallPositionCategory cat = INFIELD;
  if(getFieldInfo().fieldRect.inside(point)){
    return cat;
  } else
  {   
    //Opponent Groundline Out - Ball einen Meter hinter Roboter mind anstoß höhe. jeweils seite wo ins ausgeht
    if(point.x > getFieldInfo().xPosOpponentGroundline)
    {
      cat = OPPOUT;
    }
    //Own Groundline out -  an der seite wo raus geht
    else if(point.x < getFieldInfo().xPosOwnGroundline)
    {
      cat = OWNOUT;
    }
    //an der linken Seite raus -> ein meter hinter roboter oder wo ins ausgeht ein meter hinter
    else if(point.y > getFieldInfo().yPosLeftSideline )
    {  
      cat = LEFTOUT;
    }
    //an der rechten Seite raus -> ein meter hinter roboter oder wo ins ausgeht ein meter hinter
    else if(point.y < getFieldInfo().yPosRightSideline)
    { 
      cat = RIGHTOUT;
    }
    return cat;
  }
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

  Vector2d result = action.predict(ballRelativePreview, theParameters.distance_variance, theParameters.angle_variance);

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
  Vector2d goalDir = (oppGoalPostRightPreview - oppGoalPostLeftPreview).normalize();
  Vector2d leftEndpoint = oppGoalPostLeftPreview + goalDir*(getFieldInfo().goalpostRadius + getFieldInfo().ballRadius);
  Vector2d rightEndpoint = oppGoalPostRightPreview - goalDir*(getFieldInfo().goalpostRadius + getFieldInfo().ballRadius);

  // this is the goalline we are shooting for
  Math::LineSegment goalLinePreview(leftEndpoint, rightEndpoint);
  
  // note: the drawing is global
  DEBUG_REQUEST("Simulation:GoalLinePreview",
    FIELD_DRAWING_CONTEXT;
    PEN("000000", 1);
    Vector2d globalLeft = getRobotPose() * leftEndpoint;
    Vector2d globalRight = getRobotPose() * rightEndpoint;
    LINE(globalLeft.x,globalLeft.y,globalRight.x,globalRight.y);
  );

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
  Vector2d oppGoal(getFieldInfo().xPosOpponentGoal+200, 0.0);
  Vector2d oppDiff = oppGoal - a;

  double oppValueX = 0.1;
  double oppValueY = 1;
  MODIFY("Simulation:oppValueX", oppValueX);
  MODIFY("Simulation:oppValueY", oppValueY);
  double value_opp = oppValueX*oppDiff.x*oppDiff.x + oppValueY*oppDiff.y*oppDiff.y;
  
  //Vector2d ownGoal(getFieldInfo().xPosOwnGoal, 0.0);
  //Vector2d ownDiff = ownGoal - a;
  
  //double ownValueX = 0.01;
  //double ownValueY = 0.1;
  //MODIFY("Simulation:ownValueX", ownValueX);
  //MODIFY("Simulation:ownValueY", ownValueY);
  //double value_own = ownValueX*ownDiff.x*ownDiff.x + ownValueY*ownDiff.y*ownDiff.y;

  //return value_opp - value_own;
  return value_opp;
}

void Simulation::draw_potential_field() const
{
  static const int ySize = 20;
  static const int xSize = 30;
  double yWidth = 0.5*getFieldInfo().yFieldLength/(double)ySize;
  double xWidth = 0.5*getFieldInfo().xFieldLength/(double)xSize;

  FIELD_DRAWING_CONTEXT;
  Color white(0.0,0.0,1.0,0.5); // transparent
  Color black(1.0,0.0,0.0,0.5);

  // create new sample set
  std::vector<double> potential(xSize*ySize);
  int idx = 0;

  for (int x = 0; x < xSize; x++) {
    for (int y = 0; y < ySize; y++)
    {
      Vector2d point(xWidth*(2*x-xSize+1), yWidth*(2*y-ySize+1));
      potential[idx] = evaluateAction(point);
      idx++;
    }
  }
  
  double maxValue = 0;
  idx = 0;
  for (int x = 0; x < xSize; x++) {
    for (int y = 0; y < ySize; y++)
    {
      maxValue = max(maxValue, potential[idx++]);
    }
  }

  if(maxValue == 0) return;

  idx = 0;
  for (int x = 0; x < xSize; x++) {
    for (int y = 0; y < ySize; y++)
    {
      Vector2d point(xWidth*(2*x-xSize+1), yWidth*(2*y-ySize+1));
      
      double t = potential[idx++] / maxValue;
      Color color = black*t + white*(1-t);
      PEN(color, 20);
      FILLBOX(point.x - xWidth, point.y - yWidth, point.x+xWidth, point.y+yWidth);
    }
  }
}//end draw_closest_points
