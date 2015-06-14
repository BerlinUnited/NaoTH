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
  DEBUG_REQUEST_REGISTER("Simulation:draw_ball","draw_ball", false);
  DEBUG_REQUEST_REGISTER("Simulation:ActionTarget","ActionTarget", false);
  DEBUG_REQUEST_REGISTER("Simulation:draw_best_action","best action",false);
  DEBUG_REQUEST_REGISTER("Simulation:draw_potential_field","Draw Potential Field",false);
  DEBUG_REQUEST_REGISTER("Simulation:use_Parameters","use_Parameters",false);

  getDebugParameterList().add(&theParameters);

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

  //Proceed with Calculations only if ball is seen in the last 1 second
  if(!getBallModel().valid || getFrameInfo().getTimeInSeconds() >= getBallModel().frameInfoWhenBallWasSeen.getTimeInSeconds() + 1)
  {
    return;
  }
 
  DEBUG_REQUEST("Simulation:draw_ball",
    FIELD_DRAWING_CONTEXT;
    PEN("FF0000", 1);
    Vector2d ball = getRobotPose() * getBallModel().positionPreview;
    CIRCLE( ball.x, ball.y, 50);
  );


  size_t best_action = 0;
  
  std::map<size_t, std::vector<CategorizedBallPosition> > actionsConsequences;
  for(size_t i=0; i < action_local.size(); i++)
  {
    // physics simulator
    std::vector<Vector2d> ballPositionResults;
    // this size needs to be exposed
    for(size_t j=0; j < theParameters.numParticles; j++) {
      ballPositionResults.push_back(action_local[i].predict(getBallModel().positionPreview, theParameters.distance_variance, theParameters.angle_variance));
    }
      
    // categorize positions
    std::vector<CategorizedBallPosition> categorizedBallPositionResults;
    categorizePosition(ballPositionResults, categorizedBallPositionResults);

    actionsConsequences.insert(std::pair<size_t, std::vector<CategorizedBallPosition> >(i, categorizedBallPositionResults));
  }
   
  // plot projected actions
  DEBUG_REQUEST("Simulation:ActionTarget",
    for(size_t i=0; i<action_local.size(); i++)
    {
      std::vector<CategorizedBallPosition>::const_iterator ballPosition = actionsConsequences[i].begin();
      for(; ballPosition != actionsConsequences[i].end(); ++ballPosition)
      {
        if(ballPosition->cat() == INFIELD)
        {
          FIELD_DRAWING_CONTEXT;
          PEN("009900", 1);
          Vector2d ball = getRobotPose() * ballPosition->pos();
          FILLOVAL(ball.x, ball.y, 50, 50);
        } else if(ballPosition->cat() == OPPGOAL)
        {
          FIELD_DRAWING_CONTEXT;
          PEN("336600", 1);
          Vector2d ball = getRobotPose() * ballPosition->pos();
          FILLOVAL(ball.x, ball.y, 50, 50);
        } else
        {
          FIELD_DRAWING_CONTEXT;
          PEN("66FF33", 1);
          Vector2d ball = getRobotPose() * ballPosition->pos();
          FILLOVAL(ball.x, ball.y, 50, 50);
        }
      }
    }
  );

  // #### FILTER ####
  // now remove actions with more than threshold precentage of outs
  // also remove actions which result in own-goals
  std::vector<size_t> goodActions;
  for(size_t i=0; i<action_local.size(); i++)
  {
    int good = 0;
    bool ownGoal = false;
    for(std::vector<CategorizedBallPosition>::const_iterator ballPosition = actionsConsequences[i].begin(); ballPosition != actionsConsequences[i].end(); ballPosition++)
    {
      if(ballPosition->cat() == INFIELD || ballPosition->cat() == OPPGOAL)
      {
        good++;
      } else if(ballPosition->cat() == OWNGOAL)
      {
        ownGoal = true;
      }
    }
    // if an own-goal is detected, ignore the action
    if(ownGoal)
    {
      continue;
    }
    // check goal percentage, percentage needs to be exposed
    // the static_cast is messy but I don't know how to get around it
    //goal_percentage = 0.85
    if(good/static_cast<double>(actionsConsequences[i].size()) > theParameters.goal_percentage)
    {
      goodActions.push_back(i);
    }
  }
  // #### EVALUATION ####
  // only continue evaluation if there are good actions
  if(goodActions.size() > 0)
  {
    // now count the goals for the good actions
    std::map<size_t, int> actionsGoals;
    for(std::vector<size_t>::iterator it = goodActions.begin(); it != goodActions.end(); it++)
    {
      int goals = 0;
      for(std::vector<CategorizedBallPosition>::const_iterator ballPosition = actionsConsequences[*it].begin(); ballPosition != actionsConsequences[*it].end(); ballPosition++)
      {
        if(ballPosition->cat() == OPPGOAL)
        {
          goals++;
        }
      }
      if(goals > 0)
      {
        actionsGoals.insert(std::pair<size_t, int>(*it, goals));
      }
    }
    // if there are goals, the best action is the one with the most goals
    if(actionsGoals.size() > 0)
    {
      int mostGoals = 0;
      for(std::map<size_t, int>::iterator it = actionsGoals.begin(); it != actionsGoals.end(); it++)
      {
        if(it->second > mostGoals)
        {
          best_action = it->first;
          mostGoals = it->second;
        }
      }
    }
    else // else choose the best mean of the potential field values
    {
      double bestValue = std::numeric_limits<double>::max(); // assuming potential is [0.0, inf]
      for(std::vector<size_t>::iterator it = goodActions.begin(); it != goodActions.end(); it++)
      {
        double sumPotential = 0.0;
        for(std::vector<CategorizedBallPosition>::const_iterator ballPosition = actionsConsequences[*it].begin(); ballPosition != actionsConsequences[*it].end(); ballPosition++)
        {
          sumPotential += evaluateAction(getRobotPose() * ballPosition->pos());
        }
        // again a static cast because of size_t as I don't know a better solution
        sumPotential /= static_cast<double>(actionsConsequences[*it].size());
        if(sumPotential < bestValue)
        {
          best_action = *it;
          bestValue = sumPotential;
        }
      }
    }
  }

  getKickActionModel().bestAction = action_local[best_action].id();

  DEBUG_REQUEST("Simulation:draw_best_action",
    FIELD_DRAWING_CONTEXT;
    PEN("FF69B4", 7);
    std::string name = action_local[best_action].name();
    TEXT_DRAWING(getRobotPose().translation.x, getRobotPose().translation.y, name);
  );

  DEBUG_REQUEST("Simulation:draw_potential_field",
     draw_potential_field();
  );

}//end execute

