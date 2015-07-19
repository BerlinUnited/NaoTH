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
  DEBUG_REQUEST_REGISTER("Simulation:draw_goal_collisions", "", false);
  DEBUG_REQUEST_REGISTER("Simulation:ActionTarget","ActionTarget", false);
  DEBUG_REQUEST_REGISTER("Simulation:draw_best_action","best action",false);
  DEBUG_REQUEST_REGISTER("Simulation:draw_potential_field","Draw Potential Field",false);
  DEBUG_REQUEST_REGISTER("Simulation:use_Parameters","use_Parameters",false);
  DEBUG_REQUEST_REGISTER("Simulation:OppGoal","OppGoal",false);
  DEBUG_REQUEST_REGISTER("Simulation:OwnGoal","OwnGoal",false);
  DEBUG_REQUEST_REGISTER("Simulation:ObstacleLine","ObstacleLine",false);
  

  getDebugParameterList().add(&theParameters);

  //calculate the actions  
  action_local.reserve(KickActionModel::numOfActions);

  action_local.push_back(Action(KickActionModel::none, ActionParams(), theParameters.friction));
  action_local.push_back(Action(KickActionModel::kick_long, theParameters.kick_long, theParameters.friction)); // long
  action_local.push_back(Action(KickActionModel::kick_short, theParameters.kick_short, theParameters.friction)); // short
  action_local.push_back(Action(KickActionModel::sidekick_right, theParameters.sidekick_right, theParameters.friction)); // right
  action_local.push_back(Action(KickActionModel::sidekick_left, theParameters.sidekick_left, theParameters.friction)); // left

  actionsConsequences.resize(action_local.size());
}

Simulation::~Simulation(){}

void Simulation::execute()
{
  DEBUG_REQUEST("Simulation:use_Parameters",
    action_local.clear();
    action_local.reserve(KickActionModel::numOfActions);

    action_local.push_back(Action(KickActionModel::none, ActionParams(), theParameters.friction));
    action_local.push_back(Action(KickActionModel::kick_long, theParameters.kick_long, theParameters.friction)); // long
    action_local.push_back(Action(KickActionModel::kick_short, theParameters.kick_short, theParameters.friction)); // short
    action_local.push_back(Action(KickActionModel::sidekick_right, theParameters.sidekick_right, theParameters.friction)); // right
    action_local.push_back(Action(KickActionModel::sidekick_left, theParameters.sidekick_left, theParameters.friction)); // left

    actionsConsequences.resize(action_local.size());
  );

  //Proceed with Calculations only if ball is seen in the last 1 second
  if(!getBallModel().valid || getFrameInfo().getTimeInSeconds() >= getBallModel().frameInfoWhenBallWasSeen.getTimeInSeconds() + 1)
  {
    return;
  }

  // draw the ball
  DEBUG_REQUEST("Simulation:draw_ball",
    FIELD_DRAWING_CONTEXT;
    PEN("FF0000", 1);
    Vector2d ball = getRobotPose() * getBallModel().positionPreview;
    CIRCLE( ball.x, ball.y, 50);
  );


  // simulate the consequences for all actions
  STOPWATCH_START("Simulation:simulateConsequences");
  for(size_t i=0; i < action_local.size(); i++) {
    simulateConsequences(action_local[i], actionsConsequences[i]);
  }
  STOPWATCH_STOP("Simulation:simulateConsequences");
   
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
  
  // now decide which action to execture given their consequences
  STOPWATCH_START("Simulation:decide");
  size_t best_action = decide(actionsConsequences);
  STOPWATCH_STOP("Simulation:decide");

  getKickActionModel().bestAction = action_local[best_action].id();

  DEBUG_REQUEST("Simulation:draw_best_action",
    FIELD_DRAWING_CONTEXT;
    PEN("FF69B4", 35);
    std::string name = action_local[best_action].name();
    TEXT_DRAWING(getRobotPose().translation.x+100, getRobotPose().translation.y-200, name);
  );

  DEBUG_REQUEST("Simulation:draw_potential_field",
     draw_potential_field();
  );

}//end execute

