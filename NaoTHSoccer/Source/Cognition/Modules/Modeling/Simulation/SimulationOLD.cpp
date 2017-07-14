/**
* @file Simulation.cpp
* @author <a href="mailto:schlottb@informatik.hu-berlin.de">Benjamin Schlotter</a>
* Implementation of class SimulationOLD
*/

#include "SimulationOLD.h"

using namespace naoth;
using namespace std;

SimulationOLD::SimulationOLD()
 // : obstacleFilter(0.01, 0.1)
{
  DEBUG_REQUEST_REGISTER("SimulationOLD:draw_ball","draw_ball", false);
  DEBUG_REQUEST_REGISTER("SimulationOLD:draw_goal_collisions", "", false);
  DEBUG_REQUEST_REGISTER("SimulationOLD:ActionTarget","ActionTarget", false);
  DEBUG_REQUEST_REGISTER("SimulationOLD:draw_best_action","best action",false);
  DEBUG_REQUEST_REGISTER("SimulationOLD:draw_potential_field","Draw Potential Field",false);
  DEBUG_REQUEST_REGISTER("SimulationOLD:use_Parameters","use_Parameters",false);
  DEBUG_REQUEST_REGISTER("SimulationOLD:OwnGoal","OwnGoal",false);
  DEBUG_REQUEST_REGISTER("SimulationOLD:ObstacleLine","ObstacleLine",false);

  DEBUG_REQUEST_REGISTER("SimulationOLD:ActionTarget:None","DrawNone",false);
  DEBUG_REQUEST_REGISTER("SimulationOLD:ActionTarget:Short","DrawShortKick",false);
  //DEBUG_REQUEST_REGISTER("SimulationOLD:ActionTarget:Long","DrawLongKick",false);
  DEBUG_REQUEST_REGISTER("SimulationOLD:ActionTarget:Left","DrawLeftKick",false);
  DEBUG_REQUEST_REGISTER("SimulationOLD:ActionTarget:Right","DrawRightKick",false);

  getDebugParameterList().add(&theParameters);

  //calculate the actions  
  action_local.reserve(KickActionModel::numOfActions);

  action_local.push_back(Action(KickActionModel::none, ActionParams(), theParameters.friction));
  action_local.push_back(Action(KickActionModel::kick_short, theParameters.kick_short, theParameters.friction)); // short
  //action_local.push_back(Action(KickActionModel::kick_long, theParameters.kick_long, theParameters.friction)); // long
  action_local.push_back(Action(KickActionModel::sidekick_left, theParameters.sidekick_left, theParameters.friction)); // left
  action_local.push_back(Action(KickActionModel::sidekick_right, theParameters.sidekick_right, theParameters.friction)); // right
  
  actionsConsequences.resize(action_local.size());
}

SimulationOLD::~SimulationOLD()
{
  getDebugParameterList().remove(&theParameters);
}