void Simulation::categorizePosition(
  const std::vector<Vector2d>& ballPositions, 
  std::vector<CategorizedBallPosition>& categorizedBallPositions
  ) const
{ 
  // HINT: categorizedBallPositions is not necessarily empty here! Maybe we should clear it
  
  // calculate the opponent goal line
  GoalModel::Goal oppGoalModel = getSelfLocGoalModel().getOppGoal(getCompassDirection(), getFieldInfo());
  Vector2d oppGoalPostLeftPreview = getMotionStatus().plannedMotion.hip / oppGoalModel.leftPost;
  Vector2d oppGoalPostRightPreview = getMotionStatus().plannedMotion.hip / oppGoalModel.rightPost;

  Vector2d oppGoalDir = (oppGoalPostRightPreview - oppGoalPostLeftPreview).normalize();
  Vector2d oppLeftEndpoint = oppGoalPostLeftPreview + oppGoalDir*(getFieldInfo().goalpostRadius + getFieldInfo().ballRadius);
  Vector2d oppRightEndpoint = oppGoalPostRightPreview - oppGoalDir*(getFieldInfo().goalpostRadius + getFieldInfo().ballRadius);

  Math::LineSegment oppGoalLinePreview(oppLeftEndpoint, oppRightEndpoint);
  
  // calculate the own goal line
  GoalModel::Goal ownGoalModel = getSelfLocGoalModel().getOwnGoal(getCompassDirection(), getFieldInfo());
  Vector2d ownGoalPostLeftPreview = getMotionStatus().plannedMotion.hip / ownGoalModel.leftPost;
  Vector2d ownGoalPostRightPreview = getMotionStatus().plannedMotion.hip / ownGoalModel.rightPost;

  Vector2d ownGoalDir = (ownGoalPostRightPreview - ownGoalPostLeftPreview).normalize();
  Vector2d ownLeftEndpoint = ownGoalPostLeftPreview + ownGoalDir*(getFieldInfo().goalpostRadius + getFieldInfo().ballRadius);
  Vector2d ownRightEndpoint = ownGoalPostRightPreview - ownGoalDir*(getFieldInfo().goalpostRadius + getFieldInfo().ballRadius);

  Math::LineSegment ownGoalLinePreview(ownLeftEndpoint, ownRightEndpoint);
 
  // now loop through all positions
  for(std::vector<Vector2d>::const_iterator ballPosition = ballPositions.begin(); ballPosition != ballPositions.end(); ++ballPosition)
  {
    Vector2d globalBallPosition = getRobotPose() * (*ballPosition);

    //Schusslinie
    Math::LineSegment shootLine(getBallModel().positionPreview, globalBallPosition);

    BallPositionCategory category = INFIELD;
    // inside field
    if(getFieldInfo().fieldRect.inside(globalBallPosition))
    {
      category = INFIELD;
    }
    // goal!!
    else if(shootLine.intersect(oppGoalLinePreview) && oppGoalLinePreview.intersect(shootLine)) 
    {
      category = OPPGOAL;
    }
    // own goal
    else if(shootLine.intersect(ownGoalLinePreview) && ownGoalLinePreview.intersect(shootLine)) 
    {
      category = OWNGOAL;
    }
    //Opponent Groundline Out - Ball einen Meter hinter Roboter mind anstoß höhe. jeweils seite wo ins ausgeht
    else if(globalBallPosition.x > getFieldInfo().xPosOpponentGroundline)
    {
      category = OPPOUT;
    }
    //Own Groundline out -  an der seite wo raus geht
    else if(globalBallPosition.x < getFieldInfo().xPosOwnGroundline)
    {
      category = OWNOUT;
    }
    //an der linken Seite raus -> ein meter hinter roboter oder wo ins ausgeht ein meter hinter
    else if(globalBallPosition.y > getFieldInfo().yPosLeftSideline )
    {  
      category = LEFTOUT;
    }
    //an der rechten Seite raus -> ein meter hinter roboter oder wo ins ausgeht ein meter hinter
    else if(globalBallPosition.y < getFieldInfo().yPosRightSideline)
    { 
      category = RIGHTOUT;
    }
    CategorizedBallPosition categorizedBallPosition = CategorizedBallPosition(*ballPosition, category);
    categorizedBallPositions.push_back(categorizedBallPosition);
  }
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