void Simulation::simulateConsequences(
  const Action& action,
  std::vector<CategorizedBallPosition>& categorizedBallPositions
  ) const
{ 
  // just as a safety measure
  categorizedBallPositions.clear();
  categorizedBallPositions.reserve(static_cast<int>(theParameters.numParticles));

  // calculate the own goal line
  Vector2d ownGoalDir = (getFieldInfo().ownGoalPostRight - getFieldInfo().ownGoalPostLeft).normalize();
  Vector2d ownLeftEndpoint = getFieldInfo().ownGoalPostLeft + ownGoalDir*(getFieldInfo().goalpostRadius + getFieldInfo().ballRadius);
  Vector2d ownRightEndpoint = getFieldInfo().ownGoalPostRight - ownGoalDir*(getFieldInfo().goalpostRadius + getFieldInfo().ballRadius);
  Math::LineSegment ownGoalLineGlobal(ownLeftEndpoint, ownRightEndpoint);

  // draw own goal line
  DEBUG_REQUEST("Simulation:OwnGoal",
    FIELD_DRAWING_CONTEXT;
    PEN("DADADA", 7);
    LINE(ownGoalLineGlobal.begin().x,ownGoalLineGlobal.begin().y,
         ownGoalLineGlobal.end().x,ownGoalLineGlobal.end().y);
  );

  // calculate opponent goal lines and box
  Vector2d oppGoalBackLeft(getFieldInfo().opponentGoalPostLeft.x + getFieldInfo().goalDepth, getFieldInfo().opponentGoalPostLeft.y);
  Vector2d oppGoalBackRight(getFieldInfo().opponentGoalPostRight.x + getFieldInfo().goalDepth, getFieldInfo().opponentGoalPostRight.y);
  
  vector<Math::LineSegment> goalBackSides;
  goalBackSides.reserve(3);
  goalBackSides.push_back(Math::LineSegment(oppGoalBackLeft, oppGoalBackRight));
  goalBackSides.push_back(Math::LineSegment(getFieldInfo().opponentGoalPostLeft, oppGoalBackLeft));
  goalBackSides.push_back(Math::LineSegment(getFieldInfo().opponentGoalPostRight, oppGoalBackRight));

  Geometry::Rect2d oppGoalBox(oppGoalBackRight, getFieldInfo().opponentGoalPostLeft);
   
  // current ball position
  Vector2d globalBallStartPosition = getRobotPose() * getBallModel().positionPreview;
  
  // virtual ultrasound obstacle line
  Math::LineSegment obstacleLine(getRobotPose() * Vector2d(400, 200), getRobotPose() * Vector2d(400, -200));
  // now generate predictions and categorize
  for(size_t j=0; j < theParameters.numParticles; j++) 
  {
    // predict and calculate shoot line
    Vector2d globalBallEndPosition = getRobotPose() * action.predict(getBallModel().positionPreview);
    Math::LineSegment shootLine(globalBallStartPosition, globalBallEndPosition);

    // check if collision detection with goal has to be performed
    // if the ball start and end positions are inside of the field, you don't need to check
    if(!getFieldInfo().fieldRect.inside(globalBallEndPosition) || !getFieldInfo().fieldRect.inside(globalBallStartPosition))
    {
      // collide with goal
      double t_min = shootLine.getLength();
      bool collisionWithGoal = false;
      for(size_t i = 0; i < goalBackSides.size(); i++) {
        const Math::LineSegment& side = goalBackSides[i];
        double t = shootLine.Line::intersection(side);

        if(t >= 0 && t < t_min && side.intersect(shootLine)) {
          t_min = t;
          collisionWithGoal = true;
        }
      }

      // if there are collisions with the back goal lines, calulate where the ball will stop
      if(collisionWithGoal) {
        shootLine = Math::LineSegment(globalBallStartPosition, shootLine.point(t_min-getFieldInfo().ballRadius));
        globalBallEndPosition = shootLine.end();
        
        // draw balls and goal box if there are collisions
        DEBUG_REQUEST("Simulation:draw_goal_collisions",
          FIELD_DRAWING_CONTEXT;
        
          PEN("000000", 10);
          BOX(oppGoalBox.min().x,oppGoalBox.min().y,oppGoalBox.max().x,oppGoalBox.max().y);

          if(oppGoalBox.inside(globalBallEndPosition)) {
            PEN("0000AA66", 1);
          } else {
            PEN("FF00AA66", 1);
          }
          FILLOVAL(shootLine.end().x, shootLine.end().y, 50, 50);
        );
      }
    }
    
    // Obstacle Detection
    bool obstacleCollision = false;
    if(getObstacleModel().frontDistance < 400 && getObstacleModel().blockedTime > 100)
    {
      obstacleCollision = true;
      // draw obstacle line
      DEBUG_REQUEST("Simulation:ObstacleLine",
        FIELD_DRAWING_CONTEXT;
        PEN("000000", 25);
        LINE(obstacleLine.begin().x, obstacleLine.begin().y,
             obstacleLine.end().x, obstacleLine.end().y);
      );
    }

    // now categorize the position
    BallPositionCategory category = INFIELD;
    // goal!!
    if(oppGoalBox.inside(globalBallEndPosition)) 
    {
      category = OPPGOAL;
    }
    // obstacle collision
    else if(obstacleCollision && obstacleLine.intersect(shootLine) && shootLine.intersect(obstacleLine))
    { 
      category = COLLISION;
    }
    // inside field
    else if(getFieldInfo().fieldRect.inside(globalBallEndPosition))
    {
      category = INFIELD;
    }
    // own goal
    else if(shootLine.intersect(ownGoalLineGlobal) && ownGoalLineGlobal.intersect(shootLine)) 
    {
      category = OWNGOAL;
    }
    //Opponent Groundline Out - Ball einen Meter hinter Roboter mind anstoß höhe. jeweils seite wo ins ausgeht
    else if(globalBallEndPosition.x > getFieldInfo().xPosOpponentGroundline)
    {
      category = OPPOUT;
    }
    //Own Groundline out -  an der seite wo raus geht
    else if(globalBallEndPosition.x < getFieldInfo().xPosOwnGroundline)
    {
      category = OWNOUT;
    }
    //an der linken Seite raus -> ein meter hinter roboter oder wo ins ausgeht ein meter hinter
    else if(globalBallEndPosition.y > getFieldInfo().yPosLeftSideline )
    {  
      category = LEFTOUT;
    }
    //an der rechten Seite raus -> ein meter hinter roboter oder wo ins ausgeht ein meter hinter
    else if(globalBallEndPosition.y < getFieldInfo().yPosRightSideline)
    { 
      category = RIGHTOUT;
    }

    // save the calculated end position and category, i.e., the consequence
    CategorizedBallPosition categorizedBallPosition = CategorizedBallPosition(getRobotPose() / globalBallEndPosition, category);
    categorizedBallPositions.push_back(categorizedBallPosition);
  }
}