void SimulationOLD::execute()
{
  //obstacleFilter.setParameter(theParameters.obstacleFilter.g0, theParameters.obstacleFilter.g1);
  //obstacleFilter.update(getObstacleModel().frontDistance < 400, 0.3, 0.7);

  DEBUG_REQUEST("SimulationOLD:use_Parameters",
    action_local.clear();
    action_local.reserve(KickActionModel::numOfActions);

    action_local.push_back(Action(KickActionModel::none, ActionParams(), theParameters.friction));
    action_local.push_back(Action(KickActionModel::kick_short, theParameters.kick_short, theParameters.friction)); // short
    //action_local.push_back(Action(KickActionModel::kick_long, theParameters.kick_long, theParameters.friction)); // long
    action_local.push_back(Action(KickActionModel::sidekick_left, theParameters.sidekick_left, theParameters.friction)); // left
    action_local.push_back(Action(KickActionModel::sidekick_right, theParameters.sidekick_right, theParameters.friction)); // right

    actionsConsequences.resize(action_local.size());
  );

  //Proceed with Calculations only if ball is seen in the last 1 second
  if(!getBallModel().valid || getFrameInfo().getTimeInSeconds() >= getBallModel().getFrameInfoWhenBallWasSeen().getTimeInSeconds() + 1)
  {
    return;
  }

  // draw the ball
  DEBUG_REQUEST("SimulationOLD:draw_ball",
    FIELD_DRAWING_CONTEXT;
    PEN("FF0000", 1);
    Vector2d ball = getRobotPose() * getBallModel().positionPreview;
    CIRCLE( ball.x, ball.y, 50);
  );

  
  // simulate the consequences for all actions
  STOPWATCH_START("SimulationOLD:simulateConsequences");
  for(size_t i=0; i < action_local.size(); i++) {
    simulateConsequences(action_local[i], actionsConsequences[i]);
  }
  STOPWATCH_STOP("SimulationOLD:simulateConsequences");


  // plot projected actions
  DEBUG_REQUEST("SimulationOLD:ActionTarget:None", draw_action_results(actionsConsequences[0], Color(1.0,1.0,1.0,0.7)); );
  DEBUG_REQUEST("SimulationOLD:ActionTarget:Short", draw_action_results(actionsConsequences[1], Color(255.0/255,172.0/255,18.0/255,0.7)); );
  //DEBUG_REQUEST("SimulationOLD:ActionTarget:Long", draw_action_results(actionsConsequences[-1], Color(1.0,1.0,1.0,0.7)); );
  DEBUG_REQUEST("SimulationOLD:ActionTarget:Left", draw_action_results(actionsConsequences[2], Color(0.0/255,13.0/255,191.0/255,0.7)); );
  DEBUG_REQUEST("SimulationOLD:ActionTarget:Right", draw_action_results(actionsConsequences[3], Color(0.0/255,191.0/255,51.0/255,0.7)); );

  
  // now decide which action to execute given their consequences
  STOPWATCH_START("SimulationOLD:decide");
  size_t best_action = decide_smart(actionsConsequences);
  STOPWATCH_STOP("SimulationOLD:decide");  

  getKickActionModel().bestAction = action_local[best_action].id();
  //Arghhh BUG: this expected ball pos is not affected by goal boarders/ obstacles
  getKickActionModel().expectedBallPos = getRobotPose() * action_local[best_action].predict(getBallModel().positionPreview, false);

  DEBUG_REQUEST("SimulationOLD:draw_best_action",
    FIELD_DRAWING_CONTEXT;
    /*
    PEN("FF69B4", 35);
    std::string name = action_local[best_action].name();
    TEXT_DRAWING(getRobotPose().translation.x+100, getRobotPose().translation.y-200, name);
    */
    PEN("000000", 1);
    Vector2d expectedBallPos = getKickActionModel().expectedBallPos;
    FILLOVAL(expectedBallPos.x, expectedBallPos.y, 50, 50);
    
    Vector2d globalBall = getRobotPose() *getBallModel().positionPreview;
    Vector2d action_vector = (expectedBallPos - globalBall).normalize();
    Vector2d from = globalBall + action_vector*100;
    Vector2d to = globalBall + action_vector*350;
    
    PEN("FF0000", 50);
    ARROW(from.x, from.y, to.x, to.y);
  );

  DEBUG_REQUEST("SimulationOLD:draw_potential_field",
     draw_potential_field();
  );

}//end execute

