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
  DEBUG_REQUEST_REGISTER("Simulation:OwnGoal","OwnGoal",false);
  DEBUG_REQUEST_REGISTER("Simulation:ObstacleLine","ObstacleLine",false);
  DEBUG_REQUEST_REGISTER("Simulation:ActionTarget:None","DrawNone",false);
  DEBUG_REQUEST_REGISTER("Simulation:ActionTarget:Short","DrawShortKick",false);
  //DEBUG_REQUEST_REGISTER("Simulation:ActionTarget:Long","DrawLongKick",false);
  DEBUG_REQUEST_REGISTER("Simulation:ActionTarget:Left","DrawLeftKick",false);
  DEBUG_REQUEST_REGISTER("Simulation:ActionTarget:Right","DrawRightKick",false);

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

Simulation::~Simulation(){}

void Simulation::execute()
{
  DEBUG_REQUEST("Simulation:use_Parameters",
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
  draw_actions(actionsConsequences);

  
  // now decide which action to execute given their consequences
  STOPWATCH_START("Simulation:decide");
  size_t best_action = decide_smart(actionsConsequences);
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
  ActionResults& categorizedBallPositions
  ) const
{ 
  // just as a safety measure
  //categorizedBallPositions.clear();
  //categorizedBallPositions.reserve(static_cast<int>(theParameters.numParticles));
  categorizedBallPositions.reset();

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
    // small gap between this and the borders of the goalbox
    //check y coordinates and 
    else if(getFieldInfo().fieldRect.inside(globalBallEndPosition) ||
      (globalBallEndPosition.x <= getFieldInfo().opponentGoalPostRight.x 
      && globalBallEndPosition.y > getFieldInfo().opponentGoalPostRight.y 
      && globalBallEndPosition.y < getFieldInfo().opponentGoalPostLeft.y)
      )
      //(globalBallEndPosition.y < getFieldInfo().opponentGoalPostRight.y && globalBallEndPosition.y > getFieldInfo().opponentGoalPostLeft.y)
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
    //CategorizedBallPosition categorizedBallPosition = CategorizedBallPosition(getRobotPose() / globalBallEndPosition, category);
    categorizedBallPositions.add(getRobotPose() / globalBallEndPosition, category);
  }
}