size_t Simulation::decide(
  const std::vector<std::vector<CategorizedBallPosition> >& actionsConsequences
) const
{
  // TAKE CARE: This assumes that none is the first action!
  size_t best_action = 0;

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
    if(good/static_cast<double>(actionsConsequences[i].size()) > theParameters.good_threshold_percentage)
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
      // find number of goals for best action(s)
      int mostGoals = 0;
      for(std::map<size_t, int>::iterator it = actionsGoals.begin(); it != actionsGoals.end(); it++)
      {
        if(it->second > mostGoals)
        {
          mostGoals = it->second;
        }
      }
      // now check how many actions lead to this number of goals
      std::vector<size_t> bestActions;
      for(std::map<size_t, int>::iterator it = actionsGoals.begin(); it != actionsGoals.end(); it++)
      {
        if(it->second == mostGoals)
        {
          bestActions.push_back(it->first);
        }
      }
      if(bestActions.size() > 1)
      {
        // find the action with the most goals and the best potential field
        // THIS IS STILL WRONG BECAUSE BALLS ARE NOT KEPT IN THE GOAL
        best_action = bestActions[0];
        double bestValue = std::numeric_limits<double>::max(); // assuming potential is [0.0, inf]
        for(std::vector<size_t>::iterator it = bestActions.begin(); it != bestActions.end(); it++)
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
      } else
      {
        best_action = bestActions[0];
      }
    } else // else choose the best mean of the potential field values
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
  return best_action;
}

//correction of distance in percentage, angle in degrees
Vector2d Simulation::Action::predict(const Vector2d& ball) const
{
  double gforce = Math::g*1e3; // mm/s^2
  double speed = Math::generateGaussianNoise(action_speed, action_speed_std);
  double distance = speed*speed/friction/gforce/2.0; // friction*mass*gforce*distance = 1/2*mass*speed*speed
  double angle = Math::generateGaussianNoise(Math::fromDegrees(action_angle), Math::fromDegrees(action_angle_std)); 
  Vector2d noisyAction(distance, 0.0);
  noisyAction.rotate(angle);

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
//  Vector2d oppGoal(getFieldInfo().xPosOpponentGoal+getFieldInfo().goalDepth, 0.0);
//  Vector2d oppDiff = oppGoal - a;
//
//  double oppValueX = 0.1;
//  double oppValueY = 1;
//  MODIFY("Simulation:oppValueX", oppValueX);
//  MODIFY("Simulation:oppValueY", oppValueY);
  double x = a.x;
  double y = a.x;
  double value;
  if(x < -1500)
    value = 0.1*(x-4500)*(x-4500) + fabs(x-1500)/4500*y*y;
  else
    value = -1.0*(0.1*(x+4500)*(x+4500) + fabs(x-1500)/4500*y*y);
  return -value;
  
  //double value_opp = sqrt(oppValueX*oppDiff.x*oppDiff.x + oppValueY*oppDiff.y*oppDiff.y)+abs(oppDiff.y)+oppDiff.x;
  //double value_opp = oppValueX*oppDiff.x*oppDiff.x + oppValueY*oppDiff.y*oppDiff.y;
  //double value_opp = abs(oppDiff.y)+oppDiff.x;
  //Vector2d ownGoal(getFieldInfo().xPosOwnGoal, 0.0);
  //Vector2d ownDiff = ownGoal - a;
  
  //double ownValueX = 0.01;
  //double ownValueY = 0.1;
  //MODIFY("Simulation:ownValueX", ownValueX);
  //MODIFY("Simulation:ownValueY", ownValueY);
  //double value_own = ownValueX*ownDiff.x*ownDiff.x + ownValueY*ownDiff.y*ownDiff.y;

  //return value_opp - value_own;
  //return value_opp;
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