void SimulationOLD::simulateConsequences(
  const Action& action,
  ActionResults& categorizedBallPositions
  ) const
{ 
  // just as a safety measure
  //categorizedBallPositions.clear();
  //categorizedBallPositions.reserve(static_cast<int>(theParameters.numParticles));
  categorizedBallPositions.reset();

  // calculate the own goal line

  //TODO move to field Info as well
  Vector2d ownGoalDir = (getFieldInfo().ownGoalPostRight - getFieldInfo().ownGoalPostLeft).normalize();
  Vector2d ownLeftEndpoint = getFieldInfo().ownGoalPostLeft + ownGoalDir*(getFieldInfo().goalpostRadius + getFieldInfo().ballRadius);
  Vector2d ownRightEndpoint = getFieldInfo().ownGoalPostRight - ownGoalDir*(getFieldInfo().goalpostRadius + getFieldInfo().ballRadius);
  Math::LineSegment ownGoalLineGlobal(ownLeftEndpoint, ownRightEndpoint);

  // draw own goal line
  DEBUG_REQUEST("SimulationOLD:OwnGoal",
    FIELD_DRAWING_CONTEXT;
    PEN("DADADA", 7);
    LINE(ownGoalLineGlobal.begin().x,ownGoalLineGlobal.begin().y,
         ownGoalLineGlobal.end().x,ownGoalLineGlobal.end().y);
  );
  
  vector<Math::LineSegment> goalBackSides;
  goalBackSides.reserve(3);
  goalBackSides.push_back(Math::LineSegment(getFieldInfo().oppGoalBackLeft, getFieldInfo().oppGoalBackRight));
  goalBackSides.push_back(Math::LineSegment(getFieldInfo().opponentGoalPostLeft, getFieldInfo().oppGoalBackLeft));
  goalBackSides.push_back(Math::LineSegment(getFieldInfo().opponentGoalPostRight, getFieldInfo().oppGoalBackRight));
   
  // current ball position
  Vector2d globalBallStartPosition = getRobotPose() * getBallModel().positionPreview;
  
  // virtual ultrasound obstacle line
  Math::LineSegment obstacleLine(getRobotPose() * Vector2d(400, 200), getRobotPose() * Vector2d(400, -200));
  // now generate predictions and categorize
  for(size_t j=0; j < static_cast<size_t>(theParameters.numParticles); j++)
  {
    // predict and calculate shoot line
    Vector2d globalBallEndPosition = getRobotPose() * action.predict(getBallModel().positionPreview, true);

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
        DEBUG_REQUEST("SimulationOLD:draw_goal_collisions",
          FIELD_DRAWING_CONTEXT;
        
          PEN("000000", 10);
          BOX(getFieldInfo().oppGoalRect.min().x, getFieldInfo().oppGoalRect.min().y, getFieldInfo().oppGoalRect.max().x, getFieldInfo().oppGoalRect.max().y);

          if (getFieldInfo().oppGoalRect.inside(globalBallEndPosition)) {
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
    /*
    if(obstacleFilter.value() && getObstacleModel().blockedTime > 100)
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
    */

    // now categorize the position
    BallPositionCategory category = INFIELD;
    // goal!!
    if (getFieldInfo().oppGoalRect.inside(globalBallEndPosition))
    {
      category = OPPGOAL;
    }
    // obstacle collision
    else if(obstacleCollision && obstacleLine.intersect(shootLine) && shootLine.intersect(obstacleLine))
    { 
      category = COLLISION;
    }
    // inside field
    // small gap between this and the borders of the goalbox
    //check y coordinates and 
    else if(getFieldInfo().fieldRect.inside(globalBallEndPosition) ||
      (globalBallEndPosition.x <= getFieldInfo().opponentGoalPostRight.x 
      && globalBallEndPosition.y > getFieldInfo().opponentGoalPostRight.y 
      && globalBallEndPosition.y < getFieldInfo().opponentGoalPostLeft.y)
      )
    {
      category = INFIELD;
    }
    // own goal
    else if(shootLine.intersect(ownGoalLineGlobal) && ownGoalLineGlobal.intersect(shootLine)) 
    {
      category = OWNGOAL;
    }
    //Opponent Groundline Out - Ball einen Meter hinter Roboter mind anstoss hoehe. jeweils seite wo ins ausgeht
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
    else if(globalBallEndPosition.y > getFieldInfo().yPosLeftSideline)
    {  
      category = LEFTOUT;
    }
    //an der rechten Seite raus -> ein meter hinter roboter oder wo ins ausgeht ein meter hinter
    else if(globalBallEndPosition.y < getFieldInfo().yPosRightSideline)
    { 
      category = RIGHTOUT;
    }

    // save the calculated end position and category, i.e., the consequence
    //CategorizedBallPosition categorizedBallPosition = CategorizedBallPosition(getRobotPose() / globalBallEndPosition, category);
    categorizedBallPositions.add(getRobotPose() / globalBallEndPosition, category);
  }
}

size_t SimulationOLD::decide_smart(const std::vector<ActionResults>& actionsConsequences) const
{
  std::vector<size_t> acceptableActions;

  // select acceptable actions
  for(size_t i = 0; i < actionsConsequences.size(); ++i)
  {
    const ActionResults& results = actionsConsequences[i];

    // if an own-goal is detected, ignore the action
    if(results.category(OWNGOAL) > 0) {
      continue;
    }

    // ignore actions with too high chance of kicking out
    double score = results.likelihood(INFIELD) + results.likelihood(OPPGOAL);
    if(score <= max(0.0, theParameters.good_threshold_percentage)) {
      continue;
    }

    //all actions which are not too bad
    acceptableActions.push_back(i);
  }

  // no acceptable actions
  if(acceptableActions.empty()) {
    return 0; //assumes 0 is the turn action
  }
  // there is a clear decision
  if(acceptableActions.size() == 1) {
    return acceptableActions.front();
  }

  // select the subset of actions with highest goal chance
  std::vector<size_t> goalActions;
  for(size_t i = 0; i < acceptableActions.size(); ++i)
  {
    const ActionResults& results = actionsConsequences[acceptableActions[i]];

    // chance of scoring a goal must be significant
    if(results.category(OPPGOAL) < theParameters.minGoalParticles) {
      continue;
    }

    // there is no other action to compare yet
    if(goalActions.empty()) {
      goalActions.push_back(acceptableActions[i]);
      continue;
    }

    // the action with the highest chance of scoring the goal is the best
    if(actionsConsequences[goalActions.front()].category(OPPGOAL) < results.category(OPPGOAL)) {
      goalActions.clear();
      goalActions.push_back(acceptableActions[i]);
      continue;
    }

    if(actionsConsequences[goalActions.front()].category(OPPGOAL) == results.category(OPPGOAL)) {
      goalActions.push_back(acceptableActions[i]);
      continue;
    }
  }

  // go for the goal action!
  if(goalActions.size() == 1) {
    return goalActions.front();
  }

  // no goal actions => select one of the acceptable actions based on strategic value
  if(goalActions.empty()) 
  {
    size_t best_action = 0;
    double bestValue = std::numeric_limits<double>::max(); // assuming potential is [0.0, inf]
    for(std::vector<size_t>::const_iterator it = acceptableActions.begin(); it != acceptableActions.end(); ++it)
    {
      double potential = evaluateAction(actionsConsequences[*it]);
      if(potential < bestValue) {
        best_action = *it;
        bestValue = potential;
      }
    }
    return best_action;
  }
  
  // find min of goalActions
  size_t best_action = 0;
  double bestValue = std::numeric_limits<double>::max(); // assuming potential is [0.0, inf]
  for(std::vector<size_t>::const_iterator it = goalActions.begin(); it != goalActions.end(); ++it)
  {
    double potential = evaluateAction(actionsConsequences[*it]);
    if(potential < bestValue) {
      best_action = *it;
      bestValue = potential;
    }
  }
  return best_action;
}

//correction of distance in percentage, angle in degrees
Vector2d SimulationOLD::Action::predict(const Vector2d& ball, bool noise) const
{
	double gforce = Math::g*1e3; // mm/s^2
	double distance;
	double angle;
	if (noise){
		double speed = Math::generateGaussianNoise(action_speed, action_speed_std);
		angle = Math::generateGaussianNoise(Math::fromDegrees(action_angle), Math::fromDegrees(action_angle_std));
		distance = speed*speed / friction / gforce / 2.0; // friction*mass*gforce*distance = 1/2*mass*speed*speed
	}
	else{
		distance = action_speed*action_speed / friction / gforce / 2.0; // friction*mass*gforce*distance = 1/2*mass*speed*speed
		angle = Math::fromDegrees(action_angle);
	}  
  Vector2d predictedAction(distance, 0.0);
  predictedAction.rotate(angle);

  return ball + predictedAction;
}

double SimulationOLD::exp256(const double& x) const
{
  // exp(x) = lim(n->inf) (1 + x/n)^n
  // for n=256 about 10x faster than exp but around 2.5 % off on x in [-10, 10]
  double y = 1.0 + x / 256.0;
  y *= y;
  y *= y;
  y *= y;
  y *= y;
  y *= y;
  y *= y;
  y *= y;
  y *= y;
  return y;
}

double SimulationOLD::gaussian(const double& x, const double& y, const double& muX, const double& muY, const double& sigmaX, const double& sigmaY) const
{
  double facX = (x - muX) * (x - muX) / (2.0 * sigmaX * sigmaX);
  double facY = (y - muY) * (y - muY) / (2.0 * sigmaY * sigmaY);
  return exp256(-1.0 * (facX + facY));
}

double SimulationOLD::slope(const double& x, const double& y, const double& slopeX, const double& slopeY) const
{
  return slopeX * x + slopeY * y;
}

double SimulationOLD::evaluateAction(const Vector2d& a) const
{
  double xPosOpponentGoal = getFieldInfo().xPosOpponentGoal;
  double yPosLeftSideline = getFieldInfo().yPosLeftSideline;
  double xPosOwnGoal = getFieldInfo().xPosOwnGoal;

  double sigmaX = xPosOpponentGoal/2.0;
  double sigmaY = yPosLeftSideline/2.5;
  double slopeX = -1.0/xPosOpponentGoal;
  
  double f = 0.0;
  f += slope(a.x, a.y, slopeX, 0.0);
  f -= gaussian(a.x, a.y, xPosOpponentGoal, 0.0, sigmaX, sigmaY);
  f += gaussian(a.x, a.y, xPosOwnGoal, 0.0, 1.5*sigmaX, sigmaY);
  
  return f;
}

double SimulationOLD::evaluateAction(const ActionResults& results) const
{
  double sumPotential = 0.0;
  double numberOfActions = 0.0;
  for(ActionResults::Positions::const_iterator p = results.positions().begin(); p != results.positions().end(); ++p)
  {
    if(p->cat() == INFIELD || p->cat() == OPPGOAL) {
      sumPotential += evaluateAction(getRobotPose() * p->pos());
      numberOfActions++;
    }
  }

  ASSERT(numberOfActions > 0);
  sumPotential /= numberOfActions;
  return sumPotential;
}

void SimulationOLD::draw_potential_field() const
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
  
  // TODO: replace by std::minmax_element in C++11
  double maxValue = *std::max_element(potential.begin(), potential.end());
  double minValue = *std::min_element(potential.begin(), potential.end());
  double range = maxValue - minValue;

  if(range == 0) { return; }

  idx = 0;
  for (int x = 0; x < xSize; x++) {
    for (int y = 0; y < ySize; y++)
    {
      Vector2d point(xWidth*(2*x-xSize+1), yWidth*(2*y-ySize+1));
      
      double t = (potential[idx++] - minValue) / range;
      Color color = black*t + white*(1-t);
      PEN(color, 20);
      FILLBOX(point.x - xWidth, point.y - yWidth, point.x+xWidth, point.y+yWidth);
    }
  }
}//end draw_closest_points


void SimulationOLD::draw_action_results(const ActionResults& actionsResults, const Color& color) const
{
  FIELD_DRAWING_CONTEXT;
  std::vector<CategorizedBallPosition>::const_iterator ballPosition = actionsResults.positions().begin();
  for(; ballPosition != actionsResults.positions().end(); ++ballPosition)
  {
    Vector2d ball = getRobotPose() * ballPosition->pos();
	  if(ballPosition->cat() == INFIELD)
	  {
      PEN("000000", 1);
		  FILLOVAL(ball.x, ball.y, 50, 50);
		  PEN(color, 1);			  
		  FILLOVAL(ball.x, ball.y, 40, 40);
	  } 
    else if(ballPosition->cat() == OPPGOAL)
	  {
		  PEN("336600", 1);
		  FILLOVAL(ball.x, ball.y, 50, 50);
	  } 
    else //Outside Field
	  {		
      PEN("66FF33", 1);
      FILLOVAL(ball.x, ball.y, 50, 50);
	  }
  }
}