size_t Simulation::decide_smart(const std::vector<ActionResults>& actionsConsequences ) const
{
  std::vector<size_t> bestActions;        // high goal probability
  std::vector<size_t> secondbestActions; // low or no goal probability

  for(size_t i=0; i<action_local.size(); i++)
  {
    const ActionResults& results = actionsConsequences[i];

    // if an own-goal is detected, ignore the action
    if(results.categorie(OWNGOAL) > 0) {
      continue;
    }

    // ignore actions with too high chance of kicking out
    double numberOfInfieldPos = results.categorie(INFIELD) + results.categorie(OPPGOAL);
    double score = numberOfInfieldPos / (double)(results.categorie(NUMBER_OF_BallPositionCategory));
    if(score <= max(0.0, theParameters.good_threshold_percentage)) {
      continue;
    }

    // there is no other action to compare yet
    if(bestActions.empty()) {
      bestActions.push_back(i);
      continue;
    }

    // the actio with the highest chance of scoring the goal is the best
    if(actionsConsequences[bestActions.front()].categorie(OPPGOAL) < results.categorie(OPPGOAL)) {
      bestActions.clear();
      bestActions.push_back(i);
      continue;
    }

    if(actionsConsequences[bestActions.front()].categorie(OPPGOAL) == results.categorie(OPPGOAL)) {
      bestActions.push_back(i);
      continue;
    }
  }

  // there is a clear decision
  if(bestActions.empty()) {
    return 0; //assumes 0 is the turn action
  } else if(bestActions.size() == 1) {
      return bestActions.front();
  }

  // choose the best action by potential field
  size_t best_action = 0;
  double bestValue = std::numeric_limits<double>::max(); // assuming potential is [0.0, inf]
  
  for(std::vector<size_t>::const_iterator it = bestActions.begin(); it != bestActions.end(); ++it)
  {
    const ActionResults& results = actionsConsequences[*it];

    double sumPotential = 0.0;
    for(ActionResults::Positions::const_iterator p = results.positions().begin(); p != results.positions().end(); ++p)
    {
      if(p->cat() == INFIELD || p->cat() == OPPGOAL) {
        sumPotential += evaluateAction(getRobotPose() * p->pos());
      }
    }

    sumPotential /= (double)(results.categorie(INFIELD) + results.categorie(OPPGOAL));

    if(sumPotential < bestValue) {
      best_action = *it;
      bestValue = sumPotential;
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

double Simulation::exp256(const double& x) const
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

double Simulation::gaussian(const double& x, const double& y, const double& muX, const double& muY, const double& sigmaX, const double& sigmaY) const
{
  double facX = (x - muX) * (x - muX) / (2.0 * sigmaX * sigmaX);
  double facY = (y - muY) * (y - muY) / (2.0 * sigmaY * sigmaY);
  return exp256(-1.0 * (facX + facY));
}

double Simulation::slope(const double& x, const double& y, const double& slopeX, const double& slopeY) const
{
  return slopeX * x + slopeY * y;
}

double Simulation::evaluateAction(const Vector2d& a) const
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

void Simulation::draw_actions(const std::vector<ActionResults>& actionsConsequences )const {

DEBUG_REQUEST("Simulation:ActionTarget:None",
  Color color;
  color = color = Color(1.0,1.0,1.0,0.7); //White - None
	std::vector<CategorizedBallPosition>::const_iterator ballPosition = actionsConsequences[0].positions().begin();
	for(; ballPosition != actionsConsequences[0].positions().end(); ++ballPosition)
	{
		if(ballPosition->cat() == INFIELD)
		{
			FIELD_DRAWING_CONTEXT;
      Vector2d ball = getRobotPose() * ballPosition->pos();
      PEN("000000", 1);
		  FILLOVAL(ball.x, ball.y, 50, 50);
		  PEN(color, 1);			  
		  FILLOVAL(ball.x, ball.y, 40, 40);
		} else if(ballPosition->cat() == OPPGOAL)
		{
			FIELD_DRAWING_CONTEXT;
			PEN("336600", 1);
			Vector2d ball = getRobotPose() * ballPosition->pos();
			FILLOVAL(ball.x, ball.y, 50, 50);
		} else //Outside Field
		{
			FIELD_DRAWING_CONTEXT;
			
			Vector2d ball = getRobotPose() * ballPosition->pos();			
      PEN("66FF33", 1);
      FILLOVAL(ball.x, ball.y, 50, 50);
		}
	}
);
DEBUG_REQUEST("Simulation:ActionTarget:Short",
  Color color;
  color = Color(255.0/255,172.0/255,18.0/255,0.7); //orange - kick_short
	std::vector<CategorizedBallPosition>::const_iterator ballPosition = actionsConsequences[1].positions().begin();
	for(; ballPosition != actionsConsequences[1].positions().end(); ++ballPosition)
	{
		if(ballPosition->cat() == INFIELD)
		{
			FIELD_DRAWING_CONTEXT;
      Vector2d ball = getRobotPose() * ballPosition->pos();
      PEN("000000", 1);
			FILLOVAL(ball.x, ball.y, 50, 50);
			PEN(color, 1);			  
			FILLOVAL(ball.x, ball.y, 40, 40);
		} else if(ballPosition->cat() == OPPGOAL)
		{
			FIELD_DRAWING_CONTEXT;
			PEN("336600", 1);
			Vector2d ball = getRobotPose() * ballPosition->pos();
			FILLOVAL(ball.x, ball.y, 50, 50);
		} else
		{
			FIELD_DRAWING_CONTEXT;			
			Vector2d ball = getRobotPose() * ballPosition->pos();			
      PEN("66FF33", 1);
      FILLOVAL(ball.x, ball.y, 50, 50);
		}
	}
);

DEBUG_REQUEST("Simulation:ActionTarget:Left",
    Color color;
    color = Color(0.0/255,13.0/255,191.0/255,0.7); //blue - sidekick_left
		std::vector<CategorizedBallPosition>::const_iterator ballPosition = actionsConsequences[2].positions().begin();
		for(; ballPosition != actionsConsequences[2].positions().end(); ++ballPosition)
		{
			if(ballPosition->cat() == INFIELD)
			{
			  FIELD_DRAWING_CONTEXT;
        Vector2d ball = getRobotPose() * ballPosition->pos();
        PEN("000000", 1);
			  FILLOVAL(ball.x, ball.y, 50, 50);
			  PEN(color, 1);			  
			  FILLOVAL(ball.x, ball.y, 40, 40);
			} else if(ballPosition->cat() == OPPGOAL)
			{
			  FIELD_DRAWING_CONTEXT;
			  PEN("336600", 1);
			  Vector2d ball = getRobotPose() * ballPosition->pos();
			  FILLOVAL(ball.x, ball.y, 50, 50);
			} else
			{
			  FIELD_DRAWING_CONTEXT;			  
			  Vector2d ball = getRobotPose() * ballPosition->pos();			  
        PEN("66FF33", 1);
        FILLOVAL(ball.x, ball.y, 50, 50);
			}
		}
);
DEBUG_REQUEST("Simulation:ActionTarget:Right",
  Color color;
  color = Color(0.0/255,191.0/255,51.0/255,0.7);//green - sidekick_right
	std::vector<CategorizedBallPosition>::const_iterator ballPosition = actionsConsequences[3].positions().begin();
	for(; ballPosition != actionsConsequences[3].positions().end(); ++ballPosition)
	{
		if(ballPosition->cat() == INFIELD)
		{
			FIELD_DRAWING_CONTEXT;
      Vector2d ball = getRobotPose() * ballPosition->pos();
      PEN("000000", 1);
			FILLOVAL(ball.x, ball.y, 50, 50);
			PEN(color, 1);			
			FILLOVAL(ball.x, ball.y, 40, 40);
		} else if(ballPosition->cat() == OPPGOAL)
		{
			FIELD_DRAWING_CONTEXT;
			PEN("336600", 1);
			Vector2d ball = getRobotPose() * ballPosition->pos();
			FILLOVAL(ball.x, ball.y, 50, 50);
		} else
		{
			FIELD_DRAWING_CONTEXT;			
			Vector2d ball = getRobotPose() * ballPosition->pos();			
      PEN("66FF33", 1);
      FILLOVAL(ball.x, ball.y, 50, 50);
		}
	}
);
}
